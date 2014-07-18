// CDlgGetValFromList.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgGetValFromList.h"

#if 0
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValFromList dialog


CDlgGetValFromList::CDlgGetValFromList(const CString& szDlgTitle, const std::vector<std::string>& listValues, CWnd* pParent /*=NULL*/)
    : CDialog(CDlgGetValFromList::IDD, pParent), m_szDlgTitle(szDlgTitle), m_listValues(listValues), m_selectedIdx(-1)
{
}

BOOL CDlgGetValFromList::OnInitDialog() 
{
    CDialog::OnInitDialog();
    SetWindowText(m_szDlgTitle);
    // remplissage de la list box
    for (unsigned i = 0; i < m_listValues.size(); ++i)
    {
        m_ctrlLBValues.InsertString(i, m_listValues[i].c_str());
    }
    UpdateData(FALSE); // mise à jour des controles
    return FALSE;
}

void CDlgGetValFromList::OnOK()
{
    m_selectedIdx = m_ctrlLBValues.GetCurSel();
    if (m_selectedIdx == CB_ERR)
    {
        return;
    }
	CString strSelected;
    m_ctrlLBValues.GetText(m_selectedIdx, strSelected);
    m_selectedValue = std::string((LPCSTR)strSelected);
    CDialog::OnOK();
}
void CDlgGetValFromList::OnLbnDblclkCtrlLBValues()
{
    // equivalent d'un OnOK
    OnOK();
}


void CDlgGetValFromList::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgGetValFromList)
    DDX_Control(pDX, IDC_LIST_VALUES, m_ctrlLBValues);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGetValFromList, CDialog)
    //{{AFX_MSG_MAP(CDlgGetValFromList)
    ON_LBN_DBLCLK(IDC_LIST_VALUES, &CDlgGetValFromList::OnLbnDblclkCtrlLBValues)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValFromList message handlers
