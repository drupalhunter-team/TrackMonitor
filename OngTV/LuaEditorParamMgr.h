////////////////////////////////////////////////////////////////////////////////
/// @addtogroup LUA_EDITOR_PARAM_MGR
/// Definition du manager des parametres de l'editeur LUA (singleton)
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CLuaEditorParamMgr qui defini le singleton du manager
/// des parametres de l'editeur de script LUA (base sur scintilla)
///
///  \n \n <b>Copyright ADENEO</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaEditorParamMgr.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////


// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef LUA_EDITOR_PARAM_MGR_H
#define LUA_EDITOR_PARAM_MGR_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

class CFunctionDesc
{
NON_COPYABLE(CFunctionDesc);
public:
    // constructeur pour un mot clef fonction
    CFunctionDesc(const std::string& functionName, const std::string& retVal, const std::string& desc, const std::vector<std::string>& arrParams)
         : m_functionName(functionName), m_retVal(retVal), m_desc(desc), m_arrParams(arrParams) {};
    
    const std::string& GetFunctionName(void) {return m_functionName;};
    const std::string& GetRetVal(void) {return m_retVal;};
    const std::string& GetDesc(void) {return m_desc;};
    const std::vector<std::string>& GetArrParams(void) {return m_arrParams;};

private:
    std::string m_functionName;              // nom du mot clefs
    std::string m_retVal;                   // si fonction, description du retour de la fonction
    std::string m_desc;                     // si fonction, description de la fonction
    std::vector<std::string> m_arrParams;   // si fonction, tableau des parametres
};


class CLuaEditorParamMgr
{
NON_COPYABLE(CLuaEditorParamMgr);
private :
    CLuaEditorParamMgr();
    virtual ~CLuaEditorParamMgr();
    static CLuaEditorParamMgr* pSingleInstance;
    // Liberation des ressources de l'objet
    void Garbage(void);

public:
    typedef boost::ptr_map<std::string, CFunctionDesc> T_FUNCTIONNAME2DESC;

    // obtention du pointeur sur la seule instance du manager
    static CLuaEditorParamMgr* ReadSingleInstance(void);

    // destruction de la seule instance
    static void DeleteSingleInstance(void);

    // Chargement des parametres => on se contente de sauvegarder le nom du fichier ini
    void LoadLocalParameters(const std::string& configFileName) {m_configFileName = configFileName;};
    
    // recuperation du nombre de caracteres saisis a partir duquel on traite l'autocompletion
    int GetNbCharsToTriggerAutoCompletion(void) {return m_nbCharsToTriggerAutoCompletion;};
    
    // Nombre max de lignes dans la liste deroulante proposee pour l'autocompletion
    int GetMaxLinesInAutoCompletionList(void) {return m_maxLinesInAutoCompletionList;};
    
    // doit-on faire la mise en evidence des ()[]{}
    bool IsBracesHighlightingEnabled(void) {return m_bHighlightBraces;};
        
    // doit-on faire la mise en evidence des occurences d'un mot
    bool IsSelectedWordHighlightingEnabled(void) {return m_bHighlightSelectedWord;};
        
    // doit-on faire la mise en evidence des statements
    bool IsStatementHighlightingEnabled(void) {return m_bHighlightStatement;};
        
    // recuperation des mots-clefs standards LUA pour la coloration syntaxique
    const char *GetLuaKeyWords(void);

    // recuperation d'un pointeur sur le descripteur d'une fonction
    CFunctionDesc* GetFunctionDesc(const std::string& functionName);

    // recuperation des fonctions LUA pour la coloration syntaxique
    // il s'agit de :
    // 1. des fonctions incluses dans LUA
    // 2. des fonctions de l'interface LUA d'OngTV
    // 3. des fonctions des DLLs d'interfaces contenant des fonctions C appelables depuis LUA
    const char *GetLuaFunctions(void);

    // recuperation des constantes LUA pour la coloration syntaxique
    // il s'agit de :
    // 1. des constantes de l'interface LUA d'OngTV
    // 2. des constantes des DLLs d'interfaces contenant des fonctions C appelables depuis LUA
    const char *GetLuaConstants(void);

    // recuperation des fonctions et constantes LUA pour l'autocompletion
    // il s'agit de :
    // 1. des fonctions incluses dans LUA
    // 2. des fonctions et constantes de l'interface LUA d'OngTV
    // 3. des fonctions et constantes des DLLs d'interfaces contenant des fonctions C appelables depuis LUA
    const char *GetLuaAutoCList(void);

private:
    // on charge tout si ce n'est pas deja fait
    void Init();
    
    void LoadLocalParameters(void);
    // chargement d'un fichier contenant une description de keyword LUA. Le fichier
    // doit etre un fichier XML au format de l'autocompletion de notepad++ cf
    // http://sourceforge.net/apps/mediawiki/notepad-plus/index.php?title=Editing_Configuration_Files#Autocompletion.2C_aka_API.2C_files
    void LoadKeywordsDescFile(const std::string& fileName);

    // nombre de caracteres saisis a partir duquel on traite l'autocompletion
    int m_nbCharsToTriggerAutoCompletion;
    
    // Nombre max de lignes dans la liste deroulante proposee pour l'autocompletion
    int m_maxLinesInAutoCompletionList;
    
    // doit-on faire la mise en evidence des ()[]{}
    bool m_bHighlightBraces;
        
    // doit-on faire la mise en evidence des occurences d'un mot
    bool m_bHighlightSelectedWord;
        
    // doit-on faire la mise en evidence des statements
    bool m_bHighlightStatement;
        
    // map des descriptions des keywords
    T_FUNCTIONNAME2DESC m_mapFunctions;
    
    // liste des fonctions appelables depuis LUA
    std::string m_luaFunctions;

    // liste des constantes
    std::string m_luaConstants;

    // liste pour l'autocompletion
    std::string m_luaAutoC;

    std::string m_configFileName;   // pour stocker le nom du fichier de configuration (on ne fera l'init que lorsque l'utilisateur
                                    // editera un script LUA
    bool        m_bInitDone;        // a-t-on fait la premiere init ?
    
};

#endif // LUA_EDITOR_PARAM_MGR_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaEditorParamMgr.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
