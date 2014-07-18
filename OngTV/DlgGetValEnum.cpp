// DlgGetValEnum.cpp : implementation file
//

#include "stdafx.h"
#include "OngTV.h"
#include "DlgGetValEnum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValEnum dialog


CDlgGetValEnum::CDlgGetValEnum(const CString& szDlgTitle, const CString& szInitialValue /*= CString("")*/, CWnd* pParent /*=NULL*/)
    : CDialog(CDlgGetValEnum::IDD, pParent), m_szDlgTitle(szDlgTitle), m_szValue(szInitialValue)
{
}


BOOL CDlgGetValEnum::OnInitDialog() 
{
    CDialog::OnInitDialog();
    SetWindowText(m_szDlgTitle);
    GetDlgItem(IDC_EDIT_VALUE)->SetFocus();
    // On retourne false pour donner le focus a IDC_EDIT_VALUE
    return FALSE;
}

void CDlgGetValEnum::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgGetValEnum)
    DDX_CBString(pDX, IDC_EDIT_VALUE, m_szValue);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGetValEnum, CDialog)
    //{{AFX_MSG_MAP(CDlgGetValEnum)
    ON_WM_SHOWWINDOW()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValEnum message handlers

void CDlgGetValEnum::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    CDialog::OnShowWindow(bShow, nStatus);
    
    // TODO: Add your message handler code here
    CComboBox * combo=(CComboBox *)GetDlgItem(IDC_EDIT_VALUE);
    combo->ResetContent();
    for (unsigned i=0 ; i<comboValues.size() ; ++i)
    {
        combo->AddString(comboValues[i].c_str());
    }
}
