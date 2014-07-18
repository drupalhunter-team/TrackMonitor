// CDlgGetValBin.cpp : implementation file
//

#include "stdafx.h"
#include "OngTV.h"
#include "DlgGetValBin.h"

#if 0
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValBin dialog


CDlgGetValBin::CDlgGetValBin(const CString& szDlgTitle, const CString& szValueCaption /*= CString("")*/, const CString& szInitialValue /*= CString("")*/, CWnd* pParent /*=NULL*/)
    : CDialog(CDlgGetValBin::IDD, pParent), m_szDlgTitle(szDlgTitle), m_szValueCaption(szValueCaption), m_szValue(szInitialValue)
{
}

BOOL CDlgGetValBin::OnInitDialog() 
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

void CDlgGetValBin::OnOK()
{
    CDialog::OnOK();
}


void CDlgGetValBin::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgGetValBin)
    DDX_Text(pDX, IDC_EDIT_VALUE, m_szValue);
    DDX_Control(pDX, IDC_EDIT_VALUE, m_edValue);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGetValBin, CDialog)
    //{{AFX_MSG_MAP(CDlgGetValBin)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValBin message handlers
