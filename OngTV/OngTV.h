// OngTV.h : fichier d'en-tête principal pour l'application OngTV
//
#pragma once

#ifndef __AFXWIN_H__
    #error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include <afxwinappex.h>
#include <afxole.h>
#include "resource.h"       // symboles principaux
#include "version.h"


// clefs dans la base de registre
#define REGISTRY_KEY_VERSION  _T("Version")
#define REGISTRY_KEY_CNX_PATH _T("CnxPath")


class COngTVApp : public CWinAppEx
{
NON_COPYABLE(COngTVApp);
public:
    COngTVApp();


// Substitutions
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    // Info de version de l'application
    CVersion m_versionInfos;

// Implémentation
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()

public:
    // Chargement d'une ressource string
    CString LoadString(UINT uID);

    // On recupere la valeur d'une clef dans la base de registre
    std::string GetKeyInRegistry(const char* key);
    // On ecrit la valeur d'une clef en base de registre
    void SetKeyInRegistry(const char* key, const char* value);

    // Methodes encapsulant l'utilisation de CFileDialog pour gerer les répertoires initiaux
    // On sauvegarde (classe CPathMgr), les associations <répertoire>/<suffixe de fichier> lors de chaque demande d'un nom de fichier
    std::string SelectFile( BOOL bOpenFileDialog
                          , LPCTSTR lpszDefExt
                          , LPCTSTR lpszFilter
                          , LPCTSTR lpszFileName = NULL
                          , LPCTSTR lpszTitle = NULL
                          , DWORD dwFlags = 0
                          );
    std::string SelectFile( BOOL bOpenFileDialog
                          , CMultiDocTemplate* pDocTemplate
                          , LPCTSTR lpszFileName = NULL
                          , DWORD dwFlags = 0
                          );

    // renvoie le prefixe a utiliser pour le nom d'un fichier temporaire
    const std::string GetTemporaryFileNamePrefix(void) { return std::string("OngTV"); };
private:
public:
    CMultiDocTemplate* m_pNewLuaDocTemplate;        // Doc template pour script LUA

    HANDLE m_hDinaFont;

#ifdef SCINTILLA_DLL
    HINSTANCE m_hSciDLL;
#endif

};

extern COngTVApp theApp;