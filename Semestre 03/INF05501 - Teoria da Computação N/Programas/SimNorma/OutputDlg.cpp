/********************************************************************
	IMPLEMENTA��O DA JANELA DE SA�DA
	Cria��o:	02/08/2003
	Copyright:	(c) 2003 Marcelo Bona Boff, Fernando Trebien
						 Leonardo Golob
********************************************************************/


#include "stdafx.h"
#include "SimNorma.h"
#include "OutputDlg.h"


/********************************************************************
	INICIALIZA��O DA CLASSE DA JANELA DE SA�DA */

IMPLEMENT_DYNAMIC(COutputDlg, CDialog)

BEGIN_MESSAGE_MAP(COutputDlg, CDialog)
END_MESSAGE_MAP()


/********************************************************************
	CLASSE DA JANELA DE SA�DA */

COutputDlg::COutputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COutputDlg::IDD, pParent)
{
}

COutputDlg::~COutputDlg()
{
}

void COutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT, m_lstOutput);
}