////////////////////////////////////////////////////////////////////////////////
/// @addtogroup PATH
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe du gestionnaire de path (singleton CPathMgr)
///
/// \n \n <b>Copyright ADENEO 2008</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/PathMgr.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                    / Librairie)
#include "stdafx.h"

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
CPathMgr* CPathMgr::pSingleInstance = NULL;
CPathMgr* CPathMgr::ReadSingleInstance(void)
{
    if (pSingleInstance == NULL)
        pSingleInstance = new(CPathMgr);

    return pSingleInstance;
}
////////////////////////////////////////////////////////////////////////////////
/// Destruction du gestionnaire de services
////////////////////////////////////////////////////////////////////////////////
void CPathMgr::DeleteSingleInstance(void)
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
CPathMgr::CPathMgr() 
{
    m_isUpdated = false;
    m_bSaveDirOnExit = false;
}
////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CPathMgr::~CPathMgr()
{
    Garbage();
}

////////////////////////////////////////////////////////////////////////////////
/// Liberation des ressources de l'objet
////////////////////////////////////////////////////////////////////////////////
void CPathMgr::Garbage (void)
{
    // s'il y a eu une modification et que l'on doit sauvegarder,
    // on reecrit la section dans le fichier de config
    if (m_isUpdated && m_bSaveDirOnExit)
    {
        std::map<std::string, std::string>::iterator iter;
        std::string path;
        // On parcourt toutes les associations
        for (iter = m_dicoDirectory.begin() ; iter != m_dicoDirectory.end() ; ++iter)
        {
            path = iter->second;
            // Eventuellement, on remplace un path par un alias
            SubstitutePathWithAlias(&path);
            // et on ecrit dans le fichier de config
            WritePrivateProfileString( DIR_SECTION
                                     , iter->first.c_str()
                                     , path.c_str()
                                     , m_configFileName.c_str()
                                     );
        }
    }
    // liberation des dico
    m_dicoAlias.clear();
    m_dicoDirectory.clear();
}

////////////////////////////////////////////////////////////////////////////////
/// Chargement de la liste des paramètres locaux a partir du fichier 
/// de description des processeurs (format fichier .ini)
///
/// Parametres :
/// \param configFileName nom du fichier contenant les parametres
///
/// \return true si la recuperation des parametres s'est bien deroulee, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CPathMgr::LoadLocalParameters(const std::string& configFileName)
{
    char szDirNames[4096];
    char szSaveDirOnExit[100];
    
    // on stocke le nom de fichier dans lequel on stockera les associations suffixe/repertoire
    m_configFileName = configFileName;

    // on regarde s'il faut sauver les associations suffixe/repertoire a la sortie
    GetPrivateProfileString( GENERAL_SECTION
                           , KEY_SAVE_DIR_ON_EXIT
                           , "1"
                           , &szSaveDirOnExit[0]
                           , sizeof(szSaveDirOnExit)
                           , configFileName.c_str()
                           );
    if (strcmp(szSaveDirOnExit, "1") == 0)
    {
        m_bSaveDirOnExit = true;
    }

     // recuperation des clefs de la section DIR
    GetPrivateProfileSection(DIR_SECTION, szDirNames, sizeof(szDirNames), configFileName.c_str());
    char* pKeyName = szDirNames;

    while (strlen(pKeyName) > 0)
    {
        // The data in the buffer pointed to by the lpReturnedString parameter consists of one or more 
        // null-terminated strings, followed by a final null character. Each string has the following format:
        // key=string
        char* pEqualChar = strchr(pKeyName, '=');
        if (NULL == pEqualChar)
        {
            // pb, ne devrait pas arriver
        }
        else
        {
            // le chaine representant le repertoire associee a la clef est juste apres le =
            char* pDir = pEqualChar + 1;
            *pEqualChar = 0;    // On remplace le = par un 0 pour terminer la clef
            // On insere la paire dans le dico
            m_dicoDirectory[pKeyName] = pDir;
            // on revient a la chaine initiale (pour que le strlen suivant fonctionne)
            *pEqualChar = '=';
        }
        // On se positionne sur le nom de la clef suivante dans le buffer
        pKeyName += strlen(pKeyName) + 1; 
    }   
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Declaration d'un alias
///
/// Parametres :
/// \param symbolName nom du symbole
///
/// \return pointeur sur le symbole s'il a ete trouve NULL sinon
////////////////////////////////////////////////////////////////////////////////
void CPathMgr::SetAlias(const std::string& strPathAlias, const std::string& strPath)
{
    m_dicoAlias[strPathAlias] = strPath;
}

////////////////////////////////////////////////////////////////////////////////
/// Extraction d'un path d'un nom de fichier
///
/// Parametres :
/// \param strFileName nom du fichier duquel extraire le path
///
/// \return path ou chaine vide si aucun path n'a ete trouve
////////////////////////////////////////////////////////////////////////////////
// 
std::string CPathMgr::ExtractPath(const std::string& strFileName)
{
    // On recherche d'abord un backslash
    std::string::size_type pos = strFileName.rfind("\\");
    if (pos != std::string::npos)
    {
        return strFileName.substr(0, pos);
    }
    // Puis on recherche un /
    pos = strFileName.rfind("/");
    if (pos != std::string::npos)
    {
        return strFileName.substr(0, pos);
    }
    return "";
}

////////////////////////////////////////////////////////////////////////////////
/// Modifie eventuellement le path en lui subsituant un alias par le path correspondant
/// s'il commence par cet alias
///
/// Parametres :
/// \param strPath path
////////////////////////////////////////////////////////////////////////////////
void CPathMgr::SubstituteAliasWithPath(std::string* pStrPath)
{
    std::map<std::string, std::string>::iterator iter;

    for (iter = m_dicoAlias.begin() ; iter != m_dicoAlias.end() ; ++iter)
    {
        if (pStrPath->substr(0, iter->first.size()) == iter->first)
        {
            // le path commence par l'alias courant, on lui substitue le path de l'alias
            *pStrPath = iter->second + pStrPath->substr(iter->first.size());
            // Et on a fini le parcours
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Modifie eventuellement le path en remplacant tout ou partie d'un path 
/// par l'alias correspondant 
///
/// Parametres :
/// \param strPath path
////////////////////////////////////////////////////////////////////////////////
void CPathMgr::SubstitutePathWithAlias(std::string* pStrPath)
{
    std::map<std::string, std::string>::iterator iter;

    for (iter = m_dicoAlias.begin() ; iter != m_dicoAlias.end() ; ++iter)
    {
        if (pStrPath->substr(0, iter->second.size()) == iter->second)
        {
            // le path commence par le path courant, on lui substitue l'alias correspondant
            *pStrPath = iter->first + pStrPath->substr(iter->second.size());
            // Et on a fini le parcours
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Modifie eventuellement le path en lui subsituant un alias par le path correspondant
/// s'il commence par cet alias
///
/// Parametres :
/// \param strPath path
////////////////////////////////////////////////////////////////////////////////
void CPathMgr::SubsituteAllAlias(std::string* pStrPath)
{
    std::map<std::string, std::string>::iterator iter;
    for (iter = m_dicoAlias.begin() ; iter != m_dicoAlias.end() ; ++iter)
    {
        size_t pos;
        do {
            pos = pStrPath->find(iter->first);
            if (pos != std::string::npos)
            {
                std::string strNewPath;
                if (pos > 0)
                {
                    // on prend le debut
                    strNewPath = pStrPath->substr(0, pos);
                }
                // On substitue l'alias
                strNewPath += iter->second;
                // on met la fin
                strNewPath += pStrPath->substr(pos + iter->first.size());
                *pStrPath = strNewPath;
            }
        } while (pos != std::string::npos);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Obtention du directory associe a un suffixe
///
/// Parametres :
/// \param strSuffix suffixe
///
/// \return directory associe a ce suffixe
////////////////////////////////////////////////////////////////////////////////
std::string CPathMgr::GetSuffixDirectory(const std::string& strSuffix)
{
    std::map<std::string, std::string>::iterator iter = m_dicoDirectory.find(strSuffix);
    if (iter == m_dicoDirectory.end())
    {
        // pas trouve => chaine vide
        return "";
    }
    else
    {
        std::string str = iter->second;
        // on y remplace un eventuel alias
        SubstituteAliasWithPath(&str);
        return str;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Positionnement du directory associe a un suffixe
///
/// Parametres :
/// \param strSuffix    suffixe
/// \param strDirectory repertoire associe
///
/// \return directory associe a ce suffixe
////////////////////////////////////////////////////////////////////////////////
void CPathMgr::SetSuffixDirectory(const std::string& strSuffix, const std::string& strDirectory)
{
    if (GetSuffixDirectory(strSuffix) != strDirectory)
    {
        // l'association est modifiee
        m_dicoDirectory[strSuffix] = strDirectory;
        // et on marque qu'il y a une modification
        m_isUpdated = true;
    }
}


////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/PathMgr.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
