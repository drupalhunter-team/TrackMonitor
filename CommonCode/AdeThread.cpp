////////////////////////////////////////////////////////////////////////////////
/// @addtogroup THREAD
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe CAdeThread (encapsulation threads windows)
/// et la classe CAdeThreadPer (specialisation de CAdeThread) pour definir des
/// threads periodiques
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeThread.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include "AdeGenMacros.h"
#include "AdeNonCopyable.h"
#include "AdeThread.h"

/////////////////////////////////////////////////////////////////////////////
// Procedure de demarrage du thread (param de CreateThread)
// methode statique
/////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CAdeThread::ProcThread(LPVOID pParam) 
{
    CAdeThread * pThread = (CAdeThread*) pParam;
    DWORD cr = pThread->BodyThread();
    return cr;
}

/////////////////////////////////////////////////////////////////////////////
// Constructeur
/////////////////////////////////////////////////////////////////////////////
CAdeThread::CAdeThread()
{
    m_hThread    = NULL;
    m_hEventStop = ::CreateEvent(NULL, false, false, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Destructeur
/////////////////////////////////////////////////////////////////////////////
CAdeThread::~CAdeThread()
{
    CloseHandle(m_hEventStop);
}

/////////////////////////////////////////////////////////////////////////////
/// Demarrage du thread
///
/// Parametres :
/// \param nPriority    priorite du thread
/////////////////////////////////////////////////////////////////////////////
void CAdeThread::BeginThread(int nPriority)
{
    if (NULL != m_hThread)
    {
        // thread en cours d'execution
        return;
    }

    DWORD dwThreadId;
    // Demarrage du thread
    m_hThread = ::CreateThread( NULL
                              , 0
                              , ProcThread
                              , this
                              , CREATE_SUSPENDED
                              , &dwThreadId
                              );
    ::SetThreadPriority(m_hThread, nPriority);

    // Activation du thread
    ::ResumeThread(m_hThread);
}
/////////////////////////////////////////////////////////////////////////////
/// Demande d'arret du thread
/////////////////////////////////////////////////////////////////////////////
void CAdeThread::SendStop(void)
{
    if (NULL == m_hThread)
    {
        return;
    }
    // Envoi de l'evenement d'arret
    SetEvent(m_hEventStop);

}
/////////////////////////////////////////////////////////////////////////////
/// Attente de l'arret du thread
///
/// Parametres :
/// \param timeoutMs    timeout d'attente de l'arret du thread en ms
/////////////////////////////////////////////////////////////////////////////
void CAdeThread::WaitTerminate(DWORD timeoutMs /*= INFINITE*/)
{
    if (NULL == m_hThread)
    {
        return;
    }
    // Attente de l'arret effectif du thread
    WaitForSingleObject(m_hThread, timeoutMs);
    CloseHandle(m_hThread);
    m_hThread = NULL;
}


// Class CAdeThreadPer
/////////////////////////////////////////////////////////////////////////////
// Corps du thread pour un thread periodique
/////////////////////////////////////////////////////////////////////////////
UINT CAdeThreadPer::BodyThread(void)
{
    HANDLE hEventPer = CreateEvent(NULL, false, false, NULL);
    HANDLE arrEvent[2];
    arrEvent[0] = m_hEventStop;
    arrEvent[1] = hEventPer;

    // creation du timer periodique
    UINT idMMTimer = timeSetEvent(m_perMs, 1, (LPTIMECALLBACK )hEventPer, 0 , TIME_PERIODIC | TIME_CALLBACK_EVENT_SET|TIME_KILL_SYNCHRONOUS);
    while (1)
    {
        DWORD dw = WaitForMultipleObjects(NB_ELEMS_OF(arrEvent), arrEvent, false, INFINITE);
        if (dw == WAIT_OBJECT_0)
        {
            // demande d'arret => on sort
            timeKillEvent(idMMTimer);
            CloseHandle(hEventPer);
            return 0;
        }
        else if (dw == WAIT_OBJECT_0 + 1)
        {
            // appel de la fonction periodique
            if (PeriodicFct(m_lParam) != 0)
            {
                // demande d'arret => on sort
                timeKillEvent(idMMTimer);
                CloseHandle(hEventPer);
                return 0;
            }
        }
        else
        {
            dw = GetLastError();
        }
    }
    return 0;
}

// Class CAdeThreadExecDPC
/////////////////////////////////////////////////////////////////////////////
/// Envoie d'une requete d'execution d'une DPC
///
/// Parametres :
/// \param timeoutMs    timeout d'attente de l'arret du thread en ms
/////////////////////////////////////////////////////////////////////////////
bool CAdeThreadExecDPC::QueueDPC(T_DPC pDpc, LPVOID lpObject, LPVOID lpParam, unsigned long ulParam)
{
    CDpcRecord* pDpcRecord = new CDpcRecord;
    pDpcRecord->m_pDpc     = pDpc;
    pDpcRecord->m_lpObject = lpObject;
    pDpcRecord->m_lpParam  = lpParam;
    pDpcRecord->m_ulParam  = ulParam;
    return m_fifoDPC.Push(pDpcRecord, true); // detruit l'objet si l'insertion echoue
}
/////////////////////////////////////////////////////////////////////////////
// Corps du thread pour un thread qui execute des DPC
/////////////////////////////////////////////////////////////////////////////
UINT CAdeThreadExecDPC::BodyThread(void)
{
    HANDLE arrEvent[2];
    arrEvent[0] = m_hEventStop;
    arrEvent[1] = m_fifoDPC.GetWaitHandle();

    while (1)
    {
        DWORD dw = WaitForMultipleObjects(NB_ELEMS_OF(arrEvent), arrEvent, false, INFINITE);
        if (dw == WAIT_OBJECT_0)
        {
            // demande d'arret => on sort
            return 0;
        }
        else if (dw == WAIT_OBJECT_0 + 1)
        {
            // Une DPC au moins a executer
            CDpcRecord* pDpcRecord =  m_fifoDPC.Pop(0, true); // sans timeout et sans test du semaphore
            if (pDpcRecord)
            {
                // execution de la DPC
                pDpcRecord->m_pDpc(pDpcRecord->m_lpObject, pDpcRecord->m_lpParam, pDpcRecord->m_ulParam);
                delete pDpcRecord;
            }
        }
        else
        {
            dw = GetLastError();
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeThread.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
