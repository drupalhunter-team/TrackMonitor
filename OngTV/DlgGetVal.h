#if !defined(DLG_GET_VAL__INCLUDED_)
#define DLG_GET_VAL__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DlgGetVal.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// DlgGetVal dialog

class CDlgGetVal : public CDialog
//class CDlgGetVal : public CDialog
{
NON_COPYABLE(CDlgGetVal);
// Construction
public:
    CDlgGetVal(const CString& szDlgTitle, const CString& szValueCaption = CString(""), const CString& szInitialValue = CString(""), CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(DlgGetVal)
    enum { IDD = IDD_GET_VAL };
    CString    m_szValue;
    CString    m_szDlgTitle;
    CString    m_szValueCaption;
    CEdit      m_edValue;
//}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(DlgGetVal)
    protected:
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(DlgGetVal)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGETVAL_H__FEF2CEDD_4F70_4C0C_9A4C_AF4363550D4D__INCLUDED_)
