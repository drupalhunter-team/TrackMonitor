// CLuaChildFrame.h : interface de la classe CLuaChildFrame
//


#pragma once
#include "AdeMFCToolBar.h"


class CLuaChildFrame : public CMDIChildWndEx
{
NON_COPYABLE(CLuaChildFrame);
    DECLARE_DYNCREATE(CLuaChildFrame)
public:
    CLuaChildFrame();


// Attributs
private:
    CAdeMFCToolBar  m_wndToolBar;

// Opérations
public:

// Implémentation
public:
    virtual ~CLuaChildFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

// Fonctions générées de la table des messages
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};
