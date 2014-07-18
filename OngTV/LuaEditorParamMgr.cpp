////////////////////////////////////////////////////////////////////////////////
/// @addtogroup LUA_EDITOR_PARAM_MGR
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implemente la classe du gestionnaire des parametres (keywords et autocompletion)
/// pour l'editeur de script LUA (base sur Scintilla) (singleton CLuaEditorParamMgr)
///
/// \n \n <b>Copyright ADENEO</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaEditorParamMgr.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                     / Librairie)

#include "stdafx.h"

#include "LuaEditorParamMgr.h"
#include "PathMgr.h"
#include "LuaDllMgr.h"
#include "tinyxml.h"
#include "Utils.h"

////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                     Variables
////////////////////////////////////////////////////////////////////////////////
// Note : OBLIGATOIREMENT static

const char luaKeywords[] =
    _T(" and break do else elseif end false for function if ")
    _T("in local nil not or repeat return then true until while");

const char luaFunctions[] = 
    _T(" _G _VERSION assert collectgarbage dofile error getfenv getmetatable ")
    _T("ipairs load loadfile loadstring module next pairs pcall print ")
    _T("rawequal rawget rawset require select setfenv setmetatable tonumber ")
    _T("tostring type unpack xpcall gcinfo newproxy ")
    
    _T("debug.debug debug.getfenv debug.gethook debug.getinfo ")
    _T("debug.getlocal debug.getmetatable debug.getregistry ")
    _T("debug.getupvalue debug.setfenv debug.sethook debug.setlocal ")
    _T("debug.setmetatable debug.setupvalue debug.traceback ")

    _T("io.close io.flush io.input io.lines io.open io.output io.popen ")
    _T("io.read io.stderr io.stdin io.stdout io.tmpfile io.type io.write ")

    _T("math.abs math.acos math.asin math.atan math.atan2 math.ceil ")
    _T("math.cos math.cosh math.deg math.exp math.floor math.fmod ")
    _T("math.frexp math.huge math.ldexp math.log math.log10 math.max ")
    _T("math.min math.modf math.pi math.pow math.rad math.random ")
    _T("math.randomseed math.sin math.sinh math.sqrt math.tan math.tanh ")
    _T("math.mod ")
    
    _T("os.clock os.date os.difftime os.execute os.exit os.getenv os.remove ")
    _T("os.rename os.setlocale os.time os.tmpname ")

    _T("package.cpath package.loaded package.loaders package.loadlib ")
    _T("package.path package.preload package.seeall package.config ")

    _T("string.byte string.char string.dump string.find string.format ")
    _T("string.gmatch string.gsub string.len string.lower string.match ")
    _T("string.rep string.reverse string.sub string.upper string.gfind ")

    _T("table.concat table.insert table.maxn table.remove table.sort ")
    _T("table.setn table.getn table.foreachi table.foreach ")

    _T("bit.tobit bit.bnot bit.band bit.bor bit.bxor bit.lshift bit.rshift ")
    _T("bit.arshift bit.rol bit.ror bit.bswap bit.tohex ");

////////////////////////////////////////////////////////////////////////////////
//                                             Prototypes des fonctions internes
////////////////////////////////////////////////////////////////////////////////
// Note : OBLIGATOIREMENT static

extern "C" void ITFLUA_getKeywords (std::string *pStrFcts, std::string *pStrCsts);

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Obtention de l'instance unique du gestionnaire de connexions
///
/// \return : pointeur sur le gestionnaire
////////////////////////////////////////////////////////////////////////////////
CLuaEditorParamMgr* CLuaEditorParamMgr::pSingleInstance = NULL;
CLuaEditorParamMgr* CLuaEditorParamMgr::ReadSingleInstance(void)
{
    if (pSingleInstance == NULL)
        pSingleInstance = new(CLuaEditorParamMgr);

    return pSingleInstance;
}
////////////////////////////////////////////////////////////////////////////////
/// Destruction du gestionnaire de connexions
////////////////////////////////////////////////////////////////////////////////
void CLuaEditorParamMgr::DeleteSingleInstance(void)
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
CLuaEditorParamMgr::CLuaEditorParamMgr() : m_bInitDone(false)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CLuaEditorParamMgr::~CLuaEditorParamMgr()
{
    Garbage();
}

////////////////////////////////////////////////////////////////////////////////
/// Liberation des ressources de l'objet
////////////////////////////////////////////////////////////////////////////////
void CLuaEditorParamMgr::Garbage (void)
{
    m_mapFunctions.clear();
}


////////////////////////////////////////////////////////////////////////////////
/// recuperation des mots-clefs standards LUA pour la coloration syntaxique
///
////////////////////////////////////////////////////////////////////////////////
const char *CLuaEditorParamMgr::GetLuaKeyWords(void)
{
    Init();
    return (const char*)luaKeywords;
}

////////////////////////////////////////////////////////////////////////////////
/// recuperation des fonctions LUA pour la coloration syntaxique
/// il s'agit de :
/// 1. des fonctions incluses dans LUA
/// 2. des fonctions de l'interface LUA d'OngTV
/// 3. des fonctions des DLLs d'interfaces contenant des fonctions C appelables depuis LUA
////////////////////////////////////////////////////////////////////////////////
const char *CLuaEditorParamMgr::GetLuaFunctions(void)
{
    Init();
    return m_luaFunctions.c_str();
}

////////////////////////////////////////////////////////////////////////////////
/// recuperation des constantes LUA pour la coloration syntaxique
/// il s'agit de :
/// 1. des constantes de l'interface LUA d'OngTV
/// 2. des constantes des DLLs d'interfaces contenant des fonctions C appelables depuis LUA
////////////////////////////////////////////////////////////////////////////////
const char *CLuaEditorParamMgr::GetLuaConstants(void)
{
    Init();
    return m_luaConstants.c_str();
}

////////////////////////////////////////////////////////////////////////////////
/// recuperation des fonctions et constantes LUA pour l'autocompletion
/// il s'agit de :
/// 1. des fonctions incluses dans LUA
/// 2. des fonctions et constantes de l'interface LUA d'OngTV
/// 3. des fonctions et constantes des DLLs d'interfaces contenant des fonctions C appelables depuis LUA
////////////////////////////////////////////////////////////////////////////////
const char *CLuaEditorParamMgr::GetLuaAutoCList(void)
{
    Init();
    return m_luaAutoC.c_str();
}

////////////////////////////////////////////////////////////////////////////////
/// Recuperation d'un pointeur sur le descripteur d'une fonction
///
/// Parametres :
/// \param functionName     nom de la fonction
///
/// \return pointeur sur le descripteur, NULL si fonction pas dans le dico
////////////////////////////////////////////////////////////////////////////////
CFunctionDesc* CLuaEditorParamMgr::GetFunctionDesc(const std::string& functionName)
{
    T_FUNCTIONNAME2DESC::iterator iter = m_mapFunctions.find(functionName);
    if (iter == m_mapFunctions.end())
    {
        return NULL;
    }
    else
    {
        return iter->second;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// on charge tout si ce n'est pas deja fait
///
////////////////////////////////////////////////////////////////////////////////
static bool stringCompareIgnoringCase( const std::string &left, const std::string &right )
{
    unsigned len = min(left.size(), right.size());
    for (unsigned i = 0; i < len; ++i)
    {
        int cL = toupper(left[i]);
        int cR = toupper(right[i]);
        if (cL < cR)
        {
            return true;
        }
        if (cL > cR)
        {
            return false;
        }
    }
    if (left.size() < right.size())
    {   return true;
    }
    return false;
}
void CLuaEditorParamMgr::Init(void)
{
    if (m_bInitDone)
    {
        // deja faite
        return;
    }
    m_bInitDone = true;
    // construction de la liste des fonctions appelables depuis LUA
    // on recupere les mots clefs de l'interface LUA
    m_luaFunctions = luaFunctions;
    // puis on ajoute ceux de l'interface LUA d'OngTV
    ITFLUA_getKeywords(&m_luaFunctions, &m_luaConstants);
    // Puis ceux de toutes les DLL 
    CLuaDllMgr::ReadSingleInstance()->GetKeywords(&m_luaFunctions, &m_luaConstants);
    
    m_luaAutoC = m_luaFunctions + " " + m_luaConstants;

    // on construit maintenant la liste d'autocompletion
    // on va maintenant trier cette liste en se servant d'un vecteur et
    // d'un iterateur de stream
    std::istringstream streamLuaAutoC(m_luaAutoC.c_str());
    std::vector<std::string> arrLuaAutoC;
    std::string name;
    streamLuaAutoC >> name;
    while (streamLuaAutoC.good())
    {
        arrLuaAutoC.push_back(name);
        streamLuaAutoC >> name;
    }
    if (name.size() > 0)
    {
        // on ajoute la derniere chaine
        arrLuaAutoC.push_back(name);
    }
    // on va trier le vecteur sans tenir compte des majuscules/minuscules (car dans on indique
    // a l'autocompletion d'ignorer les majuscules)
    std::sort(arrLuaAutoC.begin(), arrLuaAutoC.end(), stringCompareIgnoringCase);
             
    // puis on reecrit dans la chaine
    std::vector<std::string>::iterator iter = arrLuaAutoC.begin();
    m_luaAutoC.clear();
    while (iter != arrLuaAutoC.end())
    {
        m_luaAutoC += ' ';
        m_luaAutoC += *iter;
        ++iter;
    }
    // chargement des parametres de l'autocompletion
    LoadLocalParameters();
}
////////////////////////////////////////////////////////////////////////////////
/// Chargement des parametres pour l'autocompletion
///
////////////////////////////////////////////////////////////////////////////////
void CLuaEditorParamMgr::LoadLocalParameters(void)
{
    char szFunc[] = "CLuaEditorParamMgr::LoadLocalParameters";

    // lecture des parametres de configuration
    // nombre de caracteres saisis a partir duquel on traite l'autocompletion
    m_nbCharsToTriggerAutoCompletion = GetPrivateProfileInt( LUA_EDITOR_SECTION
                                                           , KEY_LUA_EDITOR_NB_CHARS_TO_TRIGGER_AUTOC
                                                           , DEFAULT_LUA_EDITOR_NB_CHARS_TO_TRIGGER_AUTOC
                                                           , m_configFileName.c_str()
                                                           );
    if (m_nbCharsToTriggerAutoCompletion < 1)
    {
        ADE_WARNING("mauvais parametre KEY_LUA_EDITOR_NB_CHARS_TO_TRIGGER_AUTOC " << m_nbCharsToTriggerAutoCompletion);
        m_nbCharsToTriggerAutoCompletion = DEFAULT_LUA_EDITOR_NB_CHARS_TO_TRIGGER_AUTOC;
    }

    // Nombre max de lignes dans la liste deroulante proposee pour l'autocompletion
    m_maxLinesInAutoCompletionList = GetPrivateProfileInt( LUA_EDITOR_SECTION
                                                         , KEY_LUA_EDITOR_MAX_LINES_IN_AUTOC_LIST
                                                         , DEFAULT_LUA_EDITOR_MAX_LINES_IN_AUTOC_LIST
                                                         , m_configFileName.c_str()
                                                         );
    if (m_maxLinesInAutoCompletionList < 1)
    {
        ADE_WARNING("mauvais parametre KEY_LUA_EDITOR_MAX_LINES_IN_AUTOC_LIST " << m_maxLinesInAutoCompletionList);
        m_maxLinesInAutoCompletionList = DEFAULT_LUA_EDITOR_MAX_LINES_IN_AUTOC_LIST;
    }

    // masque pour les mises en evidence
    int mskHighLighting = GetPrivateProfileInt( LUA_EDITOR_SECTION
                                              , KEY_LUA_EDITOR_MSK_HIGHLIGHTING
                                              , DEFAULT_LUA_EDITOR_MSK_HIGHLIGHTING
                                              , m_configFileName.c_str()
                                              );
    m_bHighlightBraces       = mskHighLighting & MSK_HIGHLIGTH_BRACES        ? true : false;
    m_bHighlightSelectedWord = mskHighLighting & MSK_HIGHLIGTH_SELECTED_WORD ? true : false;
    m_bHighlightStatement    = mskHighLighting & MSK_HIGHLIGTH_STATEMENT     ? true : false;

    // on charge les fichiers de description des fonctions C accessibles depuis LUA
    // 1. le fichier decrivant les fonctions de l'interface C d'OngTV
    std::string adeViewLuaDescFileName = ALIAS_EXEPATH + std::string("\\OngTVLua.xml");
    CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&adeViewLuaDescFileName);
    LoadKeywordsDescFile(adeViewLuaDescFileName);
    // 2. pour chacune des DLLs LUA chargees, on essaye de charger un fichier de description
    //    qui aurait le meme nom que la DLL avec le suffixe .dll remplace pas .xml
    for ( std::string luaDllFullName = CLuaDllMgr::ReadSingleInstance()->GetFirstLuaDllFullName()
        ; luaDllFullName.size() > 0
        ; luaDllFullName = CLuaDllMgr::ReadSingleInstance()->GetNextLuaDllFullName()
        )
    {
        // On regarde si la chaine se termine bien par .DLL
        ToUpper(&luaDllFullName);
        size_t posSuffix = luaDllFullName.rfind(".DLL");
        if (std::string::npos != posSuffix)
        {
            // On remplace par .xml et on charge le fichier
            luaDllFullName.replace(posSuffix, 4, ".XML");
            LoadKeywordsDescFile(luaDllFullName);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Chargement d'un fichier contenant une description de keyword LUA. Le fichier
/// doit etre un fichier XML au format de l'autocompletion de notepad++ cf
/// http://sourceforge.net/apps/mediawiki/notepad-plus/index.php?title=Editing_Configuration_Files#Autocompletion.2C_aka_API.2C_files
/// On n'exploite que les fonctions (pour l'affichage des descriptions de fonctions)
///
/// Parametres :
/// \param fileName  nom du fichier 
///
////////////////////////////////////////////////////////////////////////////////
void CLuaEditorParamMgr::LoadKeywordsDescFile(const std::string& fileName)
{
    char szFunc[] = "CLuaEditorParamMgr::LoadKeywordsDescFile";

    TiXmlElement* pXMLElem = NULL;

    // creation document XML
    boost::scoped_ptr<TiXmlDocument> pXMLDoc (new TiXmlDocument(fileName));
    if (pXMLDoc->LoadFile() == false)
    {
        ADE_WARNING("impossible de charger le fichier de description LUA " << fileName);
        return;
    }

    pXMLElem = pXMLDoc->RootElement();
    if (pXMLElem == NULL)
    {
        ADE_WARNING("impossible de trouver le noeud root " << fileName);
        return;
    }
    if (pXMLElem->ValueStr() != "NotepadPlus")
    {
        ADE_WARNING("impossible de trouver le noeud NotepadPlus " << fileName);
        return;
    }
    pXMLElem = pXMLElem->FirstChildElement("AutoComplete");
    if (pXMLElem == NULL)
    {
        ADE_WARNING("impossible de trouver le noeud AutoComplete " << fileName);
        return;
    }
    // on verifie qu'il s'agit bien d'une description pour LUA
    std::string language;
    pXMLElem->QueryStringAttribute("language", &language);
    if (language != std::string("LUA"))
    {
        ADE_WARNING("pas un fichier de description de mots clefs LUA " << fileName);
        return;
    }
    // on va maintenant charger les fonctions
    for ( TiXmlElement *pElemKeyword = pXMLElem->FirstChildElement("KeyWord")
        ; pElemKeyword != NULL
        ; pElemKeyword = pElemKeyword->NextSiblingElement("KeyWord")
        )
    {
        boost::unique_ptr<CFunctionDesc> pFunctionDesc;
        std::string name;
        // on recupere le nom
        if (TIXML_SUCCESS != pElemKeyword->QueryStringAttribute("name", &name))
        {
            // pas de nom => on ignore
            ADE_WARNING("keyword sans attribut name");
            continue;
        }
        // on regarde s'il s'agit de la description d'une fonction
        bool isFunction = false;
        pElemKeyword->QueryBoolAttribute("func", &isFunction);
        if (isFunction)
        {
            // il s'agit de la declaration d'une fonction
            // comme il n'y a pas de polymorphisme en LUA, on recupere le premier overload
            TiXmlElement* pElemOverload = pElemKeyword->FirstChildElement("Overload");
            if (pElemOverload)
            {
                std::string desc;
                std::string retVal;
                std::vector<std::string> arrParams;
                pElemOverload->QueryStringAttribute("retVal", &retVal);
                pElemOverload->QueryStringAttribute("descr", &desc);
                // puis on recupere les parametres
                for ( TiXmlElement *pElemParam = pElemOverload->FirstChildElement("Param")
                    ; pElemParam != NULL
                    ; pElemParam = pElemParam->NextSiblingElement("Param")
                    )
                {
                    std::string paramName;
                    if (TIXML_SUCCESS != pElemParam->QueryStringAttribute("name", &paramName))
                    {
                        // pas de nom => on ignore
                        ADE_WARNING("parametre sans attribut name");
                    }
                    else
                    {
                        // on l'ajoute au tableau des parametres
                        arrParams.push_back(paramName);
                    }
                }
                // on cree la fonction
                pFunctionDesc.reset(new CFunctionDesc(name, retVal, desc, arrParams));
            }
            else
            {
                // pas de description => on l'ignore
                ADE_WARNING("fonction sans description : " << name);
            }
        }
        else
        {
            // mot clef tout seul => on l'ignore
        }
        // On l'insere dans le dico a qui l'on transfere la propriete de l'objet
        if (pFunctionDesc)
        {
            m_mapFunctions.insert(name, pFunctionDesc.release());
        }
    }


}


////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/LuaEditorParamMgr.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
