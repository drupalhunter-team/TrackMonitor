////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DLLMGR
/// Classe de base pour la gestion des DLL contenant des fonctions de transfert
/// ou des RPCs locales (ie fct d'une DLL ayant le format d'une RPC)
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CDllMgr qui defini le singleton du gestionnaire
/// des DLLs contenant les fonctions de transfert
///
///  \n \n <b>Copyright ADENEO 2008</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/DllMgr.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef DLLMGR_H
#define DLLMGR_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// Uniquement des includes projet : pas d'inclusion de fichier standard


////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

/// definition d'un pointeur sur une fonction de transfert
/// Parametres :
/// \param pVal        pointeur sur la valeur double a transformer
/// \param action      action demandee (fonction de transfert ou inverse de la fonction)
///
/// \return 0 pour l'instant
#define TRANSF_FCT_ACTION_FCT       0
#define TRANSF_FCT_ACTION_INV_FCT   1
typedef unsigned (*T_TRANSF_FCT) (double* pVal, unsigned action);

/// definition d'un pointeur sur une RPC local
/// Parametres :
/// @param inpP0        1er parametre d'entree
/// @param inpP1        2eme parametre d'entree
/// @param inpP2        3eme parametre d'entree
/// @param inpP3        4eme parametre d'entree
/// @param inpP4        5eme parametre d'entree
/// @param pOutP0       pointeur sur le 1er parametre de sortie
/// @param pOutP1       pointeur sur le 2eme parametre de sortie
/// @param pOutP2       pointeur sur le 3eme parametre de sortie
/// @param pOutP3       pointeur sur le 4eme parametre de sortie
/// @param pOutP4       pointeur sur le 5eme parametre de sortie
///
/// \return code de retour de la RPC
typedef unsigned long (*T_RPC_FCT) ( unsigned long inpP0
                                   , unsigned long inpP1
                                   , unsigned long inpP2
                                   , unsigned long inpP3
                                   , unsigned long inpP4
                                   , unsigned long* pOutP0
                                   , unsigned long* pOutP1
                                   , unsigned long* pOutP2
                                   , unsigned long* pOutP3
                                   , unsigned long* pOutP4
                                   );

class CDllMgr
// Class singleton => le constructeur est prive!
{
NON_COPYABLE(CDllMgr);
private :
    CDllMgr();
    virtual ~CDllMgr();

    static CDllMgr* pSingleInstance;

public:
    // obtention du pointeur sur la seule instance du manager
    static CDllMgr* ReadSingleInstance(void);

    // destruction de la seule instance
    static void DeleteSingleInstance(void);

    // Liberation des ressources de l'objet
    void Garbage(void);
    
    // retourne le pointeur de fonction sur la fonction de transfert
    T_TRANSF_FCT GetTransfFct(const std::string& strDllName, const std::string& strFctName) {return (T_TRANSF_FCT)GetFctAddr(strDllName, strFctName);};

    // retourne le pointeur de fonction sur la RPC local
    T_RPC_FCT GetLocalRpc(const std::string& strDllName, const std::string& strRpcName) {return (T_RPC_FCT)GetFctAddr(strDllName, strRpcName);};

    // retourne l'adresse d'une fonction dans une DLL
    FARPROC GetFctAddr(const std::string& strDllName, const std::string& strFctName);

    // retourne le nom complet de la DLL (comprenant le path de l'endroit ou elle a ete trouvee)
    // ne marche que si la DLL a ete correctement chargee
    std::string GetFullName(const std::string& strDllName);

    // Ajout d'un path additionnel pour le chargement des DLLs
    void AddDllPath(const std::string& strDllPath);

private:
    HMODULE GetHandle(const std::string& strDllName);
    typedef std::map<std::string, HMODULE> T_NAME2HMODULE;
    T_NAME2HMODULE m_DicoDll;    // dictionnaire des Dll contenant des fonctions de transformations
    
};

#endif // DLLMGR_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/DllMgr.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
