////////////////////////////////////////////////////////////////////////////////
/// @addtogroup LUA_DLL_MGR
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implemente la classe du serveur Web (singleton CLuaDllMgr)
/// Cette classe charge les DLLs contenant des fonctions C appelables depuis
/// l'interpreteur LUA d'OngTV (les DLLs a charger sont declarees dans le fichier
/// de connexion).
///     ex : 
///          [LUA_DLL1]
///          NAME=$(CnxDir)\DLL_LUA_Exemple.dll
/// Chacune de ces DLLs doit contenir deux ou trois fonctions exportees
///     LUA_open = qui est appelle (indirectement par la methode CLuaDllMgr::openLibs
///                et qui permet de lier les fonctions a l'interpreteur LUA
///     LUA_getKeywords = qui est appelle (indirectement par la methode CLuaDllMgr::getKeywords
///                et qui permet de recuperer les mots clefs pour la coloration syntaxique
///     LUA_close = qui est appelle (indirectement par la methode CLuaDllMgr::closeLibs
///                et qui permet de fermer/libérer des ressources gérées par la DLL - cette fonction
///                peut ne pas etre présente dans la DLL
/// Un exemple de DLL est donne dans l'arborescence du projet
///
/// \n \n <b>Copyright ADENEO</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaDllMgr.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                     / Librairie)

#include "stdafx.h"

#include "LuaDllMgr.h"
#include "DllMgr.h"

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
/// Obtention de l'instance unique du gestionnaire de connexions
///
/// \return : pointeur sur le gestionnaire
////////////////////////////////////////////////////////////////////////////////
CLuaDllMgr* CLuaDllMgr::pSingleInstance = NULL;
CLuaDllMgr* CLuaDllMgr::ReadSingleInstance(void)
{
    if (pSingleInstance == NULL)
        pSingleInstance = new(CLuaDllMgr);

    return pSingleInstance;
}
////////////////////////////////////////////////////////////////////////////////
/// Destruction du gestionnaire de connexions
////////////////////////////////////////////////////////////////////////////////
void CLuaDllMgr::DeleteSingleInstance(void)
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
CLuaDllMgr::CLuaDllMgr()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CLuaDllMgr::~CLuaDllMgr()
{
    Garbage();
}

////////////////////////////////////////////////////////////////////////////////
/// Liberation des ressources de l'objet
////////////////////////////////////////////////////////////////////////////////
void CLuaDllMgr::Garbage (void)
{
}


////////////////////////////////////////////////////////////////////////////////
/// Chargement des DLLs d'extension LUA a partir des parametres du fichier .ini
///
/// Parametres :
/// \param configFileName     nom du fichier contenant la liste des DLLs a charger
///
/// \return true
////////////////////////////////////////////////////////////////////////////////
bool CLuaDllMgr::LoadDllsFromConfigFile(const std::string& configFileName)
{
    char szFunc[] = "CLuaDllMgr::LoadDllsFromConfigFile";

    char szSectionNames[4096];
    char szLuaDllSection[100];
    char szLuaDllName[100];
    char szLuaDllPath[100];

    // recuperation du nom de toutes les sections
    GetPrivateProfileSectionNames(szSectionNames, sizeof(szSectionNames), configFileName.c_str());
    char* pSectionName = szSectionNames;

    // chargement des DLLs
    // recherche et exploitation des sections prefixees par LUA_DLL
    // ex : 
    //      [LUA_DLL1]
    //      NAME=$(CnxDir)\DLL_LUA_Exemple.dll

    while (strlen(pSectionName) > 0)
    {
        strncpy_s(szLuaDllSection, sizeof(szLuaDllSection), pSectionName, _TRUNCATE);
        // On se positionne sur le nom de la section suivante dans le buffer
        pSectionName += strlen(pSectionName) + 1; 

        if (strstr(szLuaDllSection, PREFIXE_LUA_DLL_SECTION) != szLuaDllSection)
        {
            // Il ne s'agit pas d'une section decrivant une DLL LUA a charger
            continue;
        }
        // on recherche si il y a un nom pour cette DLL
        if (GetPrivateProfileString( &szLuaDllSection[0]
                                   , KEY_LUA_DLL_NAME
                                   , ""
                                   , &szLuaDllName[0]
                                   , sizeof(szLuaDllName)
                                   , configFileName.c_str()
                                   ) == 0)
        {
            // Il ne s'agit pas d'une section decrivant une DLL LUA a charger
            continue;
        }
        GetPrivateProfileString( &szLuaDllSection[0]
                               , KEY_LUA_DLL_PATH
                               , ""
                               , &szLuaDllPath[0]
                               , sizeof(szLuaDllPath)
                               , configFileName.c_str()
                               );
        if (0 != strlen(szLuaDllPath))
        {
            // un path additionnel pour les DLL est passe en param
            // => on le rajoute au manager de DLLs
            CDllMgr::ReadSingleInstance()->AddDllPath(std::string(szLuaDllPath));
        }
        // On charge la DLL et on verifie qu'elle possede
        // - la fonction d'ouverture qui sera appelee par l'interpreteur LUA
        // - une éventuelle fonction de fermeture qui sera appelee par l'interpreteur LUA
        // - la fonction retournant les mots clefs pour la coloration syntaxique
        T_LUA_DLL_FCTS elem;
        std::string strLuaDllName(szLuaDllName);
        elem.m_pOpenFct        = (T_OPEN_FCT)CDllMgr::ReadSingleInstance()->GetFctAddr(strLuaDllName, "LUA_open");
        elem.m_pCloseFct       = (T_CLOSE_FCT)CDllMgr::ReadSingleInstance()->GetFctAddr(strLuaDllName, "LUA_close");
        elem.m_pGetKeywordsFct = (T_GET_KEYWORDS_FCT)CDllMgr::ReadSingleInstance()->GetFctAddr(strLuaDllName, "LUA_getKeywords");
        if (NULL == elem.m_pGetKeywordsFct)
        {
            // Il s'agit peut-etre d'une 2eme version de DLL dans les fonctions et les constantes sont separees
            elem.m_pGetKeywordsFct = (T_GET_KEYWORDS_FCT)CDllMgr::ReadSingleInstance()->GetFctAddr(strLuaDllName, "LUA_getKeywordsFct");
            elem.m_pGetKeywordsCst = (T_GET_KEYWORDS_FCT)CDllMgr::ReadSingleInstance()->GetFctAddr(strLuaDllName, "LUA_getKeywordsCst");
        }
        else
        {
            elem.m_pGetKeywordsCst = NULL;
        }
        elem.m_strDllFullName  = CDllMgr::ReadSingleInstance()->GetFullName(strLuaDllName);
        if ((NULL != elem.m_pOpenFct) && (NULL != elem.m_pGetKeywordsFct))
        {
            // OK, la DLL a bien les bonnes interfaces
            // On stocke les pointeurs sur les fonctions dans la liste
            m_ListDllsFcts.push_back(elem);
        }
    }   

    return true;
}
// recuperation du nom complet (avec PATH) de la premiere DLL LUA correctement chargee
// (nom vide) si pas correctement chargee
std::string CLuaDllMgr::GetFirstLuaDllFullName(void)
{
    m_iterDll = m_ListDllsFcts.begin();
    return GetNextLuaDllFullName();
}
// recuperation du nom complet (avec PATH) de la suivante
// (nom vide) si pas correctement chargee
std::string CLuaDllMgr::GetNextLuaDllFullName(void)
{
    std::string name;
    if (m_iterDll != m_ListDllsFcts.end())
    {
        name = m_iterDll->m_strDllFullName;
        ++m_iterDll;
    }
    return name;
}

// Chargement de toutes les fonctions LUA de toutes les DLL
void CLuaDllMgr::OpenLibs(lua_State* S)
{
    // on appelle les fonctions d'ouverture
    std::list<T_LUA_DLL_FCTS>::iterator iter = m_ListDllsFcts.begin();
    while (iter != m_ListDllsFcts.end())
    {
        (iter->m_pOpenFct)(S);
        ++iter;
    }
}

// Fermeture de toutes les librairies LUA de toutes les DLL
void CLuaDllMgr::CloseLibs(void)
{
    // on appelle les fonctions de fermeture
    std::list<T_LUA_DLL_FCTS>::iterator iter = m_ListDllsFcts.begin();
    while (iter != m_ListDllsFcts.end())
    {
        if (NULL != iter->m_pCloseFct)
        {
            // la fonction de fermeture est facultative
            (iter->m_pCloseFct)();
        }
        ++iter;
    }
}

// Recuperation de tous les mots clefs de toutes les DLL
void CLuaDllMgr::GetKeywords (std::string *pStrFcts, std::string *pStrCsts)
{
    char szKeywords[2048];
    // on appelle les fonctions de recuperation des mots clefs
    std::list<T_LUA_DLL_FCTS>::iterator iter = m_ListDllsFcts.begin();
    while (iter != m_ListDllsFcts.end())
    {
        // on passe par une chaine de caractere intermediaire car il y a un probleme a
        // passe un pointeur sur une string (ou tout autre objet de la STL) vers (depuis)
        // une DLL - cf http://support.microsoft.com/kb/172396/en-us/
        if (iter->m_pGetKeywordsFct)
        {
            (iter->m_pGetKeywordsFct)(szKeywords, sizeof(szKeywords));
            *pStrFcts += szKeywords;
        }
        if (iter->m_pGetKeywordsCst)
        {
            (iter->m_pGetKeywordsCst)(szKeywords, sizeof(szKeywords));
            *pStrCsts += szKeywords;
        }
        ++iter;
    }
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaDllMgr.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
