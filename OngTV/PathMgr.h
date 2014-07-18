////////////////////////////////////////////////////////////////////////////////
/// @addtogroup PATH
/// Classe de base pour les services
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CPathMgr qui defini le singleton du gestionnaire de
/// path
///
///  \n \n <b>Copyright ADENEO 2008</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/PathMgr.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef PATHMGR_H
#define PATHMGR_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// Uniquement des includes projet : pas d'inclusion de fichier standard

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////
class CPathMgr
// Class singleton => le constructeur est prive!
{
NON_COPYABLE(CPathMgr);
private :
    CPathMgr();
    virtual ~CPathMgr();

    static CPathMgr* pSingleInstance;

public:
    // obtention du pointeur sur la seule instance du manager
    static CPathMgr* ReadSingleInstance(void);

    // destruction de la seule instance
    static void DeleteSingleInstance(void);

    // Liberation des ressources de l'objet
    void Garbage(void);

    // chargement des parametres locaux
    bool LoadLocalParameters(const std::string& configFileName);

    // Declaration d'un alias
    void SetAlias(const std::string& strPathAlias, const std::string& strPath);

    // Extraction d'un path d'un nom de fichier
    static std::string ExtractPath(const std::string& strFileName);

    // Substitue tous les alias dans un path
    void SubsituteAllAlias(std::string* pStrPath);

    // Modifie eventuellement le path en lui subsituant un alias par le path correspondant
    // s'il commence par cet alias
    void SubstituteAliasWithPath(std::string* pStrPath);

    // Modifie eventuellement le path en remplacant tout ou partie d'un path 
    // par l'alias correspondant 
    void SubstitutePathWithAlias(std::string* pStrPath);

    // Obtention du directory associe a un suffixe
    std::string GetSuffixDirectory(const std::string& strSuffix);

    // Positionnement du directory associe a un suffixe
    void SetSuffixDirectory(const std::string& strSuffix, const std::string& strDirectory);

private:
    std::map<std::string, std::string> m_dicoAlias;     // map des alias
    std::map<std::string, std::string> m_dicoDirectory; // map des repertoires (on associe un repertoire à un suffixe)
    
    std::string m_configFileName;   // Nom du fichier de configuration
    bool        m_isUpdated;        // y'a-t-il une modification des associations ?
    bool        m_bSaveDirOnExit;   // faut-il sauvegarder les associations a la sortie (si elles ont
                                    // ete modifiees)
    
};

#endif // PATHMGR_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/PathMgr.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
