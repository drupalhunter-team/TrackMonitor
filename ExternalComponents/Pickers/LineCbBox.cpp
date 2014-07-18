// LineCbBox.cpp : implementation file
//

#include "stdafx.h"
#include "LineCbBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineCbBox

CLineCbBox::CLineCbBox(){
}

CLineCbBox::~CLineCbBox(){
}


BEGIN_MESSAGE_MAP(CLineCbBox, CComboBox)
    //{{AFX_MSG_MAP(CLineCbBox)
    ON_WM_DRAWITEM()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineCbBox message handlers

int CLineCbBox::AddLine(){
    
    CComboBox::AddString("");
    int i = GetCount();
    if(SetItemDataPtr(i-1, m_Lines+i-1)==CB_ERR){
        ASSERT(false);
    }
    return i;
}

int CLineCbBox::AddString(LPCTSTR /*lpszString*/){
    ASSERT(FALSE); // cannot add strings 
    return CB_ERR;
}

int CLineCbBox::InsertString(int /*nIindex*/, LPCTSTR /*lpszString*/){
    ASSERT(FALSE); // cannot insert strings
    return CB_ERR;

}

int CLineCbBox::DeleteString(int /*nIndex*/){
    ASSERT(FALSE); // cannot delete strings
    return CB_ERR;
}

void CLineCbBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS){
    lpMIS->itemHeight = 20;
}


void CLineCbBox::DrawItem(LPDRAWITEMSTRUCT lpDIS){
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    
    if (!IsWindowEnabled())    {
        CBrush brDisabled(RGB(192,192,192)); // light gray
        CBrush* pOldBrush = pDC->SelectObject(&brDisabled);
        CPen penDisabled(PS_SOLID, 1, RGB(192,192,192));
        CPen* pOldPen = pDC->SelectObject(&penDisabled);
        DrawLine(lpDIS);
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
        return;
    }
    
    // Selected
    if ((lpDIS->itemState & ODS_SELECTED) 
        && (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {
        CBrush brHighlight(GetSysColor(COLOR_HIGHLIGHT)); 
        CBrush* pOldBrush = pDC->SelectObject(&brHighlight);
        CPen penHighlight(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT));
        CPen* pOldPen = pDC->SelectObject(&penHighlight);
        pDC->Rectangle(&lpDIS->rcItem);
        pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
        pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
        DrawLine(lpDIS);
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
    }
    else{
    // De-Selected
//    if (!(lpDIS->itemState & ODS_SELECTED) 
//        && (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {
        CBrush brWindow(GetSysColor(COLOR_WINDOW)); 
        CBrush* pOldBrush = pDC->SelectObject(&brWindow);
        CPen penHighlight(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
        CPen* pOldPen = pDC->SelectObject(&penHighlight);
        pDC->Rectangle(&lpDIS->rcItem);
        pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
        pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
        DrawLine(lpDIS);
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
    }

    // Focus
    if (lpDIS->itemAction & ODA_FOCUS)     { 
        pDC->DrawFocusRect(&lpDIS->rcItem);
    }
}


void CLineCbBox::DrawLine(LPDRAWITEMSTRUCT lpDIS){
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);

    LineData* pLineData = (LineData*) GetItemDataPtr(lpDIS->itemID);
    ASSERT(pLineData != NULL);

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    lb.lbColor = pLineData->crColor;
    CPen CurPen(PS_GEOMETRIC | pLineData->nPenStyle, pLineData->nWidth, &lb);

    CPen *pOldPen = pDC->SelectObject(&CurPen);
    pDC->MoveTo(lpDIS->rcItem.left+5, (lpDIS->rcItem.bottom + lpDIS->rcItem.top) / 2);
    pDC->LineTo(lpDIS->rcItem.right-5, (lpDIS->rcItem.bottom + lpDIS->rcItem.top) / 2);
    pDC->SelectObject(pOldPen);
}




