#if !defined(DLG_GET_VAL_FROM_LIST__INCLUDED_)
#define DLG_GET_VAL_FROM_LIST__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DlgGetVal.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// DlgGetVal dialog

class CDlgGetValFromList : public CDialog
//class CDlgGetValFromList : public CDialog
{
NON_COPYABLE(CDlgGetValFromList);
// Construction
public:
    CDlgGetValFromList(const CString& szDlgTitle, const std::vector<std::string>& listValues, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(DlgGetVal)
    enum { IDD = IDD_GET_VAL_FROM_LIST };
//}}AFX_DATA

    std::string GetSelectedValue(void) {return m_selectedValue;};
    int GetIdxValue(void) {return m_selectedIdx;};

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(DlgGetVal)
protected:
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
    afx_msg void OnLbnDblclkCtrlLBValues();
protected:

    // Generated message map functions
    //{{AFX_MSG(DlgGetVal)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    const std::vector<std::string>& m_listValues;
    std::string                     m_selectedValue;
    int                             m_selectedIdx;
    CString                         m_szDlgTitle;
    CListBox                        m_ctrlLBValues;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGETVAL_H__FEF2CEDD_4F70_4C0C_9A4C_AF4363550D4D__INCLUDED_)
