// MainFrm.h : interface de la classe CMainFrame
//


#pragma once

#include <afxMDIFrameWndEx.h>
#include <afxstatusbar.h>
#include <afxtoolbar.h>
#include <afxmenubar.h>
#include "AdeMFCToolBar.h"
#include "TraceBar.h"
//#include "DragDrop.h"
#include "LuaInterpreter.h"


class CMainFrame : public CMDIFrameWndEx
{
NON_COPYABLE(CMainFrame);
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

// Attributs
public:

// Opérations
public:
    void SendToTraceWindow(char* szTrace, CTraceBar::T_TYPE_TRACE typeTrace, COLORREF color = RGB(0,0,0));
    void ClearTrace(CTraceBar::T_TYPE_TRACE typeTrace);
    void ShowTrace(CTraceBar::T_TYPE_TRACE typeTrace);

    // Pour positionner le fichier de configuration
    void SetConfigFile(const std::string& configFileName) {m_ConfigFileName = configFileName;};
// Substitutions
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implémentation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // membres incorporés de la barre de contrôle
    CMFCStatusBar     m_wndStatusBar;
    CAdeMFCToolBar    m_wndToolBar;
	CAdeMFCMenuBar    m_wndMenuBar;

    // fenetre de trace
    CTraceBar        m_TraceBar;

    // Fonctions générées de la table des messages
private:
    std::string m_ConfigFileName;    // nom du fichier de configuration
    std::string m_ConfigFilePath;    // path du fichier de configuration
    std::string m_strProject;        // nom du projet (extrait du fichier de configuration)
    bool        m_bIsGeneric;        // est-ce la version generique de l'outil
    bool        m_bAllowManageTrace;             // Bool pour contourner un pb (voir commentaire dans la methode ClearTrace)

    class CUserCmdEntry {
    public:
        std::string  strLabel;          // label de l'entree
        std::string  strDesc;           // chaine decrivant l'entree
        UINT         nID;               // ID associe a la commande
        std::string  luaScripFileName;  // nom du fichier scipt LUA associe
        CLuaInterpreter itp;            // interpreteur utilisé pour lancer le script
    };
    boost::ptr_vector<CUserCmdEntry> m_arrUserCmds;    // tableau contenant les entrees de menu user
    CString  m_szMenuUserCmdsLabel;  // label du menu contenant les commandes utilisateur, il permet de modifier
                                     // le menu a chaque connexion (on ne peut pas utiliser de pointeur sur
                                     // le menu car il n'est manifestement pas persistant).
                                     // interpreteur utilisé pour lancer le script OnOpen ou OnConnect
    CLuaInterpreter m_LuaInterpreter;

    void CreateMenuAndToolbar(UINT nIDMenuAndToolbarIcons, UINT *pButtons, int nButtons);
protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void GetMessageString(UINT nID, CString& rMessage) const;
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
                          AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnCmdNewLUAScript();
    afx_msg void OnCmdOpenLUAScript();
    afx_msg void OnUpdateCmdNewLUAScript(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCmdOpenLUAScript(CCmdUI* pCmdUI);
	afx_msg void OnViewTrace();
	afx_msg void OnUpdateViewTrace(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()
};


