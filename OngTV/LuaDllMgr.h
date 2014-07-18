////////////////////////////////////////////////////////////////////////////////
/// @addtogroup LUA_DLL_MGR
/// Definition du manager des DLL LUA (singleton)
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CLuaDllMgr qui defini le singleton du manager des DLL LUA
///
///  \n \n <b>Copyright ADENEO</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaDllMgr.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////


// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef LUA_DLL_MGR_H
#define LUA_DLL_MGR_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

extern "C" 
{
    #include "lua.h" 
    #include "lstate.h" 
    #include "lauxlib.h" 
    #include "lualib.h"
}
////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////
extern "C" {
    typedef __declspec(dllimport) void (*T_OPEN_FCT)(lua_State* S);
    typedef __declspec(dllimport) void (*T_CLOSE_FCT)(void);
    typedef __declspec(dllimport) void (*T_GET_KEYWORDS_FCT)(char *szKeywords, unsigned long maxSizeSzKeywords);
    typedef __declspec(dllimport) void (*T_GET_KEYWORDS_CST)(char *szKeywords, unsigned long maxSizeSzKeywords);
};

class CLuaDllMgr
{
NON_COPYABLE(CLuaDllMgr);
private :
    CLuaDllMgr();
    virtual ~CLuaDllMgr();
    static CLuaDllMgr* pSingleInstance;
    // Liberation des ressources de l'objet
    void Garbage(void);

public:
    // obtention du pointeur sur la seule instance du manager
    static CLuaDllMgr* ReadSingleInstance(void);

    // destruction de la seule instance
    static void DeleteSingleInstance(void);

    /// Chargement des DLLs d'extension LUA a partir des parametres du fichier .ini
    bool LoadDllsFromConfigFile(const std::string& configFileName);

    // Chargement de toutes les fonctions LUA de toutes les DLL
    void OpenLibs(lua_State* S);

    // Fermeture des librairairies de toutes les DLL
    void CloseLibs(void);

    // Recuperation de tous les mots clefs de toutes les DLL
    void GetKeywords(std::string *pStrFcts, std::string *pStrCsts);
    
    // recuperation du nom complet (avec PATH) de la premiere DLL LUA correctement chargee
    // (nom vide) si pas correctement chargee
    std::string GetFirstLuaDllFullName(void);
    // recuperation du nom complet (avec PATH) de la suivante
    // (nom vide) si pas correctement chargee
    std::string GetNextLuaDllFullName(void);

private:

    typedef struct {
        T_OPEN_FCT          m_pOpenFct;
        T_CLOSE_FCT         m_pCloseFct;
        T_GET_KEYWORDS_FCT  m_pGetKeywordsFct;
        T_GET_KEYWORDS_CST  m_pGetKeywordsCst;
        std::string         m_strDllFullName;
    } T_LUA_DLL_FCTS;
    std::list<T_LUA_DLL_FCTS> m_ListDllsFcts;       // liste des DLLs de fonctions
    std::list<T_LUA_DLL_FCTS>::iterator m_iterDll;// pour iteration GetFirst/GetNext
};

#endif // LUA_DLL_MGR_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaDllMgr.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
