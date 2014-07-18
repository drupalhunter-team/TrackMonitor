/*************************************************************************

    LineCbBox.h  
    ComboBox for line attributes selection
    André Deperrois - October 2003


    Credits To CodeGuru - Joel Wahlberg for Icon_Combox_Box    from which this class is derived

    Compatibility: Visual C++ 6.0 and up.

*************************************************************************/


#if !defined(AFX_LINECBBOX_H__CDE42B1A_BEDD_416E_84AE_D00EF1573D4D__INCLUDED_)
#define AFX_LINECBBOX_H__CDE42B1A_BEDD_416E_84AE_D00EF1573D4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LineCbBox.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CLineCbBox window


struct LineData{
    public:
        int nPenStyle;
        int nWidth;
        COLORREF crColor; 
};


class CLineCbBox : public CComboBox{
// Construction
public:
    CLineCbBox();

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLineCbBox)
    //}}AFX_VIRTUAL

// Implementation
public:
    LineData m_Lines[10];
    void DrawLine(LPDRAWITEMSTRUCT lpDIS);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
    virtual int DeleteString(int nIndex);
    virtual int InsertString(int nIindex, LPCTSTR lpszString);
    virtual int AddString(LPCTSTR lpszString);
    int AddLine();
    virtual ~CLineCbBox();

    // Generated message map functions
protected:
    //{{AFX_MSG(CLineCbBox)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINECBBOX_H__CDE42B1A_BEDD_416E_84AE_D00EF1573D4D__INCLUDED_)
