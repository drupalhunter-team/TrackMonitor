#if !defined(DLG_GET_VAL_ENUM__INCLUDED_)
#define DLG_GET_VAL_ENUM__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgGetValEnum.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgGetValEnum dialog

class CDlgGetValEnum : public CDialog
{
NON_COPYABLE(CDlgGetValEnum);
// Construction
public:
    std::vector<std::string> comboValues;
    CDlgGetValEnum(const CString& szDlgTitle, const CString& szInitialValue = CString(""), CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CDlgGetValEnum)
    enum { IDD = IDD_GET_VAL_ENUM };
    CString    m_szValue;
    CString    m_szDlgTitle;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgGetValEnum)
    protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDlgGetValEnum)
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGETVALENUM_H__744F50D5_621D_4CDD_94DC_4517F304D39A__INCLUDED_)
