// CDlgGetValNum.cpp : implementation file
//

#include "stdafx.h"
#include "OngTV.h"
#include "DlgGetValNum.h"

#if 0
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValNum dialog


CDlgGetValNum::CDlgGetValNum(const CString& szDlgTitle, const CString& szValueCaption /*= CString("")*/, const CString& szInitialValue /*= CString("")*/, CWnd* pParent /*=NULL*/)
    : CDialog(CDlgGetValNum::IDD, pParent), m_szDlgTitle(szDlgTitle), m_szValueCaption(szValueCaption), m_szValue(szInitialValue)
{
}

BOOL CDlgGetValNum::OnInitDialog() 
{
    CDialog::OnInitDialog();
    SetWindowText(m_szDlgTitle);
    if (0 != m_szValueCaption.GetLength())
    {
        GetDlgItem(IDC_STATIC_VALUE)->SetWindowText(m_szValueCaption);
    }
    UpdateData(FALSE); // mise à jour des controles
    m_edValue.SetFocus();
// On retourne false pour donner le focus a IDC_EDIT_VALUE
    return FALSE;
}

void CDlgGetValNum::OnOK()
{
    CDialog::OnOK();
}


void CDlgGetValNum::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgGetValNum)
    DDX_Control(pDX, IDC_EDIT_VALUE, m_edValue);
    DDX_Text(pDX, IDC_EDIT_VALUE, m_szValue);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGetValNum, CDialog)
    //{{AFX_MSG_MAP(CDlgGetValNum)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValNum message handlers
