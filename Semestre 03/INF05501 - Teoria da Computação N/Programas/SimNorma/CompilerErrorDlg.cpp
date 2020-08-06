/********************************************************************
	IMPLEMENTA��O DA JANELA DE ERRO DE COMPILA��O
	Cria��o:	02/08/2003
	Copyright:	(c) 2003 Marcelo Bona Boff, Fernando Trebien
						 Leonardo Golob
********************************************************************/


#include "stdafx.h"
#include "SimNorma.h"
#include "CompilerErrorDlg.h"


/********************************************************************
	INICIALIZA��O DA CLASSE DA JANELA DE ERRO DE COMPILA��O */

IMPLEMENT_DYNAMIC(CCompilerErrorDlg, CDialog)

BEGIN_MESSAGE_MAP(CCompilerErrorDlg, CDialog)
END_MESSAGE_MAP()


/********************************************************************
	CLASSE DA JANELA DE ERRO DE COMPILA��O */

CCompilerErrorDlg::CCompilerErrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompilerErrorDlg::IDD, pParent)
{
}

CCompilerErrorDlg::~CCompilerErrorDlg()
{
}

void CCompilerErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ERRORICON, m_imgIcon);
	DDX_Control(pDX, IDC_ERROR_FILE, m_txtErrorFile);
	DDX_Control(pDX, IDC_ERROR_DESCRIP, m_txtErrorDescrip);
}

BOOL CCompilerErrorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// atualiza o �cone
	m_imgIcon.SetIcon (LoadIcon (NULL, IDI_ERROR));

	// atualiza os textos de erro
	m_txtErrorFile.SetWindowText (sErrorFile);
	m_txtErrorDescrip.SetWindowText (sErrorDescrip);

	return TRUE;
}
