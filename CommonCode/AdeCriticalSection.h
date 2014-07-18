////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ADE_CRITCICAL_SECTION
/// Encapsultation pour gerer des sections critiques
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de deux classe permettant d'implementer et gere facilement des
/// sections critiques :
/// CAdeCriticalSection est une classe encapsulant une section critique :
/// construction/destruction/entree/sortie
/// CAdeSLock est une classe connexe qui permet de sortie de manière implicite d'une
/// section critique.
///
///  \n \n <b>Copyright ADETEL GROUP 2011</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeCriticalSection.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : ADE_CRITICAL_SECTION_H doit être unique
// Le prefixe ADE_ et le suffixe _H sont obligatoires
#ifndef ADE_CRITICAL_SECTION_H
#define ADE_CRITICAL_SECTION_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
// puis :
// 1. les include systeme C
#include <windows.h>
// 2. les include systeme C++

// 3. les include des autres librairies (ex STL / QT)

// 4. les include du projet
#include "AdeNonCopyable.h"

////////////////////////////////////////////////////////////////////////////////
//                                                          Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                    Defines & Types de portee fichier ou globale (hors classe)
////////////////////////////////////////////////////////////////////////////////
// On evitera en principe les macros ...


////////////////////////////////////////////////////////////////////////////////
//                                                                       Classes
////////////////////////////////////////////////////////////////////////////////


/**
 *  Cette classe encapsule les sections critiques de windows
*/ 
class CAdeCriticalSection
{
NON_COPYABLE(CAdeCriticalSection);
public:
    // creation de la section critique
    CAdeCriticalSection()
        { ::InitializeCriticalSection(&m_rep); }
    // destruction de la section critique
    ~CAdeCriticalSection()
        { ::DeleteCriticalSection(&m_rep); }
    
    // Entree dans la section critique
    void Enter()
        { ::EnterCriticalSection(&m_rep); }
    // Sortie de la section critique
    void Leave()
        { ::LeaveCriticalSection(&m_rep); }
    
private:
    CRITICAL_SECTION m_rep;
};

/**
 *  Cette classe fonctionne de concert CAdeCriticalSection.
 *  En declarant un objet de ce type, on entre dans la section
 *  critique associee. La sortie de cette section critique
 *  aura lieu lors de la destruction de l'objet.
 *  Ex : 
 *  void MaCLass::MaFct(void)
 *  {
 *     // passage en section critique
 *     CAdeSLock lock(m_criticalSection);
 *     ...
 *     if (cond)
 *     {
 *         return; // sortie implicite de la section critique
 *     }
 *     ...
 *     return; // sortie implicite de la section critique
 *  }
*/ 
class CAdeSLock
{
NON_COPYABLE(CAdeSLock);
public:
    // entree dans la section critique associee lors
    // de la construction de l'objet
    CAdeSLock(CAdeCriticalSection& a_section)
        : m_section(a_section) { m_section.Enter(); }
    // Sortie de la section critique sur la destruction
    // de l'objet
    ~CAdeSLock()
        { m_section.Leave(); }
    
private:
    CAdeCriticalSection& m_section;
};

#endif // ADE_CRITICAL_SECTION_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeCriticalSection.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
