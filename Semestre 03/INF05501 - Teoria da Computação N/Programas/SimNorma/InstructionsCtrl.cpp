/********************************************************************
	IMPLEMENTA��O DO CONTROLE DE INTRU��ES ROTULADAS
	Cria��o:	17/07/2003
	Copyright:	(c) 2003 Marcelo Bona Boff, Fernando Trebien e
						 Leonardo Golob
********************************************************************/


#include "stdafx.h"
#include "InstructionsCtrl.h"
#include "ParserData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INFLATE_RECT(r,n) {r.left -= n; r.top -= n; r.right += n; r.bottom += n;}


/********************************************************************
	CLASSE INSTRUCTIONS CONTROL */

CInstructionsCtrl::CInstructionsCtrl()
{
	// atribui o valor padr�o das var�aveis
	pProgram = NULL;
	ptoStartPos.x = ptoStartPos.y = 0;
	
	pActiveLabel = pCallerLabel = NULL;
	bCallerWay = true;

	hItemsFont = (HFONT) GetStockObject (DEFAULT_GUI_FONT);
	cBackColor = RGB (255, 255, 255);
	cBarColor = RGB (192, 192, 192);
	cBarBorder = RGB (128, 128, 128);
	cActiveColor = RGB (255, 255, 128);
	cActiveBorder = RGB (128, 128, 128);
	cCallerColor = RGB (255, 255, 192);
	cCallerBorder = RGB (192, 192, 192);
	nLabelWidth = 24;

	szInstrSize.cx = szInstrSize.cy = 0;
	bmpDC = NULL;
	bmpBuffer = NULL;
	bmpSize.cx = 0; bmpSize.cy = 0;
}

CInstructionsCtrl::~CInstructionsCtrl()
{
}


BEGIN_MESSAGE_MAP(CInstructionsCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()



// CInstructionsCtrl message handlers

BOOL CInstructionsCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style |= WS_HSCROLL | WS_VSCROLL;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CInstructionsCtrl::OnPaint() 
{
	CPaintDC pDC (this);
	
	// pinta o controle
	PaintControl (&pDC);

	// updates the scroll bars
	UpdateScroll ();
}

BOOL CInstructionsCtrl::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CInstructionsCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// atualiza a barra de rolagem
	UpdateScroll ();
}

void CInstructionsCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// rola o conte�do da tela para cima/baixo
	int nNewPos = GetScrollPos (SB_VERT);
	int nMinPos, nMaxPos;
	GetScrollRange (SB_VERT, &nMinPos, &nMaxPos);

	// pega a nova posi��o
	switch (nSBCode)
	{
	case SB_BOTTOM:
	case SB_LINEDOWN:
		nNewPos += 4;
		break;
	case SB_TOP:
	case SB_LINEUP:
		nNewPos -= 4;
		break;
	case SB_PAGEDOWN:
		nNewPos += 16;
		break;
	case SB_PAGEUP:
		nNewPos -= 16;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewPos = nPos;
		break;
	}

	// atualiza os dados
	if (nNewPos > nMaxPos)
		nNewPos = nMaxPos;
	else if (nNewPos < nMinPos)
		nNewPos = nMinPos;

	// atualiza as posi��es
	ptoStartPos.y = -nNewPos;
	SetScrollPos (SB_VERT, nNewPos);

	// repinta o control
	CDC *pDC = GetDC ();
	PaintControl (pDC);
	ReleaseDC (pDC);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInstructionsCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// rola o conte�do da tela para direita/esquerda
	int nNewPos = GetScrollPos (SB_HORZ);
	int nMinPos, nMaxPos;
	GetScrollRange (SB_HORZ, &nMinPos, &nMaxPos);

	// pega a nova posi��o
	switch (nSBCode)
	{
	case SB_RIGHT:
	case SB_LINERIGHT:
		nNewPos += 4;
		break;
	case SB_LEFT:
	case SB_LINELEFT:
		nNewPos -= 4;
		break;
	case SB_PAGERIGHT:
		nNewPos += 16;
		break;
	case SB_PAGELEFT:
		nNewPos -= 16;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		nNewPos = nPos;
		break;
	}

	// atualiza os dados
	if (nNewPos > nMaxPos)
		nNewPos = nMaxPos;
	else if (nNewPos < nMinPos)
		nNewPos = nMinPos;

	// atualiza as posi��es
	ptoStartPos.x = -nNewPos;
	SetScrollPos (SB_HORZ, nNewPos);

	// repinta o control
	CDC *pDC = GetDC ();
	PaintControl (pDC);
	ReleaseDC (pDC);

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInstructionsCtrl::UpdatePos ()
{
	// UpdatePos - atualiza o controle para a posi��o incial

	ptoStartPos.x = ptoStartPos.y = 0;
	SetScrollPos (SB_VERT, 0);
	SetScrollPos (SB_HORZ, 0);
}

void CInstructionsCtrl::SetProgram (PROGRAM *pNewProgram)
{
	// SetProgram - modifica o programa ativo

	// atualiza as vari�veis
	pProgram = pNewProgram;
	if (!pNewProgram)
		return;

	// repinta o controle
	CDC *pDC = GetDC ();
	PaintControl (pDC);
	ReleaseDC (pDC);

	// atualiza a barra de rolagem
	UpdateScroll ();
}

void CInstructionsCtrl::PaintControl (CDC *pDC)
{
	// PaintControl - repinta o controle
	CRect rRect;

	// calcula o tamanho do controle
	GetClientRect (rRect);

	// atualiza o buffer de pintura
	if (!bmpDC)
	{
		bmpDC = new CDC ();
		bmpDC->CreateCompatibleDC (pDC);
	}
	if (!bmpBuffer || ((bmpSize.cx != rRect.Width ()) || (bmpSize.cy != rRect.Height ())))
	{
		if (bmpBuffer)
			delete bmpBuffer;
		bmpBuffer = new CBitmap ();
		bmpSize.cx = rRect.Width ();
		bmpSize.cy = rRect.Height ();
		bmpBuffer->CreateCompatibleBitmap (pDC, bmpSize.cx, bmpSize.cy);
		bmpDC->SelectObject (bmpBuffer);
	}
	
	// pinta o bitmap
	if (ptoStartPos.x > 0)
		ptoStartPos.x = 0;
	if (ptoStartPos.y > 0)
		ptoStartPos.y = 0;
	PaintInstructions (bmpDC, ptoStartPos);
	pDC->BitBlt (0, 0, rRect.Width (), rRect.Height (), bmpDC, 0, 0, SRCCOPY);
}

void CInstructionsCtrl::PaintInstructions (CDC *pDC, POINT ptoStartPos)
{
	// PaintControl - pinta as instru��es
	CBrush pBackBrush (cBackColor);
	CBrush pBarBrush (cBarColor);
	CPen pBarPen (PS_SOLID, 1, cBarBorder);
	CRect rClientRect, rLabelRect, rRect, rRect2, rTrueRect, rFalseRect;
	CSize szTextSize;
	LABEL *pCurLabel;
	CString sLabelNum, sLabelText, sTrueNum, sElseText, sFalseNum;
	unsigned int i, nCount;

	// pinta a cor de fundo
	GetClientRect (rClientRect);
	pDC->FillRect (rClientRect, &pBackBrush);

	// pinta a barra
	pDC->SelectObject (pBarBrush);
	pDC->SelectObject (pBarPen);
	rRect = rClientRect;
	rRect.right = ptoStartPos.x + nLabelWidth + 1;
	INFLATE_RECT (rRect, 1);
	pDC->Rectangle (rRect);

	// verifica se tem r�tulos
	szInstrSize.cx = szInstrSize.cy = 0;
	if (!pProgram)
		return;

	// declara os outros brushes
	CBrush pActiveBrush (cActiveColor);
	CBrush pCallerBrush (cCallerColor);
	CPen pActivePen (PS_SOLID, 1, cActiveBorder);
	CPen pCallerPen (PS_SOLID, 1, cCallerBorder);

	// calcula o total de r�tulos
	nCount = pProgram->Count ();
	sElseText = "sen�o v�_para";

	// atualiza a fonte dos itens
	pDC->SelectObject (hItemsFont);
	pDC->SelectObject (pBackBrush);
	pDC->SetBkMode (TRANSPARENT);

	// atualiza os ret�ngulos padr�o
	rLabelRect.left = ptoStartPos.x;
	rLabelRect.right = rLabelRect.left + nLabelWidth;
	rRect.left = ptoStartPos.x + nLabelWidth + 4;
	rRect.top = ptoStartPos.y;
	rRect.bottom = rRect.top;

	// imprime cada um dos r�tulos
	for (i = 0; i < nCount; i++)
	{
		// pega o r�tulo atual
		pCurLabel = (*pProgram)[i];

		// formata o texto do r�tulo
		if ((pCurLabel->lType == ltOperation) || (pCurLabel->lType == ltMacroOperation))
		{
			// neste caso � uma opera��o, atualiza os �ndices
			if (pCurLabel->pTrue)
				sTrueNum.Format ("%d", pCurLabel->pTrue->nIdx + 1);
			else
				sTrueNum = "0";
			// imprime a opera��o
			if (pCurLabel->sData)
				sLabelText.Format ("fa�a %s v�_para", pCurLabel->sData);
			else
				sLabelText.Format ("fa�a %s v�_para", N_NULL);
		}
		else
		{
			// neste caso � uma condi��o, atualiza os �ndices
			if (pCurLabel->pTrue)
				sTrueNum.Format ("%d", pCurLabel->pTrue->nIdx + 1);
			else
				sTrueNum = "0";
			if (pCurLabel->pFalse)
				sFalseNum.Format ("%d", pCurLabel->pFalse->nIdx + 1);
			else
				sFalseNum = "0";
			// imprime a condi��o
			sLabelText.Format ("se %s ent�o v�_para", pCurLabel->sData);
		}

		// calcula o tamanho do texto principal
		szTextSize = pDC->GetTextExtent (sLabelText);

		// imprime o fundo de sele��o
		if (pActiveLabel == pCurLabel)
		{
			rClientRect.left = ptoStartPos.x;
			rClientRect.top = rRect.bottom + 2;
			rClientRect.bottom = rClientRect.top + szTextSize.cy + 4;
			pDC->SelectObject (pActivePen);
			pDC->SelectObject (pActiveBrush);
			pDC->Rectangle (rClientRect);
			pDC->SelectObject (pBackBrush);
		}

		// calcula a posi��o do r�tulo
		rLabelRect.top = rRect.bottom + 4;
		rLabelRect.bottom = rLabelRect.top + szTextSize.cy;

		// calcula a posi��o do texto principal do r�tulo
		rRect.top = rLabelRect.top;
		rRect.right = rRect.left + szTextSize.cx;
		rRect.bottom = rLabelRect.bottom;

		// calcula a posi��o do pulo verdadeiro
		szTextSize = pDC->GetTextExtent (sTrueNum);
		rTrueRect.left = rRect.right + 4;
		rTrueRect.top = rLabelRect.top;
		rTrueRect.right = rTrueRect.left + szTextSize.cx;
		rTrueRect.bottom = rLabelRect.bottom;

		// calcula a posi��o do texto complementar e do pulo falso
		if ((pCurLabel->lType == ltTest) || (pCurLabel->lType == ltMacroTest))
		{
			// calcula o texto complementar
			szTextSize = pDC->GetTextExtent (sElseText);
			rRect2.left = rTrueRect.right + 4;
			rRect2.top = rLabelRect.top;
			rRect2.right = rRect2.left + szTextSize.cx;
			rRect2.bottom = rLabelRect.bottom;

			// calcula o pulo falso
			szTextSize = pDC->GetTextExtent (sFalseNum);
			rFalseRect.left = rRect2.right + 4;
			rFalseRect.top = rLabelRect.top;
			rFalseRect.right = rFalseRect.left + szTextSize.cx;
			rFalseRect.bottom = rLabelRect.bottom;
		}

		// imprime o fundo do caller label
		if (pCallerLabel == pCurLabel)
		{
			pDC->SelectObject (pCallerPen);
			pDC->SelectObject (pCallerBrush);
			if (bCallerWay)
			{
				INFLATE_RECT (rTrueRect, 2);
				pDC->Rectangle (rTrueRect);
				INFLATE_RECT (rTrueRect, -2);
			}
			else
			{
				INFLATE_RECT (rFalseRect, 2);
				pDC->Rectangle (rFalseRect);
				INFLATE_RECT (rFalseRect, -2);
			}
			pDC->SelectObject (pBackBrush);
		}

		// imprime o r�tulo no device
		sLabelNum.Format ("%d:", i + 1);
		pDC->DrawText (sLabelNum, rLabelRect, DT_SINGLELINE | DT_RIGHT);
		pDC->DrawText (sLabelText, rRect, DT_SINGLELINE);
		pDC->DrawText (sTrueNum, rTrueRect, DT_SINGLELINE);
		if (szInstrSize.cx < (rTrueRect.right + ptoStartPos.x))
			szInstrSize.cx = rTrueRect.right + ptoStartPos.x;
		if ((pCurLabel->lType == ltTest) || (pCurLabel->lType == ltMacroTest))
		{
			pDC->DrawText (sElseText, rRect2, DT_SINGLELINE);
			pDC->DrawText (sFalseNum, rFalseRect, DT_SINGLELINE);
			if (szInstrSize.cx < (rFalseRect.right + ptoStartPos.x))
				szInstrSize.cx = rFalseRect.right + ptoStartPos.x;
		}
		if (szInstrSize.cy < (rRect.bottom + ptoStartPos.y + 4))
			szInstrSize.cy = rRect.bottom + ptoStartPos.y + 4;

		// atualiza o rect da instru��o
		pCurLabel->rInstructRect = rRect;
	}
}

void CInstructionsCtrl::UpdateScroll ()
{
	// UpdateScroll - atualiza a barra de rolagem
	RECT rRect;
	GetClientRect (&rRect);

	// updates the vertical scroll bar
	if (szInstrSize.cy > (rRect.bottom - rRect.top))
	{
		EnableScrollBar (SB_VERT);
		SetScrollRange (SB_VERT, 0, szInstrSize.cy - (rRect.bottom - rRect.top), TRUE);
	}
	else
	{
		EnableScrollBar (SB_VERT, ESB_DISABLE_BOTH);
		ptoStartPos.y = 0;
		SetScrollPos (SB_VERT, 0);
	}

	// updates the horizontal scroll bar
	if (szInstrSize.cx > (rRect.right - rRect.left))
	{
		EnableScrollBar (SB_HORZ);
		SetScrollRange (SB_HORZ, 0, szInstrSize.cx - (rRect.right - rRect.left), TRUE);
	}
	else
	{
		EnableScrollBar (SB_HORZ, ESB_DISABLE_BOTH);
		ptoStartPos.x = 0;
		SetScrollPos (SB_HORZ, 0);
	}
}


void CInstructionsCtrl::SelectItem (LABEL *pLabel)
{
	// SelectItem - seleciona uma instru��o, mantendo o rastro

	// atualiza as instru��es ativas
	if (pActiveLabel)
	{
		pCallerLabel = pActiveLabel;
		if (pActiveLabel->pTrue == pLabel)
			bCallerWay = true;
		else if (pActiveLabel->pFalse == pLabel)
			bCallerWay = false;
		else
			pCallerLabel = NULL;
	}
	else
		pCallerLabel = NULL;
	pActiveLabel = pLabel;

	// verifica se mandou um r�tulo nulo
	if (!pLabel)
	{
		CDC *pDC = GetDC ();
		PaintControl (pDC);
		ReleaseDC (pDC);
		return;
	}
		
	// verifica se a instru��o est� na tela
	RECT rRect;
	GetClientRect (&rRect);
	
	// atualiza a posi��o
	ptoStartPos.x = 0;
	if (pLabel->rInstructRect.top < 0)
		ptoStartPos.y -= pLabel->rInstructRect.top;
	else if (pLabel->rInstructRect.bottom > (rRect.bottom - rRect.top))
		ptoStartPos.y -= pLabel->rInstructRect.bottom - (rRect.bottom - rRect.top);

	// atualiza a posi��o das barras de rolagem
	SetScrollPos (SB_VERT, -ptoStartPos.y);
	SetScrollPos (SB_HORZ, 0);

	// repinta o controle
	CDC *pDC = GetDC ();
	PaintControl (pDC);
	ReleaseDC (pDC);
}