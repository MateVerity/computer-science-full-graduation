/********************************************************************
	IMPLEMENTA��O DA CLASSE MONOPARSER
	Cria��o:	03/06/2003
	Copyright:	(c) 2003 Marcelo Bona Boff e Fernando Trebien
********************************************************************/


#include "stdafx.h"
#include "Parser.h"
#include "ParserData.h"
#include "Program.h"


/********************************************************************
	CLASSE MONOPARSER */

MONOPARSER::MONOPARSER ()
{
	// MONOPARSER - cria a classe de parsing de programas monol�ticos
	pLabels = NULL;
	nLabelsCount = 0;
}

MONOPARSER::~MONOPARSER ()
{
	// ~MONOPARSER - destr�i a classe de parsing de programas monol�ticos
	ClearLabels ();
}
	
bool MONOPARSER::Parse (bool bIsMacro)
{
	// Parse - analisa um programa monol�tico
	LABEL *pCurLabel;

	// incializa as vari�veis
	ClearLabels ();
	prog = pSourceProg;
	tok_type = mttEnd;
	err_pos = NULL;

	// pega o primeiro token
	get_token ();
	new_line = true;
	if (tok_type == mttEnd)
	{
		err_type = metEnd;
		err_pos = prog - 1;
		return false;
	}

	// ignora os enters do incio da fonte
	while (tok_type == mttEnter)
		get_token ();

	// loop de leitura de r�tulos
	do
	{
		// verifica se � inicio de linha
		if (!new_line)
		{
			printf ("%s\n", token);
			put_back ();
			err_type = metEnterExpected;
			err_pos = prog;
			return false;
		}
		// verifica se � um r�tulo
		if (tok_type != mttLabel)
		{
			put_back ();
			err_type = metLabelExpected;
			err_pos = prog;
			return false;
		}
		// tenta criar o r�tulo
		if  (!(pCurLabel = pProgram->AddLabel ()))
		{
			err_type = metMemory;
			return false;
		}
		// adiciona o r�tulo na lista
		if (!AddLabel (token))
		{
			err_type = metMemory;
			return false;
		}
		pCurLabel->nIdx = pProgram->Count () - 1;
		get_token ();
		// verifica se � uma opera��o ou um teste
		if (tok_type == mttIf)
		{
			// pega o teste que ser� feito
			get_token ();
			if (tok_type != mttFunction)
			{
				put_back ();
				err_type = metTestNameExpected;
				err_pos = prog;
				return false;
			}
			pCurLabel->SetData (token);
			// verifica se � uma macro
			get_token ();
			if ((tok_type == mttDelimiter) && (token[0] == '('))
			{
				// analisa os par�metros
				get_token ();
				if (!evaluate_params (pCurLabel->pParams, pCurLabel->nParamsCount))
					return false;
				// atualiza o tipo do r�tulo
				pCurLabel->lType = ltMacroTest;
			}
			else
				pCurLabel->lType = ltTest;
			// verifica se � um ent�o
			if (tok_type != mttThen)
			{
				put_back ();
				err_type = metThenExpected;
				err_pos = prog;
				return false;
			}
			// verifica se � um v�_para
			get_token ();
			if (tok_type != mttGoto)
			{
				put_back ();
				err_type = metGotoExpected;
				err_pos = prog;
				return false;
			}
			// verifica se � um r�tulo de pulo
			get_token ();
			if (tok_type != mttFunction)
			{
				put_back ();
				err_type = metLabelNameExpected;
				err_pos = prog;
				return false;
			}
			if (!(pCurLabel->pTrue = (LABEL*) copy_string (token)))
			{
				err_type = metMemory;
				return false;
			}
			// verifica se � um sen�o
			get_token ();
			if (tok_type != mttElse)
			{
				put_back ();
				err_type = metElseExpected;
				err_pos = prog;
				return false;
			}
			// verifica se � um v�_para
			get_token ();
			if (tok_type != mttGoto)
			{
				put_back ();
				err_type = metGotoExpected;
				err_pos = prog;
				return false;
			}
			// verifica se � um r�tulo de pulo
			get_token ();
			if (tok_type != mttFunction)
			{
				put_back ();
				err_type = metLabelNameExpected;
				err_pos = prog;
				return false;
			}
			if (!(pCurLabel->pFalse = (LABEL*) copy_string (token)))
			{
				err_type = metMemory;
				return false;
			}
		}
		else if (tok_type == mttDo)
		{
			// pega a opera��o que ser� feita
			get_token ();
			if (tok_type != mttFunction)
			{
				put_back ();
				err_type = metOperationNameExpected;
				err_pos = prog;
				return false;
			}
			pCurLabel->SetData (token);
			// verifica se � uma macro
			get_token ();
			if ((tok_type == mttDelimiter) && (token[0] == '('))
			{
				// analisa os par�metros
				get_token ();
				if (!evaluate_params (pCurLabel->pParams, pCurLabel->nParamsCount))
					return false;
				// atualiza o tipo do r�tulo
				pCurLabel->lType = ltMacroOperation;
			}
			else
				pCurLabel->lType = ltOperation;
			// verifica se � um v�_para
			if (tok_type != mttGoto)
			{
				put_back ();
				err_type = metGotoExpected;
				err_pos = prog;
				return false;
			}
			// verifica se � um r�tulo de pulo
			get_token ();
			if (tok_type != mttFunction)
			{
				put_back ();
				err_type = metLabelNameExpected;
				err_pos = prog;
				return false;
			}
			if (!(pCurLabel->pTrue = pCurLabel->pFalse = (LABEL*) copy_string (token)))
			{
				err_type = metMemory;
				return false;
			}
		}
		else
		{
			put_back ();
			err_type = metInstructionExpected;
			err_pos = prog;
			return false;
		}
		// pega o pr�ximo token
		do
		{
			get_token ();
		} while (tok_type == mttEnter);
		// verifica se � uma finaliza��o da macro
		if (bIsMacro && (tok_type == mttEndMacro))
			break;
	} while (tok_type != mttEnd);

	// corrige os pulos para os seus respectivos r�tulos
	unsigned int i, nCount = pProgram->Count (), nIdx;
	for (i = 0; i < nCount; i++)
	{
		// pega o r�tulo
		pCurLabel = (*pProgram)[i];
		// atualiza o ponteiro do verdadeiro
		nIdx = GetLabelIdx ((char*) pCurLabel->pTrue);
		free (pCurLabel->pTrue);
		if (!nIdx)
			pCurLabel->pTrue = NULL;
		else
			pCurLabel->pTrue = (*pProgram)[nIdx - 1];
		// verifica se o r�tulo � ou n�o de teste
		if ((pCurLabel->lType == ltTest) || (pCurLabel->lType == ltMacroTest))
		{
			// atualiza o ponteiro do falso
			nIdx = GetLabelIdx ((char*) pCurLabel->pFalse);
			free (pCurLabel->pFalse);
			if (!nIdx)
				pCurLabel->pFalse = NULL;
			else
				pCurLabel->pFalse = (*pProgram)[nIdx - 1];
		}
		else
			pCurLabel->pFalse = pCurLabel->pTrue;
	}

	// o programa foi lido com sucesso
	return true;
}

void MONOPARSER::get_token ()
{
	// get_token - pega e identifica o proximo token
	char *tok;
	M_TOK_TYPE last_tok_type;

	// indica que n�o � o inicio da linha
	new_line = (tok_type == mttEnter);

	// ignora espa�os, tabs
	while ((*prog == ' ') || (*prog == '\t') || (*prog == -51) || (*prog == '{'))
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
			prog++;
	}

	// verifica se chegou ao fim
	if (!*prog)
	{
		tok_type = mttEnd;
		return;
	}

	// guarda o token atual para compara��es posteriores
	tok = token;
	last_tok_type = tok_type;

	// salva o caracter atual
	*tok = *prog++;

	// verifica se o caracter era um delimitador
	if ((*tok == 10) || (*tok == 13) || (*tok == '(') || (*tok == ')') || (*tok == ','))
	{
		// verifica se o delimitador esta em um condi��o de erro
        switch (last_tok_type)
		{
		case mttIf:
		case mttThen:
		case mttElse:
		case mttGoto:
		case mttDo:
		case mttLabel:
			{
				tok_type = mttError;
				return;
			}
		}

		// verifica se o pr�ximo caracter � um LF
		if (*tok == 10)
			tok_type = mttEnter;
		else if (*tok == 13)
		{
			tok_type = mttEnter;
			if (*prog == 10)
				prog++;
		}
		else
			tok_type = mttDelimiter;

		// como n�o ocorreu erro, o delimitador � poss�vel
		*++tok = 0;
		return;
	}

	// pega o token inteiro para identificar o que ele �
	while (*prog && (*prog != ' ') && (*prog != ',') && (*prog != '(') && (*prog != ')') && (*prog != '\t') && (*prog != 13) && (*prog != 10) && (*prog != -51) && (*prog != '{') && (*prog != ':'))
		*++tok = *prog++;
	// salva o caracter nulo
	*++tok = 0;

	// identifica o token
	if (*prog == ':')
	{
		tok_type = mttLabel;
		prog++;
	}
	else if (stricmp (token, DEFENDMACRO) == 0)
		tok_type = mttEndMacro;
	else if (stricmp (token, IF) == 0)
		tok_type = mttIf;
	else if (stricmp (token, THEN) == 0)
		tok_type = mttThen;
	else if (stricmp (token, ELSE) == 0)
		tok_type = mttElse;
	else if (stricmp (token, GOTO) == 0)
		tok_type = mttGoto;
	else if (stricmp (token, DO) == 0)
		tok_type = mttDo;
	else
		tok_type = mttFunction;
}

void MONOPARSER::put_back ()
{
	// put_back - volta para um token anterior
	unsigned int nLen = strlen (token);
	while (nLen)
	{
		prog--;
		nLen--;
	}
}

void MONOPARSER::ClearLabels ()
{
	// ClearLabels - destr�i a lista de r�tulos
	if (pLabels)
	{
		unsigned int i;
		for (i = 0; i < nLabelsCount; i++)
			free (pLabels[i]);
		free (pLabels);
		pLabels = NULL;
	}
	nLabelsCount = 0;
}

bool MONOPARSER::AddLabel (char *lpName)
{
	// AddLabel - adiciona um novo r�tulo na lista
	if (!(pLabels = (char**) realloc (pLabels, (nLabelsCount + 1) * sizeof(char*))))
		return false;
	if (!(pLabels[nLabelsCount] = (char*) malloc ((strlen (lpName) + 1) * sizeof(char))))
		return false;
	strcpy (pLabels[nLabelsCount++], lpName);
	return true;
}

unsigned int MONOPARSER::GetLabelIdx (char *lpName)
{
	// GetLabelIdx - retorna o �ndice de um r�tulo
	unsigned int i;
	for (i = 0; i < nLabelsCount; i++)
	{
		if (stricmp (pLabels[i], lpName) == 0)
			return i + 1;
	}
	// neste caso o r�tulo n�o existe
	return 0;
}

bool MONOPARSER::evaluate_params (char **&pParams, unsigned int &nParamsCount)
{
	// evaluate_params - avalia os par�metros de uma macro
	pParams = NULL;
	nParamsCount = 0;

	while (tok_type == mttFunction)
	{
		// atualiza o n�mero de par�metros
		nParamsCount++;
		// tenta alocar mem�ria
		if (!(pParams = (char**) realloc (pParams, nParamsCount * sizeof(char*))))
		{
			nParamsCount = 0;
			err_type = metMemory;
			return false;
		}
		if (!(pParams[nParamsCount - 1] = (char*) malloc ((strlen (token) + 1) * sizeof(char))))
		{
			nParamsCount--;
			err_type = metMemory;
			goto error;
		}
		// copia o par�metro
		strcpy (pParams[nParamsCount - 1], token);
		// pega o pr�ximo token
		get_token ();
		// verifica se � uma v�rgula
		if ((tok_type == mttDelimiter) && (token[0] == ','))
		{
			// pega o pr�ximo token
			get_token ();
		}
		else if ((tok_type != mttDelimiter) || (token[0] != ')'))
		{
			err_type = metParenthesisExpected;
			goto error;
		}
	}
	
	if ((tok_type == mttDelimiter) && (token[0] == ')'))
	{
		// pega o pr�ximo token
		get_token ();
		// retorna os par�metros
		return true;
	}
	else
		err_type = metParenthesisExpected;

error:
	// ocorreu um erro de mem�ria
	unsigned int i;
	for (i = 0; i < nParamsCount; i++)
		free (pParams[i]);
	free (pParams);
	pParams = NULL;
	nParamsCount = 0;
	return false;
}

char *MONOPARSER::copy_string (char *lpStr)
{
	char *lpNewStr;
	if (!(lpNewStr = (char*) malloc ((strlen (lpStr) + 1) * sizeof(char))))
		return NULL;
	strcpy (lpNewStr, lpStr);
	return lpNewStr;
}