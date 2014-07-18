// MainFrm.cpp : implémentation de la classe CMainFrame
//

#include "stdafx.h"
#include <afxvisualmanagervs2005.h>
//#include <afxvisualmanagerOffice2007.h>
#include "OngTV.h"

#include "MainFrm.h"
#include "version.h"
#include "PathMgr.h"
#include "DllMgr.h"
#include "LuaDllMgr.h"
#include "LuaEditorParamMgr.h"
#include "AdeFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------
static void cbTrace(LPVOID lpParam, unsigned long ulTraceLevel, char* szTrace, CAdeTrace::T_CONSOLE_COLOR cslColor)
{
    COLORREF color; // noir par defaut

    // On n'affiche que les erreurs
    if (ulTraceLevel != ADE_TRACE_LEVEL_ERROR)
    {
        return;
    }

    CMainFrame *pMainFrame = (CMainFrame*) lpParam;

    // on convertit les couleurs ici
    switch (cslColor)
    {
    // conversion des couleurs pour un affichage sur fond blanc
    case CAdeTrace::CONSOLE_COLOR_BLUE  : color = RGB(0,0,255); break;
    case CAdeTrace::CONSOLE_COLOR_GREEN : color = RGB(0,255,0); break;
    case CAdeTrace::CONSOLE_COLOR_CYAN  : color = RGB(0,255,255); break;
    case CAdeTrace::CONSOLE_COLOR_RED   : color = RGB(255,0,0); break;
    case CAdeTrace::CONSOLE_COLOR_PURPLE: color = RGB(0,255,0); break;
    case CAdeTrace::CONSOLE_COLOR_YELLOW: color = RGB(0,0,255); break;
    case CAdeTrace::CONSOLE_COLOR_WHITE : color = RGB(0,0,0); break;
    default: color = RGB(0,0,0);
    }
    pMainFrame->SendToTraceWindow(szTrace, CTraceBar::CONSOLE_TRACE, color);
}


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()

	ON_COMMAND(ID_VIEW_TRACE, OnViewTrace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACE, OnUpdateViewTrace)

    ON_UPDATE_COMMAND_UI(ID_NEW_LUASCRIPT,             OnUpdateCmdNewLUAScript)
    ON_UPDATE_COMMAND_UI(ID_OPEN_LUASCRIPT,            OnUpdateCmdOpenLUAScript)
//    ON_UPDATE_COMMAND_UI_RANGE(ID_LANGUAGE_FIRST, ID_LANGUAGE_LAST, OnUpdateLanguage)

    ON_COMMAND(ID_NEW_LUASCRIPT,             OnCmdNewLUAScript)
    ON_COMMAND(ID_OPEN_LUASCRIPT,            OnCmdOpenLUAScript)
END_MESSAGE_MAP()

static UINT BASED_CODE buttonsAdministrator[] =
{
    ID_NEW_LUASCRIPT,
    ID_OPEN_LUASCRIPT,
        ID_SEPARATOR,
    ID_VIEW_TRACE,
};

static UINT indicators[] =
{
    ID_SEPARATOR,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// construction ou destruction de CMainFrame

CMainFrame::CMainFrame()
{
    m_bAllowManageTrace = false;
}

CMainFrame::~CMainFrame()
{
}

//////////////////
// You must override PreTranslateMessage to pass input to drag-drop manager.
//
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
#if 0
    return m_ddm.PreTranslateMessage(pMsg) ? TRUE :
        CMDIFrameWndEx::PreTranslateMessage(pMsg);
#else
    return CMDIFrameWndEx::PreTranslateMessage(pMsg);
#endif
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

#if 0
    VERIFY(m_customizeManager.InitializeSplashWindow(IDB_SPLASHADEVIEW));
    COXSplashWnd *pSplashWindow = new COXSplashWnd;
    // se detruira toute seule
    pSplashWindow->SetAutoDelete(true);
    pSplashWindow->LoadBitmap(IDB_SPLASHADEVIEW, CLR_NONE);
#endif

    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;
        
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
//    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // active les onglets pour les fenêtres MDI
    EnableMDITabs( TRUE                         // Enable les onglets
                 , TRUE                         // Affiche les icones dans les onglest
                 , CMFCTabCtrl::LOCATION_TOP    // Onglets en haut
                 , TRUE                         // Bouton close affiché
                 , CMFCTabCtrl::STYLE_3D_VS2005 // Style des onglets
                 , FALSE                        // infos bulles personnalisées non affichées
                 , TRUE                         // Bouton close affiché sur onglet actif
                 );

    
    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators, NB_ELEMS_OF(indicators)))
    {
        TRACE0("Impossible de créer la barre d'état\n");
        return -1;      // échec de la création
    }

    // recuperation des infos du module courant
    CVersion versionInfos(NULL);

    // Initialisation du module de trace afin de lui passer le nom de l'applicatif et la callback
    CAdeTrace::InitSingleInstance( versionInfos.GetComments().c_str()
                                 , versionInfos.GetProductName().c_str()
                                 , versionInfos.GetProductVersion().c_str()
                                 , versionInfos.GetLegalCopyright().c_str()
                                 , versionInfos.GetCompanyName().c_str()
                                 , NULL
                                 , cbTrace
                                 , this
                                 , 0            // pour ne pas avoir de console
                                 , 0x3          // Par defaut, on ne trace que les messages d'erreurs et les warnings
                                 );

    // Obtention du nom du repertoire courant
    char currDir[260];
    GetCurrentDirectory(sizeof(currDir), currDir);
    if (m_ConfigFileName.size() == 0)
    {
        // le nom du fichier de symbole n'a pas ete prepositionne (par exemple par parsing de 
        // la ligne de commande
        if (versionInfos.GetComments() == std::string("OngTV"))
        {
            // version generique => pas de dialogue de login mais un dialogue de choix du fichier
            // CNX
            // on cherche le fichier a ouvrir
            m_bIsGeneric = true;

            CFileDialog SelectFile( TRUE, "cnx", NULL, OFN_FILEMUSTEXIST, theApp.LoadString(STR_CNX_FILE_FILTER));
            // On recupere le path du dernier fichier cnx qui a ete ouvert et on s'en sert comme repertoire
            // initial
            m_ConfigFilePath = theApp.GetKeyInRegistry(REGISTRY_KEY_CNX_PATH);
            if (0 != m_ConfigFilePath.size())
            {
                SelectFile.m_ofn.lpstrInitialDir = m_ConfigFilePath.c_str();
            }

            if (SelectFile.DoModal() != IDOK) 
            {
                // par defaut Adeview.cnx dans le repertoire courant
                m_ConfigFileName = versionInfos.GetComments() + ".cnx";
            }
            else
            {
                m_ConfigFileName = LPCSTR(SelectFile.GetPathName());
            }
        }
        else
        {
            // version specifique client => dialogue de login
            // et nom du fichier connexion = contenu de strComments + ".cnx"
            m_ConfigFileName = versionInfos.GetComments() + ".cnx";
        }
    }

    // extraction du path du fichier de configuration
    m_ConfigFilePath = CPathMgr::ExtractPath(m_ConfigFileName);
    if (m_ConfigFilePath.size() == 0)
    {
        // pas de path trouve => on met le path courant
        m_ConfigFilePath = currDir;
        m_ConfigFileName = m_ConfigFilePath + std::string("\\") + m_ConfigFileName;
    }
    // sauvegarde du path du fichier de config dans la base de registre
    theApp.SetKeyInRegistry(REGISTRY_KEY_CNX_PATH, m_ConfigFilePath.c_str());

    // chargement des parametres du gestionnaire de path (association suffixe fichier/repertoire)
    CPathMgr::ReadSingleInstance()->LoadLocalParameters(m_ConfigFileName);

    // Recuperation du nom du projet
    char szProject[30];
    GetPrivateProfileString( GENERAL_SECTION
                           , KEY_PROJECT
                           , ""
                           , &szProject[0]
                           , sizeof(szProject)
                           , m_ConfigFileName.c_str()
                           );
    m_strProject = szProject;
    if (m_strProject != "*")
    {
        // On change le titre de la fenetre
        char szTitle[100];
        GetWindowText(szTitle, sizeof(szTitle));
        std::string szNewTitle = szTitle;
        szNewTitle += " - ";
        szNewTitle += szProject;
        SetTitle(szNewTitle.c_str());
        SetWindowText(szNewTitle.c_str());
    }
    // Positionnement des Alias
    // Path du fichier de connexion
    CPathMgr::ReadSingleInstance()->SetAlias(ALIAS_CNXPATH, m_ConfigFilePath);
    // Path de l'executable
    CPathMgr::ReadSingleInstance()->SetAlias(ALIAS_EXEPATH, CPathMgr::ExtractPath(versionInfos.GetFileFullName()));

    std::string strRegistryBase = "Workspace";
    if (m_strProject != "*")
    {
        strRegistryBase = m_strProject + strRegistryBase;
    }

    strRegistryBase += "Administrator";
    // Creation du menu et de la toolbar
    CreateMenuAndToolbar(IDR_MAINFRAME, buttonsAdministrator, NB_ELEMS_OF(buttonsAdministrator));
    // pour faire la sauvegarde du workspace selon le projet et le niveau d'utilisateur
    theApp.SetRegistryBase(strRegistryBase.c_str());

	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

    // On charge les DLL Lua
    CLuaDllMgr::ReadSingleInstance()->LoadDllsFromConfigFile(m_ConfigFileName);

        // creation de la fenetre de trace
    if (theApp.m_pNewLuaDocTemplate != NULL)
    {
        // Si on est au niveau administrateur et que le template LUA a pu etre cree
        // alors on creera l'onglet de trace LUA
        m_TraceBar.SetLuaWindowTraceToCreate(true);
        // on charge egalement les parametres d'autocompletion pour l'editeur LUA
        CLuaEditorParamMgr::ReadSingleInstance()->LoadLocalParameters(m_ConfigFileName);
    }

    // Creation de la fenetre trace
    CString strTraceBar;
    BOOL bNameValid = strTraceBar.LoadString(STR_TRC_TITLE);
    ASSERT(bNameValid);
    if (!m_TraceBar.Create(strTraceBar, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_TRACE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create Trace window\n");
        return -1; // failed to create
    }

    m_TraceBar.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_TraceBar);

    // On regarde s'il faut lancer un script LUA sur le open de l'application
    char szLuaScriptOnOpen[255];
    // recuperation du nom du script (nom simple sans path)
    GetPrivateProfileString( LUA_AUTORUN_SECTION
                           , KEY_LUA_AUTORUN_ON_OPEN
                           , ""
                           , &szLuaScriptOnOpen[0]
                           , sizeof(szLuaScriptOnOpen)
                           , m_ConfigFileName.c_str()
                           );
    std::string strLuaScriptOnOpen = szLuaScriptOnOpen;
    if ((strLuaScriptOnOpen.size() != 0) && (m_LuaInterpreter.IsRunning() == false))
    {
        // Un script a ete indique et l'interpreteur n'est pas deja en fonctionnement
        // On rectifie son nom
        CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&strLuaScriptOnOpen);
        // s'il ne contient qu'un simple nom de fichier sans path, on considere qu'il faut aller le chercher
        // dans le repertoire par defaut des scripts LUA
        if (strLuaScriptOnOpen.find("\\") == std::string::npos)
        {
            strLuaScriptOnOpen = m_ConfigFilePath + "\\LuaScripts\\" + strLuaScriptOnOpen;
        }
        // on lance l'interpreteur
        m_LuaInterpreter.StartFromFile(strLuaScriptOnOpen);
        // et on attend qu'il soit fini
        m_LuaInterpreter.WaitTerminate();
    }
    m_bAllowManageTrace = true;

    return 0;
}

void CMainFrame::CreateMenuAndToolbar(UINT nIDMenuAndToolbarIcons, UINT *pButtons, int nButtons)
{
    // creation du menu
    CMenu NewMenu;
    NewMenu.LoadMenu(nIDMenuAndToolbarIcons);

    // creation de la toolbar correspondant au niveau d'utilisateur
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
       || !m_wndToolBar.LoadBitmap(nIDMenuAndToolbarIcons)
       || !m_wndToolBar.SetButtons(pButtons, nButtons)
       )
    {
        TRACE0("Impossible de créer toolbar\n");
    }
   
    // Affichage du menu
    m_wndMenuBar.Create(this);
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
    m_wndMenuBar.CreateFromMenu(NewMenu.GetSafeHmenu(), TRUE, TRUE);
    NewMenu.Detach();
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWndEx::PreCreateWindow(cs) )
        return FALSE;
    // TODO : changez ici la classe ou les styles Window en modifiant
    //  CREATESTRUCT cs

    return TRUE;
}


// diagnostics pour CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWndEx::Dump(dc);
}

#endif //_DEBUG

// gestionnaires de messages pour CMainFrame



//---------------------------------------------------------CMainFrame::SendToTraceWindow
//
// Description    :    Envoi sur la fenetre de trace du message passe par la callback
//                  (appele par le manager de trace)
//
// Parametres    :    Aucun
//
// Retour        :    true
//-----------------------------------------------------------------------------
void CMainFrame::SendToTraceWindow(char* szTrace, CTraceBar::T_TYPE_TRACE typeTrace, COLORREF color /*= RGB(0,0,0)*/)
{
    m_TraceBar.AddToTraceWindow(szTrace, typeTrace, color);
}

//---------------------------------------------------------CMainFrame::ClearTrace
//
// Description    :  efface la fenetre trace
//
// Parametres    :    Aucun
//
// Retour        :    true
//-----------------------------------------------------------------------------
void CMainFrame::ClearTrace(CTraceBar::T_TYPE_TRACE typeTrace)
{
    // Petite astuce pour ne pas appeler ClearTrace lorsqu'on execute le script LUA lors de l'open
    // car sinon on reste bloque dans le thread de l'interpreteur LUA
    if (m_bAllowManageTrace)
    {
        m_TraceBar.ClearTraceWindow(typeTrace);
    }
}

//---------------------------------------------------------CMainFrame::ShowTrace
//
// Description    :  montre la fenetre trace
//
// Parametres    :    Aucun
//
// Retour        :    true
//-----------------------------------------------------------------------------
void CMainFrame::ShowTrace(CTraceBar::T_TYPE_TRACE typeTrace)
{
    // Petite astuce pour ne pas appeler ShowTraceWindow lorsqu'on execute le script LUA lors de l'open
    // car sinon on reste bloque dans le thread de l'interpreteur LUA
    if (m_bAllowManageTrace)
    {
        m_TraceBar.ShowTraceWindow(typeTrace);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnClose() 
{
    // TODO: Add your message handler code here and/or call default
    CPathMgr::DeleteSingleInstance();
    CLuaEditorParamMgr::DeleteSingleInstance();
    CLuaDllMgr::DeleteSingleInstance();
    CDllMgr::DeleteSingleInstance();
    CLuaInterpreterMgr::DeleteSingleInstance();
    CAdeTrace::DeleteSingleInstance();

    CMDIFrameWndEx::OnClose();
}

#if 0
void CMainFrame::OnViewCustomize() 
{
}



void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // TODO: Add your message handler code here
    UNREFERENCED_PARAMETER(pWnd);
}


void CMainFrame::OnShowControlBars(UINT nID)
{
}

#endif
//----------------------------------------------------CMainFrame::OnCmdNewLUAScript
//
// Description    :    Fonction liee a la commande d'ouverture d'un
//                     nouveau script LUA
//
// Parametres    :    Aucun
//
// Retour        :    Aucun
//-----------------------------------------------------------------------------
void CMainFrame::OnCmdNewLUAScript()
{
    // on ouvre une fenetre script LUA
    theApp.m_pNewLuaDocTemplate->m_hMenuShared=NULL;
    theApp.m_pNewLuaDocTemplate->OpenDocumentFile(NULL);
}
//----------------------------------------------------CMainFrame::OnCmdOpenLUAScript
//
// Description    :    Fonction liee a la commande d'ouverture d'un
//                     script LUA
//
// Parametres    :    Aucun
//
// Retour        :    Aucun
//-----------------------------------------------------------------------------
void CMainFrame::OnCmdOpenLUAScript()
{
    // dialogue de choix du fichier a charger
    std::string file = theApp.SelectFile(TRUE, theApp.m_pNewLuaDocTemplate);
    if (0 == file.size()) 
    {
        return;
    }
    // on ouvre une fenetre dashboard
    theApp.m_pNewLuaDocTemplate->m_hMenuShared=NULL;
    theApp.m_pNewLuaDocTemplate->OpenDocumentFile(file.c_str());
}


void CMainFrame::OnUpdateCmdNewLUAScript(CCmdUI* pCmdUI)
{
    if (theApp.m_pNewLuaDocTemplate != NULL)
    {
        pCmdUI->Enable(1);
    }
    else
    {
        pCmdUI->Enable(0);
    }
}
void CMainFrame::OnUpdateCmdOpenLUAScript(CCmdUI* pCmdUI)
{
    if (theApp.m_pNewLuaDocTemplate != NULL)
    {
        pCmdUI->Enable(1);
    }
    else
    {
        pCmdUI->Enable(0);
    }
}
//***********************************************************************
// Function: CDynaMenuDoc::OnCmdMsg()
//
// Purpose:
//    OnCmdMsg() is called by the framework to route and dispatch
//    command messages and to handle the update of command
//    user-interface objects.
//
//    Here we extend the standard command routing to intercept command
//    messages with variable command IDs.
//    If one is found, we process the message.  Otherwise,
//    we route the command to the standard OnCmdMsg processing.
//
// Parameters:
//    nID           -- contains the command ID
//    nCode         -- identifies the command notification code
//    pExtra        -- used according to the value of nCode
//    pHandlerInfo  -- if not NULL, filled in with pTarget and pmf
//                     members of CMDHANDLERINFO structure, instead
//                     of dispatching the command.  Typically NULL.
//
// Returns:
//    nonzero if message handled, otherwise 0.
//
// Comments:
//    See the documentation for CCmdTarget::OnCmdMsg() for further
//    information.  Command routing is also discussed in tech note #21.
//
//***********************************************************************
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra,
                            AFX_CMDHANDLERINFO* pHandlerInfo)
{
    static char szFunc[] = "CMainFrame::OnCmdMsg";
    if ((ID_USER_CMD_BASE <= nID) && (nID <= (ID_USER_CMD_BASE+m_arrUserCmds.size()-1)))
    {
        int cmdIdx = nID - ID_USER_CMD_BASE;
        // il s'agit d'une commande utilisateur
        if (nCode == CN_COMMAND)
        {
            // Handle WM_COMMAND message
            if (m_LuaInterpreter.IsRunning())
            {
                // pas possible, un script est deja en cours d'execution
                return TRUE;
            }
            // on lance l'interpreter
            m_arrUserCmds[cmdIdx].itp.StartFromFile(m_arrUserCmds[cmdIdx].luaScripFileName);
        }
        else if (nCode == CN_UPDATE_COMMAND_UI)
        {
            // Update UI element state
            if (m_arrUserCmds[cmdIdx].itp.IsRunning())
            {
                // La commande est en cours d'exécution
                ((CCmdUI*)pExtra)->Enable(0);
            }
            else
            {
                ((CCmdUI*)pExtra)->Enable(1);
            }
        }
        return TRUE;
    }

    // If we didn't process the command, call the base class
    // version of OnCmdMsg so the message-map can handle the message
    return CMDIFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
    if ((ID_USER_CMD_BASE <= nID) && (nID <= (ID_USER_CMD_BASE+m_arrUserCmds.size()-1)))
    {
        // il s'agit d'une commande utilisateur
        rMessage = m_arrUserCmds[nID-ID_USER_CMD_BASE].strDesc.c_str();
    }
    else
    {
        CFrameWnd::GetMessageString(nID, rMessage);
    }
}

void CMainFrame::OnViewTrace() 
{
    if (NULL != m_TraceBar.GetSafeHwnd())
    {
        ShowPane(&m_TraceBar, !(m_TraceBar.IsVisible ()), FALSE, TRUE);
        RecalcLayout();
    }
}
void CMainFrame::OnUpdateViewTrace(CCmdUI* pCmdUI) 
{
    if (NULL != m_TraceBar.GetSafeHwnd())
    {
        pCmdUI->SetCheck(m_TraceBar.IsVisible ());
        pCmdUI->Enable(1);
    }
    else
    {
        pCmdUI->Enable(0);
    }
}
