// LuaChildFrm.cpp : implémentation de la classe CLuaChildFrame
//
#include "stdafx.h"
#include "OngTV.h"

#include <iomanip>
#include "LuaChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const UINT BASED_CODE buttons[] =
{
    // same order as in the bitmap 'LuaScriptToolBar.bmp'
    ID_LUASCRIPT_SAVE,
    ID_LUASCRIPT_SAVEAS,
        ID_SEPARATOR,
    ID_LUASCRIPT_RUN,
    ID_LUASCRIPT_TOGGLE_BREAKPOINT,
        ID_SEPARATOR,
    ID_LUASCRIPT_STOP,
    ID_LUASCRIPT_BREAK,
        ID_SEPARATOR,
    ID_LUASCRIPT_STEP_INTO,
    ID_LUASCRIPT_STEP_OVER,
    ID_LUASCRIPT_STEP_OUT,
    ID_LUASCRIPT_RUN_TO_CURSOR,
};

// CLuaChildFrame

IMPLEMENT_DYNCREATE(CLuaChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CLuaChildFrame, CMDIChildWndEx)
    ON_WM_CREATE()
END_MESSAGE_MAP()


// construction ou destruction de CLuaChildFrame

CLuaChildFrame::CLuaChildFrame()
{
    // TODO : ajoutez ici le code d'une initialisation de membre
}

CLuaChildFrame::~CLuaChildFrame()
{
}

int CLuaChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    // TODO : changez ici la classe ou les styles Window en modifiant CREATESTRUCT cs
    if(-1 == CMDIChildWndEx::OnCreate(lpCreateStruct) )
        return -1;

    if (  !m_wndToolBar.Create(this)
       || !m_wndToolBar.LoadBitmap(IDR_LUASCRIPT)
       || !m_wndToolBar.SetButtons(buttons, NB_ELEMS_OF(buttons))
       )

    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }
    m_wndToolBar.SetPaneStyle(CBRS_TOOLTIPS | CBRS_FLYBY| CBRS_TOP); 
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndToolBar);
    return 0;
}


// diagnostics pour CLuaChildFrame

#ifdef _DEBUG
void CLuaChildFrame::AssertValid() const
{
    CMDIChildWndEx::AssertValid();
}

void CLuaChildFrame::Dump(CDumpContext& dc) const
{
    CMDIChildWndEx::Dump(dc);
}

#endif //_DEBUG


// gestionnaires de messages pour CLuaChildFrame
