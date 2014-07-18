// TraceBar.h : header file
//

#ifndef _TRACEBAR_H
#define _TRACEBAR_H

#include <afxDockablePane.h>
#include <afxtabctrl.h>
#include "OXHistoryCtrl.h"
#include "AdeFifo.h"

class CAdeThreadAsMember;

class CAdeOXHistoryCtrl : public COXHistoryCtrl
{
NON_COPYABLE(CAdeOXHistoryCtrl);
public:
    CAdeOXHistoryCtrl(bool bLogToFileAllowed) {m_bLogToFileAllowed = bLogToFileAllowed; m_bLogInProgress = false;};
    virtual ~CAdeOXHistoryCtrl() {};
    DECLARE_DYNAMIC(CAdeOXHistoryCtrl);
protected:
    virtual BOOL OnPopulateContextMenu(CMenu* pMenu, CPoint& point);    
    afx_msg void OnClearHistory();
    afx_msg void OnActivateLog();
    afx_msg void OnDeactivateLog();
    DECLARE_MESSAGE_MAP()
private:
    bool m_bLogToFileAllowed;
    bool m_bLogInProgress;
};

class CTraceBar : public CDockablePane
{
NON_COPYABLE(CTraceBar);
// Construction
public:
	CTraceBar();

// Attributes
public:

// Operations
public:
    typedef enum {CONSOLE_TRACE, LUA_TRACE} T_TYPE_TRACE;
    void AddToTraceWindow(char* szTrace, T_TYPE_TRACE typeTrace, COLORREF color = RGB(0,0,0));
    void ClearTraceWindow(T_TYPE_TRACE typeTrace);
    void ShowTraceWindow(T_TYPE_TRACE typeTrace);

    void SetLuaWindowTraceToCreate(bool bLuaWindowTraceToCreate) {m_bLuaWindowTraceToCreate = bLuaWindowTraceToCreate;};

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTraceBar)
// Implementation
public:
    virtual ~CTraceBar();

    // Generated message map functions
protected:
    //{{AFX_MSG(CTraceBar)
    afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    // fonction statique passe au thread de managemeent
    static UINT StaticProcThreadManagement(LPVOID lpParam, HANDLE hEventStop);
    // corps du thread de management
    UINT ProcThreadManagement(HANDLE hEventStop);

    // vidage de la fifo
    void ClearFifoTraces(void);
    class CTraceMsg{
    public:
        CTraceMsg(char *szTrace, COLORREF color, T_TYPE_TRACE typeTrace)
        {
            m_color     = color;
            m_typeTrace = typeTrace;
            strncpy_s(m_szTrace, sizeof(m_szTrace), szTrace, _TRUNCATE);
        };
        char         m_szTrace[1000];   // chaine a afficher
        COLORREF     m_color;     // couleur de la trace
        T_TYPE_TRACE m_typeTrace; // type de la trace
    };
    CAdeFifo<CTraceMsg>         m_fifoTraces;          // fifo des traces a afficher recus
    
    // Note : on utilise un pointeur et on cree dynamiquement les objets suivants plutot
    // que d'utiliser des objets membres de la classe car dans ce dernier cas, on a une erreur a la destruction (Heap abimee ?)
    boost::unique_ptr<CAdeOXHistoryCtrl>     m_pHistoConsoleTraces;
    boost::unique_ptr<CAdeOXHistoryCtrl>     m_pHistoLuaTraces;       // pour output script LUA

    // gestionnaire des onglets
	CMFCTabCtrl	m_wndTabs;

    HANDLE m_hMutexDico;        // Mutex de protection du dictionnaire (entre le thread et les appelants)

    bool        m_bLuaWindowTraceToCreate;   // faut-il creer la fenetre de trace LUA
    unsigned    m_idxHistoLuaTracesPage;     // index de page de traces LUA

    boost::unique_ptr<CAdeThreadAsMember> m_pThreadManagement; // Thread de management
};
#endif // _TRACEBAR_H