////////////////////////////////////////////////////////////////////////////////
/// @addtogroup THREAD
/// Encapsulation des threads windows
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CAdeThread (virtuelle pure) qui encapsule les winthreads
/// Definition de la classe CAdeThreadPer (virtuelle pure) qui permet de definir un thread periodique
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeThread.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

#ifndef ADE_THREAD_H
#define ADE_THREAD_H

#include "AdeFifo.h"

class CAdeThread
{
NON_COPYABLE(CAdeThread);
public:
    // corps du thread
    virtual UINT BodyThread(void) = 0;        

    CAdeThread();
    virtual ~CAdeThread();
    // demarrage du thread
    void BeginThread(int nPriority = THREAD_PRIORITY_HIGHEST);
    // envoi de l'evenement d'arret
    void SendStop(void);
    // attente de la fin du thread
    void WaitTerminate(DWORD timeoutMs = INFINITE);
    // demande de l'arret du thread avec attente de sa fin
    void StopThread(DWORD timeoutMs = INFINITE) { SendStop(); WaitTerminate(timeoutMs);};
    // le thread est-il en fonctionnement
    bool IsStarted(void) {return (m_hThread != NULL ? true : false);}

protected:
    HANDLE m_hEventStop;        // Evenement signale pour l'arret du thread
    HANDLE m_hThread;           // handle du thread
private:
    static DWORD WINAPI ProcThread(LPVOID pParam);
};


// definition d'un thread periodique
class CAdeThreadPer : public CAdeThread
{
NON_COPYABLE(CAdeThreadPer);
public:
    CAdeThreadPer() {};
    // note : comme BodyThread est une virtuelle pure dans CAdeThread, on est obligé d'arrêter le thread
    // dans le destructeur de la classe dérivée car sinon, on pouvait tomber sur des cas bizarre du type
    // 1. on démarre un thread  via BeginThread avec l'appelant (createur) de plus haute priorité que le thread créé)
    // 2. on le détruit sans qu'il est réellement commencé
    //    => quand le StopThread était fait dans le destructeur de CAdeThread, on se retrouvait avec
    //       l'objet partiellement détruit (destructeur de la classe dérivée appelée) et en attente de l'arrêt du
    //       thread => la fonction ProcThread essayait alors d'éxécuter la fonction virtuelle sur un objet partiellement
    //       détruit ce qui provoquait un beau plantage !
    virtual ~CAdeThreadPer() {StopThread();};
    // fonction qui sera appelee periodiquement
    // si elle renvoie une valeur differente de 0
    // on arrete le thread
    virtual UINT PeriodicFct(LPVOID lpParam) = 0;

    // corps du thread
    virtual UINT BodyThread(void);

    // fct utilisee par l'objet manager 
    void BeginThreadPer(DWORD perMs, int nPriority = THREAD_PRIORITY_HIGHEST, LPVOID lParam = NULL)
    {
        // on stocke la periode d'appel et le parametre utilisateur
        m_perMs = perMs;
        m_lParam = lParam;
        // et on lance le thread
        BeginThread(nPriority);
    };
private:
    DWORD  m_perMs;                 // periode en ms
    LPVOID m_lParam;                // parametre utilisateur passe a la fonction appelee periodiquement
};

// definition d'un thread a qui on differe l'execution de procedure
// DPC = Defered Procedure Call
class CAdeThreadExecDPC : public CAdeThread
{
NON_COPYABLE(CAdeThreadExecDPC);
    // definition du prototypage d'une DPC qui sera appelee par le thread
    // lpObject => pointeur sur un objet qui sera dereferencee dans la DPC (mais peut
    // servir a autre chose)
    // lpParam => pointeur sur des donnees
    // ulParam => long additionnel
    typedef void (*T_DPC)(LPVOID lpObject, LPVOID lpParam, unsigned long ulParam);
    class CDpcRecord
    {
    public:
        T_DPC           m_pDpc;
        LPVOID          m_lpObject;
        LPVOID          m_lpParam;
        unsigned long   m_ulParam;
    };
public:
    CAdeThreadExecDPC() {};
    // cf explication dans ~CAdeThreadPer
    virtual ~CAdeThreadExecDPC() {StopThread();};

    // met dans la file des DPC a executer
    bool QueueDPC(T_DPC pDpc, LPVOID lpObject, LPVOID lpParam, unsigned long ulParam);

public:
    virtual UINT BodyThread(void);
private:
    CAdeFifo<CDpcRecord> m_fifoDPC;                  // fifo des DPC
};


// definition d'un thread destine a etre utilise comme variable membre d'une classe
// (Composition plutot qu'heritage)
class CAdeThreadAsMember : public CAdeThread
{
NON_COPYABLE(CAdeThreadAsMember);
public:
    typedef UINT (*T_THREAD_BODY)(LPVOID lpParam, HANDLE hEventStop);
    CAdeThreadAsMember(T_THREAD_BODY threadBody, LPVOID lParam)
        : m_threadBody(threadBody), m_lParam(lParam) {};
    // cf explication dans ~CAdeThreadPer
    virtual ~CAdeThreadAsMember() {StopThread();};
private:
    // corps du thread
    virtual UINT BodyThread(void)
    {
        if (m_threadBody)
        {
            // on appelle la fonction en lui passant en parametre
            // le handle de l'evenement qui est positionne quand on
            // demande le stop
            return m_threadBody(m_lParam, m_hEventStop);
        }
        else
        {
            return 0;
        }
    }
    T_THREAD_BODY m_threadBody;  // pointeur sur la fonction qui est le corps du thread
    LPVOID        m_lParam;      // parametre utilisateur passe a cette fonction
};

// definition d'un thread appelant peridiquement une fonction
// il est destine a etre utilise comme variable membre d'une classe
// (Composition plutot qu'heritage)
class CAdeThreadPerAsMember : public CAdeThreadPer
{
NON_COPYABLE(CAdeThreadPerAsMember);
public:
    typedef INT (*T_FCT_PER)(LPVOID lpParam);
    CAdeThreadPerAsMember(T_FCT_PER fctPer, LPVOID lParam)
        : m_fctPer(fctPer), m_lParam(lParam) {};
    // cf explication dans ~CAdeThreadPer
    virtual ~CAdeThreadPerAsMember() {StopThread();};
private:
    // fonction qui sera appelee periodiquement
    // si elle renvoie une valeur differente de 0
    // on arrete le thread
    virtual UINT PeriodicFct(LPVOID lpParam)
    {
        if (m_fctPer)
        {
            // on appelle la fonction periodique
            return m_fctPer(m_lParam);
        }
        else
        {
            return 0;
        }
    }
    T_FCT_PER m_fctPer;  // pointeur sur la fonction qui sera appellee periodiquement
    LPVOID    m_lParam;  // parametre utilisateur passe a cette fonction
};
#endif // ADE_THREAD_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeThread.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
