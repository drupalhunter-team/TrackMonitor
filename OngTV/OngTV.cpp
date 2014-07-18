// OngTV.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include <afxtooltipctrl.h>
#include <afxtooltipmanager.h>
#include "afxwin.h"
#include "OngTV.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "OngTVDoc.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "LuaChildFrm.h"
#include "OXRegistryItem.h"
#include "Version.h"
#include "CmdLine.h"
#include "PPHtmlStatic.h"
#include "PathMgr.h"
#include "Version.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const TCHAR szFormatVersion[] =
    _T("<center><big><b>%s version %s<br>%s<br></b></big></center>");

static const TCHAR szUsedComponents[] =
    _T("<big>This software uses the following free libraries and components:<br><br></big><table border=\"1\" bordercolor=\"slategray\">")

    _T("<tr><td><b>Some Boost libraries</b></td><td><a href=\"http://www.boost.org/\"><i>www.boost.org</i></a></td></tr>")

    _T("<tr><td><b>Ultimate Toolbox</b></td><td><a href=\"http://www.codeproject.com/KB/MFC/UltimateToolbox.aspx\"><i>Extended MFC UI Controls</i></a></td></tr>")

    _T("<tr><td><b>CWndResizer</b> by Mizan Rahman</td>")
	_T("<td><a href=\"http://www.codeproject.com/Articles/125068/MFC-C-Helper-Class-for-Window-Resizing\"><i>A MFC/C++ Helper Class for Window Resizing</i></a></td></tr>")

    _T("<tr><td><b>Universal Progress Dialog</b> by Gopalakrishna Palem</td>")
	_T("<td><a href=\"http://www.codeproject.com/KB/dialog/UPDialog.aspx\"><i>A generic progress bar dialog</i></a></td></tr>")

    _T("<tr><td><b>CPPHtmlStatic</b> by Eugene Pustovoyt</td>")
	_T("<td><a href=\"http://www.codeproject.com/KB/static/pphtmlstatic.aspx\"><i>A control based on CStatic for displaying HTML-like text formatting elements</i></a></td></tr>")

    _T("<tr><td><b>Scintilla</b></td><td>")
	_T("<td><a href=\"http://www.scintilla.org/\"><i>A free source code editing component</i></a></td></tr>")

    _T("<tr><td><b>CScintillaCtrl, CScintillaView, CScintillaDoc</b> by PJ Naughter</td>")
	_T("<td><a href=\"http://www.naughter.com/scintilla.html\"><i>MFC classes to encapsulate the Scintilla edit control</i></a></td></tr>")

    _T("<tr><td><b>LUA</b> by Roberto Ierusalimschy</td>")
	_T("<td><a href=\"http://www.lua.org/\"><i>A scripting language</i></a></td></tr>")

    _T("<tr><td><b>toLua</b> by Waldemar Celes</td>")
	_T("<td><a href=\"http://www.tecgraf.puc-rio.br/~celes/tolua/\"><i>Support code for Lua bindings</i></a></td></tr>")

    _T("<tr><td><b>Lua BitOp</b> by Mike Pall</td>")
	_T("<td><a href=\"http://bitop.luajit.org/\"><i>a C extension module for Lua which adds bitwise operations on numbers</i></a></td></tr>")

    _T("<tr><td><b>TinyXML</b> by Lee Thomason</td>")
	_T("<td><a href=\"http://www.grinninglizard.com/tinyxml/\"><i>A simple, small, C++ XML parser</i></a></td></tr>")

    _T("<tr><td><b>CCmdLine</b> by Chris Losinger</td>")
	_T("<td><a href=\"http://www.codeproject.com/KB/recipes/ccmdline.aspx\"><i>A command line parser</i></a></td></tr>")

    _T("<tr><td><b>Sha2</b> by Aaron D. Gifford</td>")
	_T("<td><a href=\"http://www.aarongifford.com/computers/sha.html\"><i>A Secure Hash Algorithm (SHA) implementation</i></a></td></tr>")

    _T("<tr><td><b>UPX</b> by Markus Oberhumer, Laszlo Molnar & John Reiser</td>")
	_T("<td><a href=\"http://upx.sourceforge.net\"><i>the Ultimate Packer for eXecutables</i></a></td></tr>")

    _T("<tr><td><b>C/Invoke</b> by Will Weisser</td>")
	_T("<td><a href=\"http://www.nongnu.org/cinvoke/index.html\"><i>A library for connecting to C libraries at runtime</i></a></td></tr></table>");


// COngTVApp

BEGIN_MESSAGE_MAP(COngTVApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &COngTVApp::OnAppAbout)
END_MESSAGE_MAP()


// construction COngTVApp

COngTVApp::COngTVApp() : m_versionInfos(NULL)
{
    // TODO : ajoutez ici du code de construction,
    // Placez toutes les initialisations significatives dans InitInstance
    m_pNewLuaDocTemplate = NULL;
}


// Seul et unique objet COngTVApp

COngTVApp theApp;
COXRegistryItem g_Registry;

// destruction d'un arbre de clefs de registre
void DeleteRegTree(COXRegistryItem* const pReg)
{
    for (;;)
    {
        CString sSubKey = pReg->EnumerateSubkey(0);
        if (!sSubKey.IsEmpty())
        {
            sSubKey = pReg->GetFullRegistryItem() + sSubKey +_T("\\");
            COXRegistryItem reg;
            reg.SetFullRegistryItem((LPCTSTR) sSubKey);
            DeleteRegTree(&reg);
        }
        else
            break;
    }
    pReg->Delete();
    pReg->Close();
}


// initialisation de COngTVApp

BOOL COngTVApp::InitInstance()
{
    // customisation des barres d'outils non autorisée
    CMFCToolBar::SetCustomizeMode(FALSE);
#ifdef SCINTILLA_DLL
    // Chargement de la DLL Scintilla : on va la chercher dans le repertoire de l'exe et nul part ailleurs
    // recuperation du path de l'exe
    char lpFilename[255];
    memset(lpFilename,0,255);
    GetModuleFileName(NULL ,lpFilename, 255);
    std::string strSciLexerDllName = CPathMgr::ExtractPath(lpFilename);
    strSciLexerDllName = strSciLexerDllName + "\\SciLexer.dll";
    m_hSciDLL = LoadLibrary(strSciLexerDllName.c_str());
    if (NULL == m_hSciDLL)
    {
        // DLL scintilla non disponible => on ne pourra pas editer de scrip LUA
    }
    else
    {
        CVersion versionSciDLL(strSciLexerDllName);
        if (versionSciDLL.GetProductVersion() < std::string(EXPECTED_SCINTILLA_VERSION))
        {
            // version de la DLL inferieur a la version attendue => on ne pourra pas editer de scrip LUA
            FreeLibrary(m_hSciDLL);
            m_hSciDLL = NULL;
            std::string strMsgError = "Dll min expected version : "EXPECTED_SCINTILLA_VERSION" obtained : ";
            strMsgError += versionSciDLL.GetProductVersion();
            MessageBox(NULL, strMsgError.c_str(), "Unable to load SciLexer.dll", MB_ICONEXCLAMATION);
        }
    }
#endif

#ifdef TEST_ENGLISH
    // Pour forcer une langue
    ::SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH,
                       SUBLANG_DEFAULT),SORT_DEFAULT));
#endif

    // nettoyage des clefs de registres de la version precedente
    // si la version qui s'execute n'est pas identique a celle enregistree dans la base de registres
    // MFCFP = MFC Feature Pack => pour pouvoir faire cohabiter sur un même PC des versions antérieures à la 1.6.0 et des versions postérieures
    // a la 1.6.0
    // avant la 1.6.0 => IHM basée sur UltimateToolbox et sauvegarde des caractéristiques des fenêtres à plat dans la base de registre
    // à partir de 1.6.0 => IHM basée sur MFC Feature Pack et sauvegarde des caractéristiques des fenêtres dans une arborescence de clefs
    // dans la base de registres
    // le nettoyage de la base de registres plantait si on essaiyait de revenir à une version < 1.6.0 => on différencie la clef de base

    std::string strReg = "\\CurrentUser\\Software\\" + m_versionInfos.GetCompanyName() + "\\" + m_versionInfos.GetProductName() + "MFCFP\\";
    g_Registry.SetFullRegistryItem(strReg.c_str());
    CString sVersion=g_Registry.GetStringValue(REGISTRY_KEY_VERSION);
    CString sProductVersion(m_versionInfos.GetProductVersion().c_str());
    if (sVersion != sProductVersion)
    {
        DeleteRegTree(&g_Registry);
    }
    g_Registry.Close();

    // Pour utilisation de RichEditCtrl
    AfxEnableControlContainer();
    AfxInitRichEdit();
    // InitCommonControlsEx() est requis sur Windows XP si le manifeste de l'application
    // spécifie l'utilisation de ComCtl32.dll version 6 ou ultérieure pour activer les
    // styles visuels.  Dans le cas contraire, la création de fenêtres échouera.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // À définir pour inclure toutes les classes de contrôles communs à utiliser
    // dans votre application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    InitContextMenuManager();
    InitShellManager();
    InitKeyboardManager();
    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // enregistrement Scintilla
#ifndef SCINTILLA_DLL
    Scintilla_RegisterClasses(AfxGetInstanceHandle());
    Scintilla_LinkLexers();
#endif

// Supprime pour la gestion multi-lingues
//    CWinAppEx::InitInstance();


    // Initialisation standard
    // Si vous n'utilisez pas ces fonctionnalités et que vous souhaitez réduire la taille
    // de votre exécutable final, vous devez supprimer ci-dessous
    // les routines d'initialisation spécifiques dont vous n'avez pas besoin.
    // Changez la clé de Registre sous laquelle nos paramètres sont enregistrés
    // TODO : modifiez cette chaîne avec des informations appropriées,
    // telles que le nom de votre société ou organisation
    SetRegistryKey(m_versionInfos.GetCompanyName().c_str());

    //First free the string allocated by MFC at CWinApp startup.
    //The string is allocated before InitInstance is called.
    free((void*)m_pszProfileName);
    //Change the name of the .INI file.
    //The CWinApp destructor will free the memory.
    // Ce nom est utilise pour la sauvegarde des parametres
    strReg = m_versionInfos.GetProductName() + "MFCFP";
    m_pszProfileName = _tcsdup(strReg.c_str());

    LoadStdProfileSettings(4);  // Charge les options de fichier INI standard (y compris les derniers fichiers utilisés)
    // Inscrire les modèles de document de l'application. Ces modèles
    //  lient les documents, fenêtres frame et vues entre eux
    
    // creation du manager de doc specifique afin de gerer specifiquement les repertoires
    // de sauvegarde des diffents documents.
    // cf http://www.codeguru.com/cpp/w-d/dislog/commondialogs/article.php/c1967
    m_pDocManager = new COngTVDocManager;

    // Creation doc template pour les scripts LUA (uniquement si DLL scintilla disponible)
    if (NULL != m_hSciDLL)
    {
        m_pNewLuaDocTemplate = new CMultiDocTemplate( IDR_LUATYPE
                                                    , RUNTIME_CLASS(CLuaDoc)
                                                    , RUNTIME_CLASS(CLuaChildFrame)
                                                    , RUNTIME_CLASS(CLuaView)
                                                    );
        if (!m_pNewLuaDocTemplate)
            return FALSE;
        AddDocTemplate(m_pNewLuaDocTemplate);
    }

    // Installe la police DINA
    HRSRC hRes   = FindResource(NULL, MAKEINTRESOURCE(IDR_DINA), _T("DINA"));
    PVOID lpFont = LockResource(LoadResource(NULL, hRes)); 
    DWORD dwSize = SizeofResource(NULL, hRes), cFonts = 0;
    m_hDinaFont = AddFontMemResourceEx(lpFont, dwSize, NULL, &cFonts);
    ASSERT(cFonts > 0);

    // crée la fenêtre frame MDI principale
    CMainFrame* pMainFrame = new CMainFrame;
    // On parse la ligne de commande
    CCmdLine cmdLine;
    cmdLine.SplitLine(__argc, __argv);
    // On indique le fichier de configuration ("" par defaut)
    std::string configFileName = cmdLine.GetSafeArgument("-cnx", 0, "");
    pMainFrame->SetConfigFile(configFileName);

    m_pMainWnd = pMainFrame;
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pMainFrame;
        return FALSE;
    }

    // La fenêtre principale a été initialisée et peut donc être affichée et mise à jour
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    return TRUE;
}



// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialog
{
public:
    CAboutDlg(CVersion &Version);

// Données de boîte de dialogue
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

// Implémentation
protected:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    DECLARE_MESSAGE_MAP()
public:
    CPPHtmlStatic m_stVersion;
    CPPHtmlStatic m_stComponents;
    CVersion     &m_Version;
};

CAboutDlg::CAboutDlg(CVersion &Version) : CDialog(CAboutDlg::IDD), m_Version(Version)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ABOUT_VERSION, m_stVersion);
    DDX_Control(pDX, IDC_ABOUT_COMPONENTS, m_stComponents);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void CAboutDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    char szVersionText[1024];
    _snprintf_s( szVersionText, sizeof(szVersionText), _TRUNCATE
		       , szFormatVersion
               , m_Version.GetProductName().c_str()
               , m_Version.GetProductVersion().c_str()
               , m_Version.GetLegalCopyright().c_str()
               );

    m_stVersion.SetWindowText(szVersionText);    
    m_stComponents.SetWindowText(szUsedComponents);    
}
// Commande App pour exécuter la boîte de dialogue
void COngTVApp::OnAppAbout()
{
    CAboutDlg aboutDlg(m_versionInfos);
    aboutDlg.DoModal();
}


int COngTVApp::ExitInstance() 
{
    // ecriture de la version du produit dans la base de registre
    // (utilite : nettoyer la base de registre au lancement du produit si la version
    // du produit lance n'est pas identique a celle de la base de registre)
    SetKeyInRegistry(REGISTRY_KEY_VERSION, m_versionInfos.GetProductVersion().c_str());

    // Remove Dina Font
    VERIFY(RemoveFontMemResourceEx(m_hDinaFont));

    // dechargement DLL LUA
    if (NULL != m_hSciDLL)
    {
        FreeLibrary(m_hSciDLL);
    }

    return CWinApp::ExitInstance();
}

// Chargement d'une ressource string
CString COngTVApp::LoadString(UINT uID)
{
    CString str;
    str.LoadString(uID);
    return str;
}

// On recupere la valeur d'une clef dans la base de registre
std::string COngTVApp::GetKeyInRegistry(const char* key)
{
    std::string strReg = "\\CurrentUser\\Software\\" + m_versionInfos.GetCompanyName() + "\\" + m_versionInfos.GetProductName() + "MFCFP\\";
    g_Registry.SetFullRegistryItem(strReg.c_str());
    CString sValue = g_Registry.GetStringValue(key);
    g_Registry.Close();
    return LPCSTR(sValue);
}
// On ecrit la valeur d'une clef en base de registre
void COngTVApp::SetKeyInRegistry(const char* key, const char* value)
{
    std::string strReg = "\\CurrentUser\\Software\\" + m_versionInfos.GetCompanyName() + "\\" + m_versionInfos.GetProductName() + "MFCFP\\";
    g_Registry.SetFullRegistryItem(strReg.c_str());
    g_Registry.SetStringValue(value, key);
}
// Methode encapsulant l'utilisation de CFileDialog pour gerer les répertoires initiaux
// On sauvegarde (classe CPathMgr), les associations <répertoire>/<suffixe de fichier> lors de chaque demande d'un nom de fichier
std::string COngTVApp::SelectFile( BOOL bOpenFileDialog
                                   , LPCTSTR lpszDefExt
                                   , LPCTSTR lpszFilter
                                   , LPCTSTR lpszFileName /*= NULL*/
                                   , LPCTSTR lpszTitle /*= NULL*/
                                   , DWORD dwFlags /*= 0*/
                                   )
{
    if (bOpenFileDialog)
    {
        dwFlags |= OFN_FILEMUSTEXIST;
    }
    else
    {
        dwFlags |= OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
    }
    CFileDialog dlgFile(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter);
    std::string strSuffix(lpszDefExt);
    std::string strDirectory;
    if (0 != strSuffix.size())
    {
        // On recupere le repertoire associe a ce suffixe
        strDirectory = CPathMgr::ReadSingleInstance()->GetSuffixDirectory(strSuffix);
        if (0 != strDirectory.size())
        {
            // s'il n'est pas vide, on en fait le repertoire de depart
            dlgFile.m_ofn.lpstrInitialDir = strDirectory.c_str();
        }
        if (0 != strDirectory.size())
        {
            // s'il n'est pas vide, on en fait le repertoire de depart
            dlgFile.m_ofn.lpstrInitialDir = strDirectory.c_str();
        }
    } 
    dlgFile.m_ofn.lpstrTitle = lpszTitle;
    INT_PTR nResult = dlgFile.DoModal();
    
    // On modifie le repertoire associe au suffixe
    if (IDOK == nResult)
    {
        std::string fileName((LPCSTR)dlgFile.GetPathName());
        if (0 != strSuffix.size())
        {
            // on recupere le repertoire selectionne
            strDirectory = CPathMgr::ExtractPath(fileName);
            CPathMgr::ReadSingleInstance()->SetSuffixDirectory(strSuffix, strDirectory);
        }
        return fileName;
    }
    else
    {
        return "";
    }
}
// Methode encapsulant l'utilisation de CFileDialog pour gerer les répertoires initiaux
// On sauvegarde (classe CPathMgr), les associations <répertoire>/<suffixe de fichier> lors de chaque demande d'un nom de fichier
// On se sert de la methode du manager de doc specifique gerant ces associations COngTVDocManager
std::string COngTVApp::SelectFile( BOOL bOpenFileDialog
                                   , CMultiDocTemplate* pDocTemplate
                                   , LPCTSTR lpszFileName /*= NULL*/
                                   , DWORD dwFlags /*= 0*/
                                   )
{
    CString fileName(lpszFileName);
    if (TRUE == m_pDocManager->DoPromptFileName(fileName, 0, dwFlags, bOpenFileDialog, pDocTemplate))
    {
        return (LPCSTR)fileName;
    }
    else
    {
        return "";
    }
}

