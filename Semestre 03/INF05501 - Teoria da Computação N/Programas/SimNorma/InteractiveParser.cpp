/********************************************************************
	IMPLEMENTA��O DA CLASSE ITERACTIVEPARSER
	Cria��o:	30/05/2003
	Copyright:	(c) 2003 Marcelo Bona Boff e Fernando Trebien
********************************************************************/


#include "stdafx.h"
#include "Parser.h"
#include "ParserData.h"


/********************************************************************
	CLASSE ITERACTIVEPARSER */

bool ITERACTIVEPARSER::Parse (bool bIsMacro, bool bRemoveNullOp)
{
	// Parse - inicia o parsing de um programa iterativo
	prog = pSourceProg;
	err_pos = NULL;

	// atualiza os flags internos
	support_exit = bIsMacro;
	support_null = !bRemoveNullOp;

	// pega o primeiro token
	get_token ();
	if (tok_type == ittEnd)
	{
		err_type = ietEnd;
		err_pos = prog - 1;
		return false;
	}

	// avalia o programa
	LABEL *pResult = NULL;
	if (!evaluate_instruction (pResult, NULL, NULL, NULL))
	{
		err_pos = prog - 1;
		return false;
	}

	// verifica se chegou realmente ao fim
	if ((bIsMacro && (tok_type == ittEndMacro)) ||
		(!bIsMacro && (tok_type == ittEnd)))
		return true;
	else
	{
		if (bIsMacro)
			err_type = ietEndMacroExpected;
		else
			err_type = ietEndExpected;
		err_pos = prog - 1;
		return false;
	}
}

void ITERACTIVEPARSER::get_token ()
{
	// get_token - pega e identifica o proximo token
	char *tok;
	I_TOK_TYPE last_tok_type;

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
			prog++;
	}

	// verifica se chegou ao fim
	if (!*prog)
	{
		tok_type = ittEnd;
		return;
	}

	// guarda o token atual para compara��es posteriores
	tok = token;
	last_tok_type = tok_type;

	// salva o caracter atual
	*tok = *prog++;

	// verifica se o caracter era um delimitador
	if ((*tok == '(') || (*tok == ')') || (*tok == ';') || (*tok == ','))
	{
		// verifica se o delimitador esta em um condi��o de erro
        switch (last_tok_type)
		{
		case ittIf:
		case ittWhile:
		case ittUntil:
			{
				tok_type = ittError;
				return;
			}
		}

		// como n�o ocorreu erro, o delimitador � poss�vel
		*++tok = 0;
		tok_type = ittDelimiter;
		return;
	}

	// pega o token inteiro para identificar o que ela �
	while (*prog && (*prog != ' ') && (*prog != ';') && (*prog != ',') && (*prog != '(') && (*prog != ')') && (*prog != '\t') && (*prog != 13) && (*prog != 10) && (*prog != -51) && (*prog != '{'))
		*++tok = *prog++;
	// salva o caracter nulo
	*++tok = 0;

	// identifica o token
	if (stricmp (token, DEFENDMACRO) == 0)
		tok_type = ittEndMacro;
	else if (stricmp (token, IF) == 0)
		tok_type = ittIf;
	else if (stricmp (token, THEN) == 0)
		tok_type = ittThen;
	else if (stricmp (token, ELSE) == 0)
		tok_type = ittElse;
	else if (stricmp (token, WHILE) == 0)
		tok_type = ittWhile;
	else if (stricmp (token, UNTIL) == 0)
		tok_type = ittUntil;
	else if (stricmp (token, DO) == 0)
		tok_type = ittDo;
	else if (stricmp (token, DEFEXITMACRO) == 0)
	{
		if (support_exit)
			tok_type = ittExit;
		else
			tok_type = ittError;
	}
	else
		tok_type = ittFunction;
}

bool ITERACTIVEPARSER::evaluate_instruction (LABEL *&pResult, LABEL *pPrevious, LABEL *pEndJump, STACK *pEndLabels, bool bCodeBlock)
{
	// evaluate_instruction - avalia uma instru��o
	STACK *pInternalEndLabels = NULL;

	// verifica se � um erro
	if (tok_type == ittError)
	{
		err_type = ietSyntax;
		return false;
	}

	// verifica se o item atual � um parenteses
	if ((tok_type == ittDelimiter) && (token[0] == '('))
	{
		// pega o pr�ximo token
		get_token ();
		// cria uma nova pilha
		pInternalEndLabels = new STACK ();
		// avalia a instru��o
		if (!evaluate_instruction (pResult, pPrevious, pEndJump, pInternalEndLabels))
		{
			delete pInternalEndLabels;
			return false;
		}
		// verifica se o o token atual � um fecha par�nteses
		if ((tok_type != ittDelimiter) || (token[0] != ')'))
		{
			err_type = ietParenthesisExpected;
			delete pInternalEndLabels;
			return false;
		}
		// pega o pr�ximo token
		get_token ();
	}
	else if (tok_type == ittIf)
	{
		// pega o pr�ximo token
		get_token ();
		// cria uma nova pilha
		pInternalEndLabels = new STACK ();
		// avalia a condi��o
		if (!(pResult = evaluate_condition (pEndJump, pInternalEndLabels)))
		{
			delete pInternalEndLabels;
			return false;
		}
	}
	else if (tok_type == ittWhile)
	{
		// pega o pr�ximo token
		get_token ();
		// avalia o enquanto
		if (!(pResult = evaluate_while (pEndJump, pEndLabels)))
			return false;
	}
	else if (tok_type == ittUntil)
	{
		// pega o pr�ximo token
		get_token ();
		// avalia o at�
		if (!(pResult = evaluate_until (pEndJump, pEndLabels)))
			return false;
	}
	else if (tok_type == ittExit)
	{
		// sai da macro, ignorando o resto do c�digo
		pResult = pMacroReturn;
		get_token ();
		/*// insere o r�tulo anterior na pilha de atualiza��o
		if (pPrevious)
		{
			if (pEndLabels && ((pPrevious->lType == ltOperation) || (pPrevious->lType == ltMacroOperation)))
				pEndLabels->Push (pPrevious);
		}*/
		return true;
	}
	else if (tok_type == ittFunction)
	{
		// verifica se a intru��o n�o � vazia
		if (support_null || stricmp (token, N_NULL))
		{
			// aloca espa�o para o r�tulo criado e adiciona na lista
			if (!(pResult = pProgram->AddLabel ()))
			{
				err_type = ietMemory;
				return false;
			}
			// atualiza as propriedades
			pResult->lType = ltOperation;
			pResult->nIdx = pProgram->Count () - 1;
			pResult->pTrue = pEndJump;
			pResult->pFalse = pEndJump;
			// verifica se � uma opera��o nula
			if (stricmp (token, N_NULL))
				pResult->SetData (token);
			else
				pResult->sData = NULL;
			// pega o pr�ximo token
			get_token ();
			// verifica se � uma chamada de macro
			if ((tok_type == ittDelimiter) && (token[0] == '('))
			{
				// pega o pr�ximo token
				get_token ();
				// � uma chamada de macro
				pResult->lType = ltMacroOperation;
				if (!evaluate_params (pResult->pParams, pResult->nParamsCount))
					return false;
			}
		}
		else
		{
			// ignora e passa para o pr�ximo token
			get_token ();
		}
	}
	else if ((tok_type != ittDelimiter) || (token[0] != ')'))
	{
		err_type = ietSyntax;
		return false;
	}

	// verifica se a instru��o atual � composta
	if ((!bCodeBlock) && (tok_type == ittDelimiter) && (token[0] == ';'))
	{
		// pega o pr�ximo token
		get_token ();
		// pega o r�tulo anterior
		LABEL *pTmpPrevious, *pNext;
		if (pResult)
			pTmpPrevious = pResult;
		else
			pTmpPrevious = pPrevious;
		// avalia a instru��o que � composta
		if (!evaluate_instruction (pNext, pTmpPrevious, pEndJump, pEndLabels))
		{
			if (pInternalEndLabels)
				delete pInternalEndLabels;
			return false;
		}
		// atualiza os ponteiros finais de pulo
		if (pInternalEndLabels)
		{
			LABEL *pEndLabel;
			while (pEndLabel = (LABEL*) pInternalEndLabels->Pop ())
				pEndLabel->SetLastJump (pNext, pEndJump);
			delete pInternalEndLabels;
		}
		// atualiza os ponteiros do resultado
		if (pResult)
			pResult->SetLastJump (pNext, pEndJump);
		else
			pResult = pNext;
	}
	// � um r�tulo final, verifica se a instru��o tem para onde voltar
	else
	{
		// atualiza os ponteiros finais de pulo
		if (pInternalEndLabels)
		{
			LABEL *pEndLabel;
			while (pEndLabel = (LABEL*) pInternalEndLabels->Pop ())
			{
				pEndLabel->SetLastJump (pEndJump, pEndJump);
				if (pEndLabels)
					pEndLabels->Push (pEndLabel);
			}
			delete pInternalEndLabels;
		}
		// pega o r�tulo que deve ser inclu�do na pilha de atualiza��es
		LABEL *pUpdate = NULL;
		if (!pResult)
		{
			pUpdate = pPrevious;
			//pResult = pEndJump;
		}
		else
		{
			pUpdate = pResult;
			pResult->SetLastJump (pEndJump, pEndJump);
		}
		// atualiza a pilha de atualiza��es
		if (pUpdate)
		{
			if (pEndLabels && ((pUpdate->lType == ltOperation) || (pUpdate->lType == ltMacroOperation)))
				pEndLabels->Push (pUpdate);
		}
	}

	// returna o primeiro r�tulo criado
	return true;
}

LABEL *ITERACTIVEPARSER::evaluate_condition (LABEL *pEndJump, STACK *pEndLabels)
{
	// evaluate_condition - avalia uma condi��o
	LABEL *pResult;

	// verifica se � um erro
	if (tok_type == ittError)
	{
		err_type = ietSyntax;
		return NULL;
	}

	// verifica se o item atual � uma fun��o (teste)
	if (tok_type == ittFunction)
	{
		// aloca espa�o para o r�tulo criado
		if (!(pResult = pProgram->AddLabel ()))
		{
			err_type = ietMemory;
			return NULL;
		}

		// adiciona na lista e atualiza propriedades
		pResult->lType = ltTest;
		pResult->nIdx = pProgram->Count () - 1;

		// salva o nome da fun��o
		pResult->SetData (token);
		// pega o pr�ximo token
		get_token ();

		// verifica se � uma chamada de macro
		if ((tok_type == ittDelimiter) && (token[0] == '('))
		{
			// pega o pr�ximo token
			get_token ();
			// � uma chamada de macro
			pResult->lType = ltMacroTest;
			if (!evaluate_params (pResult->pParams, pResult->nParamsCount))
				return NULL;
		}

		// verifica se � um ent�o
		if (tok_type != ittThen)
		{
			err_type = ietThenExpected;
			return NULL;
		}
		// neste caso � um ent�o, ent�o pega o pr�ximo token
		get_token ();

		// avalia a instru��o
		if (!evaluate_instruction (pResult->pTrue, pResult, pEndJump, pEndLabels))
			return NULL;

		// verifica se � um sen�o
		if (tok_type != ittElse)
		{
			err_type = ietElseExpected;
			return NULL;
		}
		// neste caso � um sen�o, ent�o pega o pr�ximo token
		get_token ();

		// avalia a instru��o
		if (!evaluate_instruction (pResult->pFalse, pResult, pEndJump, pEndLabels))
			return NULL;

		return pResult;
	}
	else
	{
		err_type = ietSyntax;
		return NULL;
	}
}

LABEL *ITERACTIVEPARSER::evaluate_while (LABEL *pEndJump, STACK *pEndLabels)
{
	// evaluate_while - avalia o enquanto
	LABEL *pResult;

	// verifica se � um erro
	if (tok_type == ittError)
	{
		err_type = ietSyntax;
		return NULL;
	}

	// verifica se o item atual � uma fun��o (teste)
	if (tok_type == ittFunction)
	{
		// aloca espa�o para o r�tulo criado
		if (!(pResult = pProgram->AddLabel ()))
		{
			err_type = ietMemory;
			return NULL;
		}

		// adiciona na lista e atualiza propriedades
		pResult->lType = ltTest;
		pResult->nIdx = pProgram->Count () - 1;
		pResult->pFalse = pEndJump;

		// salva o nome da fun��o
		pResult->SetData (token);
		// pega o pr�ximo token
		get_token ();

		// verifica se � uma chamada de macro
		if ((tok_type == ittDelimiter) && (token[0] == '('))
		{
			// pega o pr�ximo token
			get_token ();
			// � uma chamada de macro
			pResult->lType = ltMacroTest;
			if (!evaluate_params (pResult->pParams, pResult->nParamsCount))
				return NULL;
		}

		// verifica se � um fa�a
		if (tok_type != ittDo)
		{
			err_type = ietDoExpected;
			return NULL;
		}
		// neste caso � um fa�a, ent�o pega o pr�ximo token
		get_token ();

		// avalia a instru��o
		if (!evaluate_instruction (pResult->pTrue, pResult, pResult, NULL, true))
			return NULL;

		// adiciona o enquanto na pilha
		if (pEndLabels)
			pEndLabels->Push (pResult);

		return pResult;
	}
	else
	{
		err_type = ietSyntax;
		return NULL;
	}
}

LABEL *ITERACTIVEPARSER::evaluate_until (LABEL *pEndJump, STACK *pEndLabels)
{
	// evaluate_until - avalia o at�
	LABEL *pResult;

	// verifica se � um erro
	if (tok_type == ittError)
	{
		err_type = ietSyntax;
		return NULL;
	}

	// verifica se o item atual � uma fun��o (teste)
	if (tok_type == ittFunction)
	{
		// aloca espa�o para o r�tulo criado
		if (!(pResult = pProgram->AddLabel ()))
		{
			err_type = ietMemory;
			return NULL;
		}

		// adiciona na lista e atualiza propriedades
		pResult->lType = ltTest;
		pResult->nIdx = pProgram->Count () - 1;
		pResult->pTrue = pEndJump;

		// salva o nome da fun��o
		pResult->SetData (token);
		// pega o pr�ximo token
		get_token ();

		// verifica se � uma chamada de macro
		if ((tok_type == ittDelimiter) && (token[0] == '('))
		{
			// pega o pr�ximo token
			get_token ();
			// � uma chamada de macro
			pResult->lType = ltMacroTest;
			if (!evaluate_params (pResult->pParams, pResult->nParamsCount))
				return NULL;
		}

		// verifica se � um fa�a
		if (tok_type != ittDo)
		{
			err_type = ietDoExpected;
			return NULL;
		}
		// neste caso � um fa�a, ent�o pega o pr�ximo token
		get_token ();

		// avalia a instru��o
		if (!evaluate_instruction (pResult->pFalse, pResult, pResult, NULL, true))
			return NULL;

		// adiciona o ate na pilha
		if (pEndLabels)
			pEndLabels->Push (pResult);

		return pResult;
	}
	else
	{
		err_type = ietSyntax;
		return NULL;
	}
}

bool ITERACTIVEPARSER::evaluate_params (char **&pParams, unsigned int &nParamsCount)
{
	// evaluate_params - avalia os par�metros de uma macro
	pParams = NULL;
	nParamsCount = 0;

	while (tok_type == ittFunction)
	{
		// atualiza o n�mero de par�metros
		nParamsCount++;
		// tenta alocar mem�ria
		if (!(pParams = (char**) realloc (pParams, nParamsCount * sizeof(char*))))
		{
			nParamsCount = 0;
			err_type = ietMemory;
			return false;
		}
		if (!(pParams[nParamsCount - 1] = (char*) malloc ((strlen (token) + 1) * sizeof(char))))
		{
			nParamsCount--;
			err_type = ietMemory;
			goto error;
		}
		// copia o par�metro
		strcpy (pParams[nParamsCount - 1], token);
		// pega o pr�ximo token
		get_token ();
		// verifica se � uma v�rgula
		if ((tok_type == ittDelimiter) && (token[0] == ','))
		{
			// pega o pr�ximo token
			get_token ();
		}
		else if ((tok_type != ittDelimiter) || (token[0] != ')'))
		{
			err_type = ietParenthesisExpected;
			goto error;
		}
	}
	
	if ((tok_type == ittDelimiter) && (token[0] == ')'))
	{
		// pega o pr�ximo token
		get_token ();
		// retorna os par�metros
		return true;
	}
	else
		err_type = ietParenthesisExpected;

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