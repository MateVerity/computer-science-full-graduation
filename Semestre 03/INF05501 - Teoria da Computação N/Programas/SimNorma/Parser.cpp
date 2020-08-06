/********************************************************************
	IMPLEMENTA��O DA CLASSE PARSER
	Cria��o:	30/05/2003
	Copyright:	(c) 2003 Marcelo Bona Boff e Fernando Trebien
********************************************************************/


#include "stdafx.h"
#include "Parser.h"
#include "ParserData.h"


/********************************************************************
	CLASSE PARSER */

PARSER::PARSER ()
{
	// PARSER - cria a classe de parsing
	pLibs = NULL;
	nLibsCount = 0;
	err_libname = err_source = err_pos = err_undefdata = NULL;
}

PARSER::~PARSER ()
{
	// ~PARSER - destr�i a classe de parsing
	ClearLibs ();
}

void PARSER::get_token ()
{
	// get_token - pega e identifica o proximo token
	char *tok;
	TOK_TYPE last_tok_type;

	// indica que n�o � o incio da linha
	new_line = false;

	// ignora espa�os, tabs, enters
	while ((*prog == ' ') || (*prog == '\t') || (*prog == 13) || (*prog == 10) || (*prog == -51) || (*prog == '{'))
	{
		if (*prog == '{')
		{
			// pula os coment�rios
			prog++;
			while (*prog && (*prog != '}'))
				prog++;
			if (*prog == '}')
				prog++;
		}
		else
		{
			new_line = ((*prog == 13) || (*prog == 10));
			prog++;
		}
	}

	// verifica se chegou ao fim
	if (!*prog)
	{
		tok_type = ttEnd;
		return;
	}

	// guarda o token atual para compara��es posteriores
	tok = token;
	last_tok_type = tok_type;

	// salva o caracter atual
	*tok = *prog++;

	// verifica se o caracter era um delimitador
	if ((*tok == '(') || (*tok == ')') || (*tok == ','))
	{
		// verifica se o delimitador esta em um condi��o de erro
        switch (last_tok_type)
		{
		case ttImport:
		case ttMacro:
		case ttTest:
		case ttProgram:
		case ttIteractive:
		case ttMono:
			{
				tok_type = ttError;
				return;
			}
		}

		// como n�o ocorreu erro, o delimitador � poss�vel
		*++tok = 0;
		tok_type = ttDelimiter;
		return;
	}

	// pega o token inteiro para identificar o que ela �
	if ((last_tok_type == ttImport) && (*tok == '"'))
	{
		while (*prog && (*prog != '"'))
			*tok++ = *prog++;
		if (*prog == '"')
		{
			prog++;
			tok--;
		}
	}
	else
	{
		while (*prog && (*prog != ' ') && (*prog != ',') && (*prog != '(') && (*prog != ')') && (*prog != '\t') && (*prog != 13) && (*prog != 10) && (*prog != -51) && (*prog != '{'))
			*++tok = *prog++;
	}
	// salva o caracter nulo
	*++tok = 0;

	// identifica o token
	if (stricmp (token, DEFMACRO) == 0)
		tok_type = ttMacro;
	else if (stricmp (token, DEFTEST) == 0)
		tok_type = ttTest;
	else if (stricmp (token, DEFENDMACRO) == 0)
		tok_type = ttEndMacro;
	else if (stricmp (token, DEFIMPORT) == 0)
		tok_type = ttImport;
	else if (stricmp (token, DEFPROGRAM) == 0)
		tok_type = ttProgram;
	else if (stricmp (token, DEF_ITR) == 0)
		tok_type = ttIteractive;
	else if (stricmp (token, DEF_MONO) == 0)
		tok_type = ttMono;
	else
		tok_type = ttFunction;
}

void PARSER::put_back ()
{
	// put_back - volta para um token anterior
	unsigned int nLen = strlen (token);
	while (nLen)
	{
		prog--;
		nLen--;
	}
}

bool FileExists (char *lpFileName)
{
	FILE *pFile = fopen (lpFileName, "r");
	if (pFile)
	{
		fclose (pFile);
		return true;
	}
	else
		return false;
}

bool HasPath (char *lpFileName)
{
	if (!isalpha (*lpFileName))
		return false;
	lpFileName++;

	if (*lpFileName != ':')
		return false;
	lpFileName++;

	while (*lpFileName)
	{
		if (*lpFileName == '\\')
			return true;
		lpFileName++;
	}

	return false;
}

bool PARSER::UpdateLibPath ()
{
	// UpdateLibPath - atualiza o path da biblioteca
	char lpPath[MAX_PATH];

	if (!HasPath (token))
	{
		if (!sFilePath->IsEmpty ())
		{
			strcpy (lpPath, *sFilePath);
			strcat (lpPath, token);
			if (FileExists (lpPath))
			{
				strcpy (token, lpPath);
				return true;
			}
		}

		strcpy (lpPath, *sProgramPath);
		strcat (lpPath, "biblios\\");
		strcat (lpPath, token);
		if (FileExists (lpPath))
		{
			strcpy (token, lpPath);
			return true;
		}

		strcpy (lpPath, *sProgramPath);
		strcat (lpPath, token);
		if (FileExists (lpPath))
		{
			strcpy (token, lpPath);
			return true;
		}

		return false;
	}
	else
		return true;
}

bool PARSER::Parse (char *pSourceName, char *pSource, bool bEditor, bool bFullParse)
{
	// Parse - analisa um programa de entrada
	MONOPARSER pMono;
	ITERACTIVEPARSER pIteractive;
	MACRO *pMacro, *pLoopMacro;
	PROGRAM_BASE *pCurProg;
	LABEL *pCurLabel;
	MACRO_TYPE tMacro;
	STACK pLibsStack;
	unsigned int i, nCurItem, nMacrosCount, nCount;
	bool bIteractive = true, bEditorResult = true;


	// limpa o programa atual
	pProgram->ClearMacros (bEditor && !bFullParse);
	pProgram->Clear ();

	// atualiza as vari�veis
	pIteractive.pMacroReturn = pProgram->pMacroReturn;


	// PASSO 1 - procura pelas bibliotecas
	nCurItem = 0;
	prog = pSource;
	err_libname = NULL;
	err_source = pSource;
	err_undefdata = NULL;
	pLibsStack.Push (pSourceName);
	do
	{
		// pega o primeiro token
		get_token ();
		new_line = true;
		while (tok_type != ttEnd)
		{
			if (new_line)
			{
				// verifica se � uma biblioteca ou uma indica��o de tipo de programa
				if (tok_type == ttImport)
				{
					// neste caso � uma biblioteca, l� o nome do arquivo
					get_token ();
					// importa a biblioteca
					if (tok_type == ttFunction)
					{
						// verifica se j� foi listada
						if (!pLibsStack.IsInStack (token))
						{
							// atualiza o path da biblioteca
							if (!UpdateLibPath ())
							{
								if (bEditor)
								{
									bEditorResult = false;
									goto step1_nextlib;
								}
								else
								{
									err_type = etLoadLibError;
									err_pos = token;
									return false;
								}
							}
							// tenta incluir a biblioteca
							if (!AddLibrary (token))
							{
								if (bEditor)
								{
									bEditorResult = false;
									goto step1_nextlib;
								}
								else
								{
									err_type = etLoadLibError;
									err_pos = token;
									return false;
								}
							}
							// inclui a biblioteca j� visitada
							pLibsStack.StrPush (token);
						}
						else
						{
							// erro de loop de inclus�o de bibliotecas
							if (bEditor)
							{
								bEditorResult = false;
								goto step1_nextlib;
							}
							else
							{
								err_type = etLibLoopError;
								err_pos = token;
								return false;
							}
						}
					}
					else
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step1_nextlib;
						}
						else
						{
							err_type = etLibNameExpected;
							err_pos = prog - 1;
							return false;
						}
					}
				}
				else if (tok_type == ttProgram)
				{
					// neste caso � uma indica��o de tipo de programa
					get_token ();
					// verifica qual � o tipo de programa
					if (tok_type == ttMono)
						bIteractive = false;
					else if (tok_type == ttIteractive)
						bIteractive = true;
					else
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step1_nextlib;
						}
						else
						{
							err_type = etProgTypeExpected;
							err_pos = prog - 1;
							return false;
						}
					}
				}
			}
			// pega o pr�ximo token
			get_token ();
		}
step1_nextlib:
		// pega a pr�xima biblioteca
		if ((nCurItem >= nLibsCount) || (bEditor && !bFullParse))
			prog = NULL;
		else
		{
			prog = pLibs[nCurItem].sData;
			err_source = prog;
			err_libname = pLibs[nCurItem++].sName;
		}
	} while (prog);
	pLibsStack.Clear ();


	// PASSO 2 - procura pelas macros no programa e em cada biblioteca
	prog = pSource;
	nCurItem = 0;
	do
	{
		// atualiza a origem do poss�vel erro
		err_source = prog;
		// pega o primeiro token
		get_token ();
		new_line = true;
		while (tok_type != ttEnd)
		{
			if (new_line)
			{
				// verifica se � uma defini��o de macro
				if (tok_type == ttMacro)
				{
					// aloca uma nova macro
					if (!(pMacro = pProgram->AddMacro ()))
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step2_nextlib;
						}
						else
						{
							err_type = etMemory;
							err_pos = prog - 1;
							return false;
						}
					}
					// atualiza a flag da macro
					pMacro->SetExternalMacro (err_source != pSource);
					// neste caso � uma defini��o de macro, pega o pr�ximo token
					get_token ();
					// verifica qual � o tipo da macro
					if (tok_type == ttMono)
						pMacro->SetType (mtMono);
					else if (tok_type == ttIteractive)
						pMacro->SetType (mtIteractive);
					else
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step2_nextlib;
						}
						else
						{
							err_type = etMacroTypeExpected;
							err_pos = prog - 1;
							return false;
						}
					}
					// pega o nome da macro
					get_token ();
					// verifica se � o nome da macro
					if (tok_type != ttFunction)
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step2_nextlib;
						}
						else
						{
							err_type = etMacroNameExpected;
							err_pos = prog - 1;
							return false;
						}
					}
					pMacro->SetName (token);
					// pega os par�metros
					get_token ();
					if ((tok_type != ttDelimiter) || (token[0] != '('))
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step2_nextlib;
						}
						else
						{
							err_type = etMacroParamsExpected;
							err_pos = prog - 1;
							return false;
						}
					}
					// pega o primeiro par�metro
					get_token ();
					if (!evaluate_macro_params (pMacro))
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step2_nextlib;
						}
						else
						{
							err_pos = prog - 1;
							return false;
						}
					}
					// verifica se � uma macro relacionada com teste
					if (tok_type == ttTest)
					{
						// pega o nome do teste
						get_token ();
						// verifica se � um nome de teste
						if (tok_type != ttFunction)
						{
							if (bEditor)
							{
								bEditorResult = false;
								goto step2_nextlib;
							}
							else
							{
								err_type = etTestNameExpected;
								err_pos = prog - 1;
								return false;
							}
						}
						pMacro->SetTest (token);
					}
					else
					{
						// volta uma token atr�s
						put_back ();
					}
					// salva o ponteiro de onde come�a a macro
					pMacro->SetData (prog);
					// pega o nome da biblioteca onde esta macro est� inclusa
					if (nCurItem)
						pMacro->SetLibName (pLibs[nCurItem - 1].sName);
					// vai at� o fim da macro ou do arquivo
					while ((tok_type != ttEnd) && ((tok_type != ttEndMacro) || (!new_line)))
					{
						// pega o pr�ximo token
						get_token ();
					}
					// se for final do arquivo, � um erro
					if (tok_type == ttEnd)
					{
						if (bEditor)
						{
							bEditorResult = false;
							goto step2_nextlib;
						}
						else
						{
							err_type = etEndMacroExpected;
							err_pos = prog - 1;
							return false;
						}
					}
				}
			}
			// pega o pr�ximo token
			get_token ();
		}
step2_nextlib:
		// pega a pr�xima biblioteca
		if ((nCurItem >= nLibsCount) || (bEditor && !bFullParse))
			prog = NULL;
		else
		{
			prog = pLibs[nCurItem].sData;
			err_libname = pLibs[nCurItem++].sName;
		}
	} while (prog);

	// se for apenas para o editor, para por aqui
	if (bEditor)
	{
		ClearLibs ();
		return bEditorResult;
	}


	// PASSO 3 - faz o parsing de cada um dos macros
	nMacrosCount = pProgram->MacrosCount ();
	for (i = 0; i < nMacrosCount; i++)
	{
		// verifica qual � o tipo de macro
		pMacro = pProgram->GetMacro (i);
		tMacro = pMacro->GetType ();
		if (tMacro == mtMono)
		{
			// neste caso � uma macro monol�tica
			pMono.pProgram = pMacro;
			pMono.pSourceProg = pMacro->GetData ();
			if (!pMono.Parse (true))
			{
				err_type = etMonoError;
				err_type_mono = pMono.GetError ();
				err_libname = pMacro->GetLibName ();
				if (err_libname)
					err_source = GetLibraryData (err_libname);
				else
					err_source = pSource;
				err_pos = pMono.GetErrorPos ();
				return false;
			}
		}
		else if (tMacro == mtIteractive)
		{
			// neste caso � uma macro iterativa
			pIteractive.pProgram = pMacro;
			pIteractive.pSourceProg = pMacro->GetData ();
			if (!pIteractive.Parse (true))
			{
				err_type = etIteractiveError;
				err_type_itr = pIteractive.GetError ();
				err_libname = pMacro->GetLibName ();
				if (err_libname)
					err_source = GetLibraryData (err_libname);
				else
					err_source = pSource;
				err_pos = pIteractive.GetErrorPos ();
				return false;
			}
		}
	}


	// PASSO 4 - procura o inicio do programa e faz o parsing do programa
	prog = pSource;
	// pega o primeiro token
	get_token ();
	new_line = true;
	while (tok_type != ttEnd)
	{
		if (new_line)
		{
			// est� no inicio de uma nova linha, verifica se � o inicio do programa
			if (tok_type == ttMacro)
			{
				// pega o pr�ximo token
				get_token ();
				// vai at� o fim da macro
				while ((tok_type != ttEndMacro) || (!new_line))
				{
					// pega o pr�ximo token
					get_token ();
				}
			}
			else if ((tok_type != ttImport) && (tok_type != ttProgram))
			{
				put_back ();
				break;
			}
		}
		// pega o pr�ximo token
		get_token ();
	}
	// analisa o programa
	if (!bIteractive)
	{
		// neste caso � o programa � monol�tico
		pMono.pProgram = pProgram;
		pMono.pSourceProg = prog;
		if (!pMono.Parse (false))
		{
			err_type = etMonoError;
			err_type_mono = pMono.GetError ();
			err_libname = NULL;
			err_source = pSource;
			err_pos = pMono.GetErrorPos ();
			return false;
		}
	}
	else
	{
		// neste caso � o programa � iterativo
		pIteractive.pProgram = pProgram;
		pIteractive.pSourceProg = prog;
		if (!pIteractive.Parse (false))
		{
			err_type = etIteractiveError;
			err_type_itr = pIteractive.GetError ();
			err_libname = NULL;
			err_source = pSource;
			err_pos = pIteractive.GetErrorPos ();
			return false;
		}
	}


	// PASSO 5 - verifica se todas as macros referenciadas existem e se o n�mero de par�metros � compat�vel
	pCurProg = pProgram;
	nCurItem = 0;
	do
	{
		// pega o total de r�tulos
		nCount = pCurProg->Count ();
		// verifica cada r�tulo
		for (i = 0; i < nCount; i++)
		{
			// pega o r�tulo
			pCurLabel = (*pCurProg)[i];
			// verifica se � um r�tulo de macro
			if ((pCurLabel->lType == ltMacroOperation) || (pCurLabel->lType == ltMacroTest))
			{
				// pega a macro
				pMacro = pProgram->GetMacro (pCurLabel->sData);
				// verifica se a macro foi definida
				if (!pMacro)
				{
					err_type = etUndefinedMacro;
					err_undefdata = pCurLabel->sData;
					if (!nCurItem)
					{
						err_libname = NULL;
						err_source = pSource;
						err_pos = NULL;
					}
					else
					{
						err_libname = ((MACRO*)pCurProg)->GetLibName ();
						if (err_libname)
							err_source = GetLibraryData (err_libname);
						else
							err_source = pSource;
						err_pos = ((MACRO*)pCurProg)->GetName ();
					}
					return false;
				}
				// se for uma referencia em um teste, verifica se a macro tem um teste definido
				if ((pCurLabel->lType == ltMacroTest) && !pMacro->GetTest ())
				{
					err_type = etUndefinedMacroTest;
					err_undefdata = pCurLabel->sData;
					if (!nCurItem)
					{
						err_libname = NULL;
						err_source = pSource;
						err_pos = NULL;
					}
					else
					{
						err_libname = ((MACRO*)pCurProg)->GetLibName ();
						if (err_libname)
							err_source = GetLibraryData (err_libname);
						else
							err_source = pSource;
						err_pos = ((MACRO*)pCurProg)->GetName ();
					}
					return false;
				}
				// verifica o n�mero de par�metros
				if (pCurLabel->nParamsCount != pMacro->ParamsCount ())
				{
					err_type = etMacroCallParamsError;
					err_undefdata = pCurLabel->sData;
					if (!nCurItem)
					{
						err_libname = NULL;
						err_source = pSource;
						err_pos = NULL;
					}
					else
					{
						err_libname = ((MACRO*)pCurProg)->GetLibName ();
						if (err_libname)
							err_source = GetLibraryData (err_libname);
						else
							err_source = pSource;
						err_pos = ((MACRO*)pCurProg)->GetName ();
					}
					return false;
				}
			}
		}
		// pega a pr�xima macro
		if (nCurItem >= nMacrosCount)
			pCurProg = NULL;
		else
			pCurProg = pProgram->GetMacro (nCurItem++);
	} while (pCurProg);


	// PASSO 6 - detecta loops entre macros
	for (i = 0; i < nMacrosCount; i++)
	{
		// pega a macro a ser verificada
		pMacro = pProgram->GetMacro (i);
		// verifica se tem loop com outra macro
		pLoopMacro = DetectMacroLoop (pMacro, NULL);
		if (pLoopMacro)
		{
			err_type = etMacroLoopError;
			err_libname = pMacro->GetLibName ();
			if (err_libname)
				err_source = GetLibraryData (err_libname);
			else
				err_source = pSource;
			err_pos = pMacro->GetName ();
			err_undefdata = pLoopMacro->GetName ();
			return false;
		}
	}


	// FIM! - o programa de origem foi analisado sintaticamente com sucesso e est� na mem�ria
	ClearLibs ();
	return true;
}

bool PARSER::evaluate_macro_params (MACRO *pMacro)
{
	// evaluate_macro_params - avalia os par�metros formais de uma macro
	while (tok_type == ttFunction)
	{
		// adiciona o par�metro
		pMacro->AddParameter (token, token);
		// pega o pr�ximo token
		get_token ();
		// verifica se � uma v�rgula
		if ((tok_type == ttDelimiter) && (token[0] == ','))
		{
			// pega o pr�ximo token
			get_token ();
		}
		else if ((tok_type != ttDelimiter) || (token[0] != ')'))
		{
			err_type = etParenthesisExpected;
			return false;
		}
	}
	
	if ((tok_type == ttDelimiter) && (token[0] == ')'))
	{
		// pega o pr�ximo token
		get_token ();
		// retorna os par�metros
		return true;
	}
	else
	{
		err_type = etParenthesisExpected;
		return false;
	}
}

void PARSER::ClearLibs ()
{
	// ClearLibs - libera a lista de bibliotecas
	if (pLibs)
	{
		unsigned int i;
		// remove cada biblioteca
		for (i = 0; i < nLibsCount; i++)
		{
			free (pLibs[i].sName);
			free (pLibs[i].sData);
		}
		free (pLibs);
		pLibs = NULL;
	}
	// atualiza o contador
	nLibsCount = 0;
}

bool PARSER::AddLibrary (char *sName, char *sData)
{
	// AddLibrary - adiciona uma biblioteca na lista de bibliotecas
	nLibsCount++;
	if (!(pLibs = (LIB_INFO*) realloc (pLibs, nLibsCount * sizeof(LIB_INFO))))
		return false;
	pLibs[nLibsCount - 1].sName = sName;
	pLibs[nLibsCount - 1].sData = sData;
	return true;
}

bool PARSER::AddLibrary (char *sFileName)
{
	// AddLibrary - adiciona uma biblioteca baseada no nome do arquivo
	FILE *pFile = fopen (sFileName, "r");
	if (pFile)
	{
		long nFileSize = fsize (pFile);
		char *pData = (char*) malloc (nFileSize + 1);
		if (pData)
		{
			// l� os dados do arquivo
			memset (pData, 0, nFileSize + 1);
			fread (pData, 1, nFileSize, pFile);
			fclose (pFile);
			// aloca espa�o para o nome da biblioteca
			//char *sName = ExtractFileName (sFileName);
			char *sNewName = (char*) malloc ((strlen (sFileName/*sName*/) + 1) * sizeof(char));
			if (sNewName)
			{
				// inclui na lista
				strcpy (sNewName, sFileName/*sName*/);
				if (AddLibrary (sNewName, pData))
					return true;
				free (sNewName);
			}
		}
	}
	// ocorreu algum erro
	return false;
}

char *PARSER::GetLibraryData (char *sName)
{
	// GetLibraryData - pega o dado de um biblioteca
	unsigned int i;
	for (i = 0; i < nLibsCount; i++)
	{
		if (stricmp (pLibs[i].sName, sName) == 0)
			return pLibs[i].sData;
	}
	// neste caso n�o foi encontrada
	return NULL;
}

MACRO *PARSER::DetectMacroLoop (MACRO *pMacro, STACK *pStack)
{
	// DetectMacroLoop - detecta loops entre macros
	if (pStack)
	{
		LABEL *pCurLabel;
		unsigned int i, nCount = pMacro->Count ();
		for (i = 0; i < nCount; i++)
		{
			// pega o r�tulo
			pCurLabel = (*pMacro)[i];
			// verifica se � um r�tulo de macro
			if ((pCurLabel->lType == ltMacroOperation) || (pCurLabel->lType == ltMacroTest))
			{
				// pega a macro
				char *sRefMacro = pCurLabel->sData;
				MACRO *pRefMacro = pProgram->GetMacro (sRefMacro);
				// verifica se a macro j� foi visitada
				if (pStack->IsInStack (sRefMacro))
					return pRefMacro;
				else
				{
					pStack->Push (sRefMacro);
					if (DetectMacroLoop (pRefMacro, pStack))
						return pRefMacro;
					pStack->Pop ();
				}
			}
		}
		// neste caso nao a loop
		return NULL;
	}
	else
	{
		// inicia a pesquisa de loop
		pStack = new STACK ();
		pStack->Push (pMacro->GetName ());
		pMacro = DetectMacroLoop (pMacro, pStack);
		delete pStack;
		return pMacro;
	}
}