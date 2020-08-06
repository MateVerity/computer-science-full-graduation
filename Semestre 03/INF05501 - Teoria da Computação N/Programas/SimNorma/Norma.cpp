/********************************************************************
	IMPLEMENTA��O DA CLASSE SIMNORMA
	Cria��o:	31/05/2003
	Copyright:	(c) 2003 Marcelo Bona Boff e Fernando Trebien
********************************************************************/


#include "stdafx.h"
#include "Norma.h"


/********************************************************************
	CLASSE SIMNORMA */

SIMNORMA::SIMNORMA ()
{
	// SIMNORMA - cria a classe de simula��o da m�quina norma
	pRegs = NULL;
	nRegsCount = 0;
}

SIMNORMA::~SIMNORMA ()
{
	// ~SIMNORMA - destr�i a classe de simula��o da m�quina norma
	DestroyRegs ();
}

void SIMNORMA::DestroyRegs ()
{
	// DestroyRegs - destr�i os registradores atuais
	free (pRegs);
	pRegs = NULL;
	nRegsCount = 0;
}

bool SIMNORMA::GrowRegs (unsigned int nNewRegsCount)
{
	// GrowRegs - aumenta o n�mero de registradores se necess�rio
	if (nNewRegsCount > nRegsCount)
	{
		if (!(pRegs = (unsigned long*) realloc (pRegs, nNewRegsCount * sizeof(unsigned long))))
			return false;
		memset (pRegs + nRegsCount, 0, (nNewRegsCount - nRegsCount) * sizeof(unsigned long));
		nRegsCount = nNewRegsCount;
	}
	return true;
}

unsigned int SIMNORMA::GetRegNumber (char *sReg)
{
	// GetRegNumber - retorna o �ndice de um registrador
	if (isalpha (*sReg))
	{
		if (isupper (*sReg))
			return *sReg - 'A';
		else
			return *sReg - 'a';
	}
	else
		return atol (sReg) - 1;
}

bool SIMNORMA::CheckProgram ()
{
	// CheckProgram - verifica se todas as intru��es inclu�das s�o v�lidas
	PROGRAM_BASE *pCurProg = pProgram;
	MACRO *pMacro = NULL;
	LABEL *pCurLabel;
	char *sReg = NULL;
	unsigned int i, j, nCount, nCurItem = 0;

	// destr�i os registradores atuais
	DestroyRegs ();

	// repete para cada macro e para o programa principal
	do
	{
		// verifica cada r�tulo
		nCount = pCurProg->Count ();
		for (i = 0; i < nCount; i++)
		{
			// pega o r�tulo
			pCurLabel = (*pCurProg)[i];
			// verifica o r�tulo dependendo do seu tipo
			switch (pCurLabel->lType)
			{
			case ltOperation:
				{
					// verifica se a opera��o existe
					if (!pCurLabel->sData)
						sReg = NULL;
					else if (strnicmp (pCurLabel->sData, N_ADD, 3) == 0)
						sReg = pCurLabel->sData + 3;
					else if (strnicmp (pCurLabel->sData, N_SUB, 4) == 0)
						sReg = pCurLabel->sData + 4;
					else if (strnicmp (pCurLabel->sData, N_PRINT, 6) == 0)
						sReg = pCurLabel->sData + 6;
					else
					{
						err_type = netInvalidOperation;
						err_data = pCurLabel->sData;
						return false;
					}
					break;
				}
			case ltTest:
				{
					// verifica se o teste exite
					if (strnicmp (pCurLabel->sData, N_ZERO, 5) == 0)
						sReg = pCurLabel->sData + 5;
					else
					{
						err_type = netInvalidTest;
						err_data = pCurLabel->sData;
						return false;
					}
					break;
				}
			default:
				sReg = NULL;
			}
			// verifica se o registrador � v�lido, verifica se � um par�metro formal
			if (sReg)
			{
				if (!CheckRegister (sReg, pMacro))
				{
					// neste caso o registrador � inv�lido
					err_type = etInvalidRegister;
					err_data = sReg;
					return false;
				}
				else
				{
					// neste caso o registrador � v�lido, verifica se n�o � um par�metro formal
					if (!pMacro)
						GrowRegs (GetRegNumber (sReg) + 1);
					else if (!pMacro->IsParam (sReg))
						GrowRegs (GetRegNumber (sReg) + 1);
				}
			}
			// se for uma chamada a macro, verifica cada par�metro
			if ((pCurLabel->lType == ltMacroOperation) || (pCurLabel->lType == ltMacroTest))
			{
				// verifica cada par�metro
				for (j = 0; j < pCurLabel->nParamsCount; j++)
				{
					if (!CheckRegister (pCurLabel->pParams[j], pMacro))
					{
						// neste caso o registrador � inv�lido
						err_type = etInvalidRegister;
						err_data = pCurLabel->pParams[j];
						return false;
					}
					else
					{
						// neste caso o registrador � v�lido, verifica se n�o � um par�metro formal
						if (!pMacro)
							GrowRegs (GetRegNumber (pCurLabel->pParams[j]) + 1);
						else if (!pMacro->IsParam (pCurLabel->pParams[j]))
							GrowRegs (GetRegNumber (pCurLabel->pParams[j]) + 1);
					}
				}
			}
		}

		// verifica se a macro tem um teste definido
		if (pMacro)
		{
			if (sReg = pMacro->GetTest ())
			{
				// verifica se o teste exite
				if (strnicmp (sReg, N_ZERO, 5) == 0)
					sReg = sReg + 5;
				else
				{
					err_type = netInvalidTest;
					err_data = sReg;
					return false;
				}
				// verifica se o registrador � v�lido
				if (!CheckRegister (sReg, pMacro))
				{
					// neste caso o registrador � inv�lido
					err_type = etInvalidRegister;
					err_data = sReg;
					return false;
				}
				else if (!pMacro->IsParam (sReg))
					GrowRegs (GetRegNumber (sReg) + 1);
			}
		}

		// pega a pr�xima macro
		if (nCurItem >= pProgram->MacrosCount ())
			pCurProg = NULL;
		else
		{
			pMacro = pProgram->GetMacro (nCurItem++);
			pCurProg = pMacro;
		}
	} while (pCurProg);
	// neste caso o programa � valido
	return true;
}

bool SIMNORMA::CheckRegister (char *sReg, MACRO *pMacro)
{
	// CheckRegister - retorna se um registrador � v�lido
	if (pMacro)
	{
		if (pMacro->IsParam (sReg))
			return true;
	}

	// verifica se come�a com um n�mero ou uma letra
	if (isalpha (*sReg))
	{
		if (strlen (sReg) == 1)
			return true;
	}
	else if (isdigit (*sReg))
	{
		char *lpReg = sReg;
		do
		{
			lpReg++;
		} while (isdigit (*lpReg));
		if (!*lpReg)
			return true;
	}

	// � um registrador inv�lido
	return false;
}

void SIMNORMA::ClearRegs ()
{
	// ClearRegs - zera todos os registradores
	if (pRegs)
		memset (pRegs, 0, nRegsCount * sizeof(unsigned long));
}

CUR_STEP SIMNORMA::Step (HWND hwndParent, CUR_STEP pCurStep, bool bPrintTuple)
{
	// Step - executa um passo
	LABEL *pCurLabel;
	if (!pCurStep)
		pCurLabel = (*pProgram)[0];
	else
		pCurLabel = pCurStep;

	// imprime a upla
	if (bPrintTuple)
	{
		unsigned int i;
		CString sTuple = "(";
		CString sNumber;

		// cria a tupla
		for (i = 0; i < nRegsCount - 1; i++)
		{
			sNumber.Format ("%d", pRegs[i]);
			sTuple += sNumber + ", ";
		}
		sNumber.Format ("%d", pRegs[nRegsCount - 1]);
		sTuple += sNumber + ")";

		// manda a mensagem para a sa�da
		SendMessage (hwndParent, WM_NORMAMSG, WNM_PRINT, (LPARAM) sTuple.GetBuffer (sTuple.GetLength ()));
		sTuple.ReleaseBuffer ();
	}

	// verifica qual � o tipo de r�tulo atual
	if (pCurLabel->lType == ltOperation)
	{
		// � uma opera��o, verifica qual � a opera��o
		if (pCurLabel->sData)
		{
			if (strnicmp (pCurLabel->sData, N_ADD, 3) == 0)
			{
				// executa a opera��o
				pRegs[GetRegNumber (pCurLabel->sData + 3)]++;
			}
			else if (_strnicmp (pCurLabel->sData, N_SUB, 4) == 0)
			{
				// executa a opera��o
				unsigned int nRegNumber = GetRegNumber (pCurLabel->sData + 4);
				if (pRegs[nRegNumber])
                    pRegs[nRegNumber]--;
			}
			else if (_strnicmp (pCurLabel->sData, N_PRINT, 6) == 0)
			{
				// imprime o registrador
				CString sReg = (char*) pCurLabel->sData + 6;
				CString sNumber;

				sNumber.Format ("%d", pRegs[GetRegNumber (pCurLabel->sData + 6)]);
				sReg += " = " + sNumber;
				SendMessage (hwndParent, WM_NORMAMSG, WNM_PRINT, (LPARAM) sReg.GetBuffer (sReg.GetLength ()));
				sReg.ReleaseBuffer ();
			}
		}

		// atualiza o fluxo do programa
		pCurLabel = pCurLabel->pTrue;
	}
	else
	{
		// � um teste, verifica qual � o teste
		if (strnicmp (pCurLabel->sData, N_ZERO, 5) == 0)
		{
			// verifica o teste e atualiza o fluxo
			if (!pRegs[GetRegNumber (pCurLabel->sData + 5)])
				pCurLabel = pCurLabel->pTrue;
			else
				pCurLabel = pCurLabel->pFalse;
		}
	}

	// retorna o pr�ximo passo
	return pCurLabel;
}

unsigned long &SIMNORMA::operator[] (unsigned int nIdx)
{
	// [] - pega o valor do registrador
	if (nIdx >= nRegsCount)
		GrowRegs (nIdx + 1);
	return pRegs[nIdx];
}