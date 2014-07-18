////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ADE_FIFO
/// Template d'une FIFO basee sur une boost::ptr_list mais offrant une fonction
/// de lecture avec timeout
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
///  \n \n <b>Copyright ADETEL GROUP</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeFifo.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : ADE_FIFO_H doit être unique
// Le prefixe ADE_ et le suffixe _H sont obligatoires
#ifndef ADE_FIFO_H
#define ADE_FIFO_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
// puis :
// 1. les include systeme C
#include <windows.h>
// 2. les include systeme C++

// 3. les include des autres librairies (ex STL / QT)
#include <list>

// 4. les include du projet
#include "AdeNonCopyable.h"
#include "AdeCriticalSection.h"

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
 *  Template d'une FIFO de pointeurs sur des objets T
 *  La specificité de cette classe par rapport à une FIFO basée sur std::list
 *  ou std::deque est qu'un semaphore est associé à la FIFO : le nombre de
 *  jetons dans le sémaphore reflète le nombre d'éléments dans la FIFO.
 *  L'utilisation de ce sémaphore permet de faire une lecture bloquante avec
 *  gestion d'un timeour dans cette FIFO.
 * 
 *  Note : une fois un élément inséré dans la FIFO (par son pointeur), la FIFO
 *  devient la propriétaire de l'objet et il sera détruit lors de la destruction
 *  de la FIFO ou lors d'un vidage de la FIFO (flush).
 *
 *  Note : il est possible de récupérer le handle du sémaphore pour faire une
 *  attente directe sur ce sémaphore dans un thread, dans ce cas, il faut ensuite
 *  ensuite demander une lecture (Pop) sans faire de wait sur le handle afin
 *  de garder la FIFO et le sémaphore synchronisés.
*/
 template <class T>
class CAdeFifo
{
NON_COPYABLE(CAdeFifo);
#define ADE_FIFO_MAX_ELEM_DEFAULT   1000
public:
    /** Constructeur.
     * @param maxElemsInFifo[in]  nombre max d'éléments pouvant être mis dans la FIFO
     *                            (nécéssaire pour initialiser la capacité max du sémaphore en jetons)
    */
    CAdeFifo(const unsigned long maxElemsInFifo = ADE_FIFO_MAX_ELEM_DEFAULT) : m_maxElemsInFifo(maxElemsInFifo)
    {
        // creation du semaphore associe (nombre d'unite = nombre d'elements dans la FIFO)
        m_hSemaphoreCntFifo = ::CreateSemaphore(NULL, 0, m_maxElemsInFifo, NULL);
    };
    /** Destructeur : on vide la FIFO en détruisant tous les éléments.
    */
    virtual ~CAdeFifo()
    {
        CloseHandle(m_hSemaphoreCntFifo);
        // vidage de la fifo
        while (0 != m_fifo.size())
        {
            delete m_fifo.front();
            m_fifo.pop_front();
        }
    };
    
    /** Empilage d'un élément dans la FIFO.
     *
     * @param pElem[in]             pointeur sur l'element à mettre dans la FIFO
     * @param deleteIfPushError[in] flag indiquant s'il faut détruire l'élément si on
     *                              ne peut pas l'inserer
     * 
     * @retval true si l'élément a été ajouté dans la FIFO, false sinon
    */
    bool Push(T* const pElem, const bool deleteIfPushError)
    {
        // passage en section critique pour l'acces a la FIFO
        CAdeSLock lock(m_criticalSection);
        if (m_fifo.size() >= m_maxElemsInFifo)
        {
            // trop d'elements dans la FIFO
            if (deleteIfPushError)
            {
                delete pElem;
            }
            return false;
        }
        m_fifo.push_back(pElem);
        // on rajoute un jeton dans le semaphore
        ReleaseSemaphore(m_hSemaphoreCntFifo, 1, NULL);
        return true;
    }
    
    /** Dépilage d'un élément.
     *
     * @param timeoutMs[i]          timeout (en ms) d'attente d'un élément
     *                              dans la FIFO
     * @param bNoWaitHandleTest[in] flag indiquant s'il faut tester le sémaphore
     *                              ce paramètre doit être utilisé (ie true) lorsque
     *                              l'attente est faite en dehors de la fonction
     *                              (ie par un Wait direct sur le handle du sémpahore
     *                              obtenu par la méthode GetWaitHandle)
     * 
     * @retval pointeur sur l'élement dépilé, NULL si la FIFO était vide
    */
    T* Pop(const int timeoutMs = INFINITE, const bool bNoWaitHandleTest = false)
    {
        if (false == bNoWaitHandleTest)
        {
            if (WAIT_OBJECT_0 != WaitForSingleObject(m_hSemaphoreCntFifo, timeoutMs))
            {
                // pas d'elem dans la FIFO
                return NULL;
            }
        }
        // passage en section critique pour l'acces a la FIFO
        CAdeSLock lock(m_criticalSection);
        if (0 == m_fifo.size())
        {
            return NULL;
        }
        T* pRet = m_fifo.front();
        m_fifo.pop_front();
        return pRet;
    };

    /** Vidage de la FIFO (avec destruction de tous ces éléments).
    */
    void Flush(void)
    {
        // passage en section critique pour l'acces a la FIFO
        CAdeSLock lock(m_criticalSection);
        // Vidage de la FIFO
        while (0 != m_fifo.size())
        {
            delete m_fifo.front();
            m_fifo.pop_front();
        }
        // remise a 0 du compteur du semaphore
        while (WAIT_OBJECT_0 == WaitForSingleObject(m_hSemaphoreCntFifo, 0)) {}
    };

    /** Obtention du handle du sémaphore.
     * Cette méthode est à utiliser lorsqu'on souhaite dissocier l'attente
     * de la lecture d'un élément
     * 
     * @retval handle sur le sémaphore
    */
    HANDLE GetWaitHandle(void) {return m_hSemaphoreCntFifo;};
    
private:

    CAdeCriticalSection m_criticalSection;   // pour la gestion des acces concurrents à la FIFO (Push/Pop)
    HANDLE              m_hSemaphoreCntFifo; // Semaphore refletant le nombre d'elements dans la FIFO
    unsigned long       m_maxElemsInFifo;    // Nombre max d'éléments pouvant être mis dans la FIFO
    std::list<T*>       m_fifo;              // Fifo
};


#endif // ADE_FIFO_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeFifo.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
