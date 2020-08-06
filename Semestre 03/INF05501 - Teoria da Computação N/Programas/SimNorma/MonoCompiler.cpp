/********************************************************************
	IMPLEMENTA��O DAS FUN��ES MONOCOMPILER
	Cria��o:	30/05/2003
	Copyright:	(c) 2003 Marcelo Bona Boff e Fernando Trebien
	Descri��o:	
				Fun��es de compila��o para programa completamente
				monol�tico sem macros.
********************************************************************/


#include "stdafx.h"
#include "MonoCompiler.h"


bool MonoCompiler (PROGRAM_BASE *pBaseProg)
{
	// MonoCompiler - compila um programa em um �nico mon�lito descartando macros
	PROGRAM *pProgram = (PROGRAM*) pBaseProg;
	LABEL **pSource = pBaseProg->pLabels, **pNew = NULL, *pCurLabel;
	UPDATESTACK pBaseLabels;
	unsigned int nCurLabel, nSourceCount = pBaseProg->nLabelsCount, nNewCount = 0, nNewIdx = nSourceCount;

	MACRO *pMacro;
	UPDATESTACK pStack;
	LABEL *pEndJump;
	unsigned char nUpdateFlags, nNewFlags;

	// adiciona cada r�tulo cuidando especialmente das macros
	for (nCurLabel = 0; nCurLabel < nSourceCount; nCurLabel++)
	{
		// pega o r�tulo
		pCurLabel = pSource[nCurLabel];
		// verifica se � algum r�tulo de macro
		if ((pCurLabel->lType == ltMacroOperation) || (pCurLabel->lType == ltMacroTest))
		{
			// atualiza a macro chamada
			pMacro = pProgram->GetMacro (pCurLabel->sData);
			// inclui a macro
			if (!MC_IncludeMacro (pProgram, pMacro, &pStack, pCurLabel, pNew, nNewCount, pCurLabel->nIdx, nNewIdx))
				return false;
			// atualiza o r�tulo de pulo final da macro
			if (pCurLabel->lType == ltMacroOperation)
				pEndJump = pCurLabel->pTrue;
			else
			{
				// adiciona o teste da macro
				pEndJump = new LABEL ();
				pEndJump->lType = ltTest;
				pEndJump->nIdx = nNewIdx++;
				pEndJump->pTrue = pCurLabel->pTrue;
				pEndJump->pFalse = pCurLabel->pFalse;
				pEndJump->SetData (pMacro->GetTest ());
				if (!MC_AddLabel (pEndJump, pNew, nNewCount))
				{
					delete pEndJump;
					return false;
				}
				// atualiza o r�tulo
				if (!MC_UpdateMacroLabel (pMacro, pEndJump))
					return false;
				// adiciona na pilha das atualiza��es
				pBaseLabels.Push (pEndJump, UPDATE_TRUE | UPDATE_FALSE);
			}
			// atualiza os pulos dos r�tulos finais das macros
			LABEL *pEndLabel;
			while (pEndLabel = (LABEL*) pStack.Pop (&nUpdateFlags))
			{
				nNewFlags = 0;
				if ((nUpdateFlags & UPDATE_TRUE) && !pEndLabel->pTrue || (pEndLabel->pTrue == pProgram->pMacroReturn))
				{
					pEndLabel->pTrue = pEndJump;
					nNewFlags |= UPDATE_TRUE;
				}
				if ((nUpdateFlags & UPDATE_FALSE) && !pEndLabel->pFalse || (pEndLabel->pFalse == pProgram->pMacroReturn))
				{
					pEndLabel->pFalse = pEndJump;
					nNewFlags |= UPDATE_FALSE;
				}
				// adiciona na pilha das atualiza��es
				pBaseLabels.Push (pEndLabel, nNewFlags);
			}
		}
		else
		{
			// adiciona o r�tulo atual
			if (!MC_AddLabel (pCurLabel, pNew, nNewCount))
				return false;
			// adiciona na pilha das atualiza��es
			pBaseLabels.Push (pCurLabel, UPDATE_TRUE | UPDATE_FALSE);
		}
	}

	// atualiza os pulos dos r�tulos que est�o na pilha de atuliza��o
	while (pCurLabel = (LABEL*) pBaseLabels.Pop (&nUpdateFlags))
	{
		// atualiza cada pulo
		if ((nUpdateFlags & UPDATE_TRUE) && pCurLabel->pTrue)
			pCurLabel->pTrue = MC_LabelFromIdx (pNew, nNewCount, pCurLabel->pTrue->nIdx);
		if ((nUpdateFlags & UPDATE_FALSE) && pCurLabel->pFalse)
			pCurLabel->pFalse = MC_LabelFromIdx (pNew, nNewCount, pCurLabel->pFalse->nIdx);
	}

	// atualiza os �ndices de todos os r�tulos da nova lista
	for (nCurLabel = 0; nCurLabel < nNewCount; nCurLabel++)
		pNew[nCurLabel]->nIdx = nCurLabel;

	// destr�i os r�tulos de macro
	for (nCurLabel = 0; nCurLabel < nSourceCount; nCurLabel++)
	{
		// pega o r�tulo
		pCurLabel = pSource[nCurLabel];
		// verifica se � algum r�tulo de macro para o destruir
		if ((pCurLabel->lType == ltMacroOperation) || (pCurLabel->lType == ltMacroTest))
			delete pCurLabel;
	}

	// atualiza a lista do programa para a nova lista
	pBaseProg->pLabels = pNew;
	pBaseProg->nLabelsCount = nNewCount;
	free (pSource);

	// apaga as macros
	pProgram->ClearMacros ();

	// o programa foi compilado com sucesso
	return true;
}

bool MC_AddLabel (LABEL *pLabel, LABEL **&pLabels, unsigned int &nLabelsCount)
{
	// MC_AddLabel - adiciona um novo r�tulo numa lista de r�tulos
	if (!(pLabels = (LABEL**) realloc (pLabels, (nLabelsCount + 1) * sizeof (LABEL*))))
		return false;
	pLabels[nLabelsCount] = pLabel;
	nLabelsCount++;
	return true;
}

bool MC_UpdateMacroLabel (MACRO *pMacro, LABEL *pLabel)
{
	// MC_UpdateMacroLabel - atualiza o r�tulo de uma macro substituindo os par�metros
	// OBS: ESTA FUN��O � ESPEC�FICA PARA A SIMULA��O DA M�QUINA NORMA
	if (!pLabel->sData)
		return true;
	char *sReg = pLabel->sData;
	// atualiza o nome do registrador
	while (*sReg)
		sReg++;
	sReg--;
	while ((*sReg != '-') && (sReg > pLabel->sData))
		sReg--;
	// verifica se a instru��o usa algum registrador
	if (*sReg == '-')
	{
		// atualiza o registrador
		sReg++;
		// neste caso utiliza o registrador, verifica se ele � um par�metro formal da macro
		if (pMacro->IsParam (sReg))
		{
			// atualiza com o seu valor
			char *sValue = pMacro->GetValue (sReg);
			// atualiza o dado do r�tulo
			if (!(pLabel->sData = (char*) realloc (pLabel->sData, ((sReg - pLabel->sData) + strlen (sValue) + 1) * sizeof(char))))
				return false;
			// copia o valor do par�metro
			strcpy (sReg, sValue);
		}
	}
	// a atualiza��o foi feita com sucesso
	return true;
}

bool MC_IncludeMacro (PROGRAM *pProgram, MACRO *pMacro, UPDATESTACK *pStack, LABEL *pCaller, LABEL **&pLabels, unsigned int &nLabelsCount, unsigned int nFirstIdx, unsigned int &nSecondIdx)
{
	// MC_IncludeMacro - adiciona um novo macro numa lista de r�tulos
	MAPPER pMap;
	UPDATESTACK pMacroLabels, pMacroStack;
	LABEL *pCurLabel, *pEndJump, *pEndLabel, *pNewLabel;
	MACRO *pCallMacro;
	char **pParams;
	bool bIsFirst = true;
	unsigned int i, nCurLabel, nNewIdx, nParamsCount, nMacroCount = pMacro->Count ();
	unsigned char nUpdateFlags, nNewFlags;


	// atualiza os par�mtros da macro
	for (i = 0; i < pCaller->nParamsCount; i++)
		pMacro->SetValue (i, pCaller->pParams[i]);

	// adiciona cada r�tulo cuidando especialmente das macros
	for (nCurLabel = 0; nCurLabel < nMacroCount; nCurLabel++)
	{
		// pega o r�tulo
		pCurLabel = (*pMacro)[nCurLabel];
		// verifica se � algum r�tulo de macro
		if ((pCurLabel->lType == ltMacroOperation) || (pCurLabel->lType == ltMacroTest))
		{
			// mapeia o primeiro r�tulo da macro
			if (bIsFirst)
			{
				nNewIdx = nFirstIdx;
				bIsFirst = false;
			}
			else
				nNewIdx = nSecondIdx++;
			// mapeia o r�tulo para a macro
			pMap.Map (pCurLabel->nIdx, nNewIdx);
			// atualiza a macro chamada
			pCallMacro = pProgram->GetMacro (pCurLabel->sData);
			// guarda os par�metros
			pParams = pCurLabel->pParams;
			nParamsCount = pCurLabel->nParamsCount;
			pCurLabel->pParams = NULL;
			pCurLabel->nParamsCount = 0;
			for (i = 0; i < nParamsCount; i++)
			{
				// verifica se � um par�metro da macro atual
				if (pMacro->IsParam (pParams[i]))
					pCurLabel->AddParam (pMacro->GetValue (pParams[i]));
				else
					pCurLabel->AddParam (pParams[i]);
			}
			// inclui a macro
			if (!MC_IncludeMacro (pProgram, pCallMacro, &pMacroStack, pCurLabel, pLabels, nLabelsCount, nNewIdx, nSecondIdx))
			{
				// restaura os par�metros antigos
				free (pCurLabel->pParams);
				pCurLabel->pParams = pParams;
				pCurLabel->nParamsCount = nParamsCount;
				// retorna falso
				return false;
			}
			// restaura os par�metros antigos
			free (pCurLabel->pParams);
			pCurLabel->pParams = pParams;
			pCurLabel->nParamsCount = nParamsCount;
			// atualiza o r�tulo de pulo final da macro
			if (pCurLabel->lType == ltMacroOperation)
				pEndJump = pCurLabel->pTrue;
			else
			{
				// calcula o novo �ndice
				nNewIdx = nSecondIdx++;
				// adiciona o teste da macro
				pEndJump = new LABEL ();
				pEndJump->lType = ltTest;
				pEndJump->nIdx = nNewIdx;
				pEndJump->pTrue = pCurLabel->pTrue;
				pEndJump->pFalse = pCurLabel->pFalse;
				pEndJump->SetData (pCallMacro->GetTest ());
				if (!MC_AddLabel (pEndJump, pLabels, nLabelsCount))
				{
					delete pEndJump;
					return false;
				}
				// atualiza o r�tulo
				if (!MC_UpdateMacroLabel (pCallMacro, pEndJump))
					return false;
				// adiciona na pilha das atualiza��es
				pMacroLabels.Push (pEndJump, UPDATE_TRUE | UPDATE_FALSE);
			}
			// atualiza os pulos dos r�tulos finais das macros
			while (pEndLabel = (LABEL*) pMacroStack.Pop (&nUpdateFlags))
			{
				nNewFlags = 0;
				if ((nUpdateFlags & UPDATE_TRUE) && !pEndLabel->pTrue || (pEndLabel->pTrue == pProgram->pMacroReturn))
				{
					pEndLabel->pTrue = pEndJump;
					nNewFlags |= UPDATE_TRUE;
				}
				if ((nUpdateFlags & UPDATE_FALSE) && !pEndLabel->pFalse || (pEndLabel->pFalse == pProgram->pMacroReturn))
				{
					pEndLabel->pFalse = pEndJump;
					nNewFlags |= UPDATE_FALSE;
				}
				// adiciona na pilha das atualiza��es
				pMacroLabels.Push (pEndLabel, nNewFlags);
			}
		}
		else
		{
			// calcula o novo �ndice
			if (bIsFirst)
			{
				nNewIdx = nFirstIdx;
				bIsFirst = false;
			}
			else
				nNewIdx = nSecondIdx++;
			// copia o r�tulo
			pNewLabel = new LABEL ();
			pNewLabel->CopyFrom (pCurLabel);
			pNewLabel->nIdx = nNewIdx;
			// mapeia o r�tulo que est� sendo inserido
			pMap.Map (pCurLabel->nIdx, pNewLabel->nIdx);
			// adiciona o r�tulo atual
			if (!MC_AddLabel (pNewLabel, pLabels, nLabelsCount))
				return false;
			// atualiza o r�tulo
			if (!MC_UpdateMacroLabel (pMacro, pNewLabel))
				return false;
			// adiciona na pilha das atualiza��es
			pMacroLabels.Push (pNewLabel, UPDATE_TRUE | UPDATE_FALSE);
		}
	}

	// atualiza os pulos dos r�tulos que est�o na pilha de atuliza��o
	while (pCurLabel = (LABEL*) pMacroLabels.Pop (&nUpdateFlags))
	{
		// verificam se algum r�tulo pula para o fim
		if (!pCurLabel->pTrue || !pCurLabel->pFalse || (pCurLabel->pTrue == pProgram->pMacroReturn) || (pCurLabel->pFalse == pProgram->pMacroReturn))
			pStack->Push (pCurLabel, nUpdateFlags);
		// atualiza cada pulo
		if ((nUpdateFlags & UPDATE_TRUE) && pCurLabel->pTrue && (pCurLabel->pTrue != pProgram->pMacroReturn))
			pCurLabel->pTrue = MC_LabelFromIdx (pLabels, nLabelsCount, pMap[pCurLabel->pTrue->nIdx]);
		if ((nUpdateFlags & UPDATE_FALSE) && pCurLabel->pFalse && (pCurLabel->pFalse != pProgram->pMacroReturn))
			pCurLabel->pFalse = MC_LabelFromIdx (pLabels, nLabelsCount, pMap[pCurLabel->pFalse->nIdx]);
	}

	// a macro foi inclu�da com sucesso
	return true;
}

LABEL *MC_LabelFromIdx (LABEL **pLabels, unsigned int nLabelsCount, unsigned int nIdx)
{
	// MC_LabelFromIdx - retorna o r�tulo baseado no seu �ndice
	LABEL *pCurLabel;
	unsigned int i;
	for (i = 0; i < nLabelsCount; i++)
	{
		// pega o r�tulo
		pCurLabel = pLabels[i];
		// verifica o �ndice
		if (pCurLabel->nIdx == nIdx)
			return pCurLabel;
	}
	// neste caso o r�tulo n�o foi encontrado
	return NULL;
}


/********************************************************************
	CLASSE MAPPER */

MAPPER::MAPPER ()
{
	// MAPPER - cria a classe de mapeamento
	pItems = NULL;
	nItemsCount = 0;
}
	
MAPPER::~MAPPER ()
{
	// ~MAPPER - destr�i a classe de mapeamento
	free (pItems);
}

bool MAPPER::Map (unsigned int nNum1, unsigned int nNum2)
{
	// Map - mapeia um novo n�mero
	if (!(pItems = (MAPPER_ITEM*) realloc (pItems, (nItemsCount + 1) * sizeof(MAPPER_ITEM))))
		return false;
	pItems[nItemsCount].nNum1 = nNum1;
	pItems[nItemsCount].nNum2 = nNum2;
	nItemsCount++;
	return true;
}

unsigned int MAPPER::operator[] (unsigned int nNum1)
{
	// [] - retorna o n�mero mapeado por nNum1
	unsigned int i;
	for (i = 0; i < nItemsCount; i++)
	{
		if (pItems[i].nNum1 == nNum1)
			return pItems[i].nNum2;
	}
	return nNum1;
}