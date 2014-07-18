#if !defined(AFX_DLGGETVAL_H__FEF2CEDD_4F70_4C0C_9A4C_AF4363550D4D__INCLUDED_)
#define AFX_DLGGETVAL_H__FEF2CEDD_4F70_4C0C_9A4C_AF4363550D4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SecureEditEx.h"

// DlgGetPwd.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// DlgGetPwd dialog

class CDlgGetPwd : public CDialog
//class CDlgGetPwd : public CDialog
{
// Construction
public:
    typedef enum {REAL_CONNECTION, PSEUDO_CONNECTION, NONE} T_CONNECTION_TYPE;
    CDlgGetPwd(CWnd* pParent = NULL);   // standard constructor
    ~CDlgGetPwd() {DeletePassword();};

    LPTSTR GetPassword(void )  {return m_pPassword; };
    void DeletePassword(void ) {if (m_pPassword) m_edPwd.DeletePassword(m_pPassword); m_pPassword = NULL;};
    T_CONNECTION_TYPE GetConnectionType()  {return (T_CONNECTION_TYPE)m_nConnectionType;};


// Dialog Data
    //{{AFX_DATA(DlgGetPwd)
    enum { IDD = IDD_GET_PWD };
    CSecureEditEx m_edPwd;
    LPSTR         m_pPassword;
//}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(DlgGetPwd)
    protected:
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(DlgGetPwd)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    int  m_nConnectionType;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGETVAL_H__FEF2CEDD_4F70_4C0C_9A4C_AF4363550D4D__INCLUDED_)
