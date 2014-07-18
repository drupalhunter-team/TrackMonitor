//////////////////////  //////////////////////////////////////////////////////////
/// @addtogroup DLLMGR
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe du gestionnaire des DLLs contenant les
/// fonctions de transfert
///
/// \n \n <b>Copyright ADENEO 2008</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/DllMgr.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                    / Librairie)
#include "stdafx.h"

#include "DllMgr.h"
#include "Utils.h"
#include "PathMgr.h"

////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                     Variables
////////////////////////////////////////////////////////////////////////////////
// Note : OBLIGATOIREMENT static


////////////////////////////////////////////////////////////////////////////////
//                                             Prototypes des fonctions internes
////////////////////////////////////////////////////////////////////////////////
// Note : OBLIGATOIREMENT static

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Obtention de l'instance unique du gestionnaire de path
///
/// \return : pointeur sur le gestionnaire
////////////////////////////////////////////////////////////////////////////////
CDllMgr* CDllMgr::pSingleInstance = NULL;
CDllMgr* CDllMgr::ReadSingleInstance(void)
{
    if (pSingleInstance == NULL)
        pSingleInstance = new(CDllMgr);

    return pSingleInstance;
}
////////////////////////////////////////////////////////////////////////////////
/// Destruction du gestionnaire de services
////////////////////////////////////////////////////////////////////////////////
void CDllMgr::DeleteSingleInstance(void)
{
    if (pSingleInstance != NULL)
    {
        delete pSingleInstance;
        pSingleInstance = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Constructeur
////////////////////////////////////////////////////////////////////////////////
CDllMgr::CDllMgr() 
{
}
////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CDllMgr::~CDllMgr()
{
    Garbage();
}

////////////////////////////////////////////////////////////////////////////////
/// Liberation des ressources de l'objet
////////////////////////////////////////////////////////////////////////////////
void CDllMgr::Garbage (void)
{
    // Liberation des DLLs qui ont ete chargees
    T_NAME2HMODULE::iterator iter;
    for (iter = m_DicoDll.begin() ; iter != m_DicoDll.end() ; ++iter)
    {
        FreeLibrary(iter->second);
    }
    // Vidage du dictionnaire
    m_DicoDll.clear();
    // Restauration du chemin de recherche par defaut utilises par Windows pour la recherche des DLLs
    SetDllDirectory(NULL);
}

////////////////////////////////////////////////////////////////////////////////
/// Retourne l'adresse d'une fonction dans une DLL
///
/// Parametres :
/// \param strDllName   nom de la DLL censee contenir la fonction
/// \param strFctName   nom de la fonction
///
/// \return pointeur sur la fonction
///         NULL si elle n'a pas ete trouvee
////////////////////////////////////////////////////////////////////////////////
// retourne le pointeur de fonction sur la fonction de transfert
FARPROC CDllMgr::GetFctAddr(const std::string& strDllName, const std::string& strFctName)
{
    char szFunc[] = "CDllMgr::GetFctAddr";
    
    HMODULE hDll = GetHandle(strDllName);
    
    if (NULL == hDll)
    {
        // DLL pas chargee
        return NULL;
    }
    // On cherche la fonction dans la DLL
    FARPROC pFct = NULL;
    pFct = GetProcAddress(hDll, strFctName.c_str());
    if (pFct == NULL)
    {
        ADE_ERROR("Unable to find function " << strFctName << " in Dll " << strDllName);
    }
    return pFct;
}

////////////////////////////////////////////////////////////////////////////////
/// retourne le nom complet de la DLL (comprenant le path de l'endroit ou elle a ete trouvee)
/// ne marche que si la DLL a ete correctement chargee
///
/// Parametres :
/// \param strDllName   nom de la DLL
///
/// \return string contenant le nom complet
///         vide si elle n'a pas ete trouvee
////////////////////////////////////////////////////////////////////////////////
// retourne le pointeur de fonction sur la fonction de transfert
std::string CDllMgr::GetFullName(const std::string& strDllName)
{
    HMODULE hDll = GetHandle(strDllName);
    std::string strDllFullName;
    if (NULL != hDll)
    {
        // recuperation des infos de version de la DLL
        char lpDllname[255];
        memset(lpDllname,0,255);
        GetModuleFileName(hDll ,lpDllname, 255);
        strDllFullName = lpDllname;
    }
    return strDllFullName;
}

////////////////////////////////////////////////////////////////////////////////
/// Retourne le handle d'une DLL
///
/// Parametres :
/// \param strDllName   nom de la DLL
///
/// \return HANDLE de la DLL
///         NULL si elle n'a pas ete trouvee
////////////////////////////////////////////////////////////////////////////////
HMODULE CDllMgr::GetHandle(const std::string& strDllName)
{
    char szFunc[] = "CDllMgr::GetHandle";
    HMODULE hDll;
    std::string dllName = strDllName;

    // On y remplace un eventuel ALIAS (obligatoirement avant le ToUpper)
    CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&dllName);

    // On regarde si la chaine se termine bien par .DLL
    ToUpper(&dllName);
    if (dllName.rfind(".DLL") == std::string::npos)
    {
        // On rajoute .dll
        dllName += ".DLL";
    }

    // On commence par rechercher la Dll dans le dico
    T_NAME2HMODULE::iterator iter;
    iter = m_DicoDll.find(dllName);
    if (iter == m_DicoDll.end())
    {
        // La DLL n'a pas deja ete chargee, on la charge
        hDll = LoadLibrary(dllName.c_str());
        if (hDll == NULL)
        {
            ADE_ERROR("Unable to load Dll " << dllName);
            return NULL;
        }
        // OK, DLL bien chargee, on l'ajoute au module
        m_DicoDll[dllName] = hDll;
    }
    else
    {
        hDll = iter->second;
    }
    return hDll;
}

////////////////////////////////////////////////////////////////////////////////
/// Ajout d'un path additionnel pour le chargement des DLLs
///
/// Parametres :
/// \param strDllPath   path additionnel
///
////////////////////////////////////////////////////////////////////////////////
// retourne le pointeur de fonction sur la fonction de transfert
void CDllMgr::AddDllPath(const std::string& strDllPath)
{
     std::string dllPath = strDllPath;

   // On y remplace un eventuel ALIAS
    CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&dllPath);

    // Ajout du path au chemins de recherche utilises par Windows
    SetDllDirectory(dllPath.c_str());
}


////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/DllMgr.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
