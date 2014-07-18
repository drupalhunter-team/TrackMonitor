
#include "stdafx.h"
#include "TCHAR.h"
#include "EditNumHexa.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CEditHexa, CEdit)
    //{{AFX_MSG_MAP(CEditHexa)
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditHexa::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar){
        case 'a' : DefWindowProc(WM_CHAR, 'A', MAKELONG(nRepCnt, nFlags));return;
        case 'A' : break;
        case 'b' : DefWindowProc(WM_CHAR, 'B', MAKELONG(nRepCnt, nFlags));return;
        case 'B' : break;
        case 'c' : DefWindowProc(WM_CHAR, 'C', MAKELONG(nRepCnt, nFlags));return;
        case 'C' : break;
        case 'd' : DefWindowProc(WM_CHAR, 'D', MAKELONG(nRepCnt, nFlags));return;
        case 'D' : break;
        case 'e' : DefWindowProc(WM_CHAR, 'E', MAKELONG(nRepCnt, nFlags));return;
        case 'E' : break;
        case 'f' : DefWindowProc(WM_CHAR, 'F', MAKELONG(nRepCnt, nFlags));return;
        case 'F' : break;
        case '0' :
        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case '8' :
        case '9' :
        case VK_BACK :
        case VK_TAB :
        case VK_RETURN :
        case VK_ESCAPE : break;
        default: return;
    }
    
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

BEGIN_MESSAGE_MAP(CEditNum, CEdit)
    //{{AFX_MSG_MAP(CEditNum)
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditNum::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar){
        case '0' :
        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case '8' :
        case '9' :
        case VK_BACK :
        case VK_TAB :
        case VK_RETURN :
        case VK_ESCAPE : break;
        default: return;
    }
    
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

BEGIN_MESSAGE_MAP(CEditBin, CEdit)
    //{{AFX_MSG_MAP(CEditNum)
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditBin::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar){
        case '0' :
        case '1' :
        case VK_BACK :
        case VK_TAB :
        case VK_RETURN :
        case VK_ESCAPE : break;
        default: return;
    }
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}
