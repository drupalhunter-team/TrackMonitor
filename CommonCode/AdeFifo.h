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

// Pour se proteger de la multi-inclusion : ADE_FIFO_H doit �tre unique
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
 *  La specificit� de cette classe par rapport � une FIFO bas�e sur std::list
 *  ou std::deque est qu'un semaphore est associ� � la FIFO : le nombre de
 *  jetons dans le s�maphore refl�te le nombre d'�l�ments dans la FIFO.
 *  L'utilisation de ce s�maphore permet de faire une lecture bloquante avec
 *  gestion d'un timeour dans cette FIFO.
 * 
 *  Note : une fois un �l�ment ins�r� dans la FIFO (par son pointeur), la FIFO
 *  devient la propri�taire de l'objet et il sera d�truit lors de la destruction
 *  de la FIFO ou lors d'un vidage de la FIFO (flush).
 *
 *  Note : il est possible de r�cup�rer le handle du s�maphore pour faire une
 *  attente directe sur ce s�maphore dans un thread, dans ce cas, il faut ensuite
 *  ensuite demander une lecture (Pop) sans faire de wait sur le handle afin
 *  de garder la FIFO et le s�maphore synchronis�s.
*/
 template <class T>
class CAdeFifo
{
NON_COPYABLE(CAdeFifo);
#define ADE_FIFO_MAX_ELEM_DEFAULT   1000
public:
    /** Constructeur.
     * @param maxElemsInFifo[in]  nombre max d'�l�ments pouvant �tre mis dans la FIFO
     *                            (n�c�ssaire pour initialiser la capacit� max du s�maphore en jetons)
    */
    CAdeFifo(const unsigned long maxElemsInFifo = ADE_FIFO_MAX_ELEM_DEFAULT) : m_maxElemsInFifo(maxElemsInFifo)
    {
        // creation du semaphore associe (nombre d'unite = nombre d'elements dans la FIFO)
        m_hSemaphoreCntFifo = ::CreateSemaphore(NULL, 0, m_maxElemsInFifo, NULL);
    };
    /** Destructeur : on vide la FIFO en d�truisant tous les �l�ments.
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
    
    /** Empilage d'un �l�ment dans la FIFO.
     *
     * @param pElem[in]             pointeur sur l'element � mettre dans la FIFO
     * @param deleteIfPushError[in] flag indiquant s'il faut d�truire l'�l�ment si on
     *                              ne peut pas l'inserer
     * 
     * @retval true si l'�l�ment a �t� ajout� dans la FIFO, false sinon
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
    
    /** D�pilage d'un �l�ment.
     *
     * @param timeoutMs[i]          timeout (en ms) d'attente d'un �l�ment
     *                              dans la FIFO
     * @param bNoWaitHandleTest[in] flag indiquant s'il faut tester le s�maphore
     *                              ce param�tre doit �tre utilis� (ie true) lorsque
     *                              l'attente est faite en dehors de la fonction
     *                              (ie par un Wait direct sur le handle du s�mpahore
     *                              obtenu par la m�thode GetWaitHandle)
     * 
     * @retval pointeur sur l'�lement d�pil�, NULL si la FIFO �tait vide
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

    /** Vidage de la FIFO (avec destruction de tous ces �l�ments).
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

    /** Obtention du handle du s�maphore.
     * Cette m�thode est � utiliser lorsqu'on souhaite dissocier l'attente
     * de la lecture d'un �l�ment
     * 
     * @retval handle sur le s�maphore
    */
    HANDLE GetWaitHandle(void) {return m_hSemaphoreCntFifo;};
    
private:

    CAdeCriticalSection m_criticalSection;   // pour la gestion des acces concurrents � la FIFO (Push/Pop)
    HANDLE              m_hSemaphoreCntFifo; // Semaphore refletant le nombre d'elements dans la FIFO
    unsigned long       m_maxElemsInFifo;    // Nombre max d'�l�ments pouvant �tre mis dans la FIFO
    std::list<T*>       m_fifo;              // Fifo
};


#endif // ADE_FIFO_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeFifo.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
