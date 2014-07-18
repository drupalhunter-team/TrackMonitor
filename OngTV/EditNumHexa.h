
#if !defined(EDIT_HEXA_H__INCLUDED_)
#define EDIT_HEXA_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CEditHexa : public CEdit
{
NON_COPYABLE(CEditHexa);
public:
    CEditHexa() {};
protected:
    //{{AFX_MSG(CEditHexa)
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};
 
class CEditNum : public CEdit
{
NON_COPYABLE(CEditNum);
public:
    CEditNum() {};
protected:
    //{{AFX_MSG(CEditNum)
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};
 
class CEditBin : public CEdit
{
NON_COPYABLE(CEditBin);
public:
    CEditBin() {};
protected:
    //{{AFX_MSG(CEditNum)
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
 
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(EDIT_HEXA_H__INCLUDED_)
