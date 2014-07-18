#if !defined(DLG_GET_VAL_NUM__INCLUDED_)
#define DLG_GET_VAL_NUM__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EditNumHexa.h"

// DlgGetValNum.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// DlgGetValNum dialog

class CDlgGetValNum : public CDialog
//class CDlgGetValNum : public CDialog
{
NON_COPYABLE(CDlgGetValNum);
// Construction
public:
    CDlgGetValNum(const CString& szDlgTitle, const CString& szValueCaption = CString(""), const CString& szInitialValue = CString(""), CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(DlgGetValNum)
    enum { IDD = IDD_GET_VAL };
    CString    m_szValue;
    CString    m_szDlgTitle;
    CString    m_szValueCaption;
    CEditNum   m_edValue;
//}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(DlgGetValNum)
    protected:
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(DlgGetValNum)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DLG_GET_VAL_HEXA__INCLUDED_)
