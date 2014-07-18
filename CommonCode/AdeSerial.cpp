////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMUNICATION
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe CAdeSerial (encapsulation de la liaison série pour Win32)
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 90 $
/// @date $Date: 2014-05-13 10:41:27 +0200 (mar., 13 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeSerial.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                     / Librairie)
#include <windows.h>

// Puis les include projet
#include "AdeNonCopyable.h"
#include "AdeSerial.h"
#include "AdeThread.h"

////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////

//typedef struct {
//    <type>   m1;        // commentaire
//    <type>  m2;         // commentaire
//} MON_TYPE_INTERNE;

////////////////////////////////////////////////////////////////////////////////
//                                                                     Variables
////////////////////////////////////////////////////////////////////////////////
// Note : OBLIGATOIREMENT static


static const unsigned char aWin32Parity[] =   {       
                                                    EVENPARITY,
                                                    MARKPARITY,
                                                    NOPARITY  , 
                                                    ODDPARITY , 
                                                    SPACEPARITY,
                                              };

static const unsigned char aWin32StopBits[] = {       
                                                    ONESTOPBIT  ,
                                                    ONE5STOPBITS,
                                                    TWOSTOPBITS , 
                                              };


// configuration par defaut
static const CAdeSerial::T_CONFIGURATION DEFAULT_CONFIGURATION = 
            { "COM1"                            // COM1
            , 38400                              // 38400 bps
            , 8                                  // 8 data bits
            , CAdeSerial::SERIAL_ONESTOPBIT   // 1 stop bit
            , CAdeSerial::SERIAL_NOPARITY     // Parity=none
            };



////////////////////////////////////////////////////////////////////////////////
//                                             Prototypes des fonctions internes
////////////////////////////////////////////////////////////////////////////////
// Note : OBLIGATOIREMENT static

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// Constructeur
///
/// Parametres :
/// \param ulReadFIFOSize  taille de la FIFO de reception si utilisation de ce mode
////////////////////////////////////////////////////////////////////////////////
CAdeSerial::CAdeSerial(unsigned long ulReadFIFOSize /* = ADE_SERIAL_DFLT_READ_FIFO_SIZE */) :
    m_hSerial(INVALID_HANDLE_VALUE),
    m_hSemRead(NULL),
    m_pFIFORead(NULL),
    m_pPtrReceive(NULL),
    m_pPtrRead(NULL),
    m_ulOverRun(0),
    m_pThreadRx(NULL),
    m_LastError(SERIAL_NO_ERROR),
    m_pCallbackOnRecv(NULL),
    m_pCallbackOnBreak(NULL),
    m_pCallbackOnErr(NULL),
    m_lpParam(NULL),
    m_CurrentConfig(DEFAULT_CONFIGURATION),
    m_ulReadFIFOSize(ulReadFIFOSize)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CAdeSerial::~CAdeSerial()
{
    if (m_hSerial != INVALID_HANDLE_VALUE)
    {
        Close();
    }
}
////////////////////////////////////////////////////////////////////////////////
/// Obtention de la config courante
//
/// Parametres :
//
/// \return configuration courante
////////////////////////////////////////////////////////////////////////////////
CAdeSerial::T_CONFIGURATION CAdeSerial::GetConfiguration(void)
{
    return m_CurrentConfig;
}

////////////////////////////////////////////////////////////////////////////////
/// Modification de la config courante
///
/// Parametres :
/// \param pSrcConfig  structure contenant la config à écrire
///
/// \return false si déjà ouvert (SERIAL_ERROR_OPENED) true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeSerial::SetConfiguration(const T_CONFIGURATION& newConfig)
{
    bool bResult;
    if (m_hSerial == INVALID_HANDLE_VALUE)
    {
        // port fermé. On peut modifier les settings
        m_CurrentConfig = newConfig;
        bResult = true;
    }
    else
    {
        bResult = false;
        m_LastError = SERIAL_ERROR_OPENED;
    }

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
/// ouverture de la connexion serie
///
/// Parametres :
/// \param pCallbackOnRecv ptr sur la callback a appeler sur réception. Si NULL,
///                        fonctionnement par FIFO tournante
/// \param lpParam   parametre utilisateur a passer a cette callback
/// \param pCallbackOnBreak ptr sur la callback a appeler sur réception d'un break. Si NULL,
///                        pas de gestion du break
/// \param nPriority       priorite du thread de reception
///
/// \return false si 
///                déjà ouvert (SERIAL_ERROR_OPENED)
///                Erreur de connexion(SERIAL_ERROR_CANTOPENPORT)
///                Configuration invalide(SERIAL_ERROR_INVALIDCONFIG)
///                Erreur Système(SERIAL_ERROR_SYSTEM)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeSerial::Open(T_CALLBACKRECV pCallbackOnRecv /*= NULL*/, LPVOID lpParam /*= NULL*/, T_CALLBACKBREAK pCallbackOnBreak /*= NULL*/, T_CALLBACKERR pCallbackOnErr /*= NULL*/, int nPriority /*= THREAD_PRIORITY_NORMAL*/)
{
    bool bResult = true;

    m_pCallbackOnRecv = pCallbackOnRecv;
    m_lpParam = lpParam;
    m_pCallbackOnBreak = pCallbackOnBreak;
    m_pCallbackOnErr = pCallbackOnErr;

    // vérif si port déja ouvert
    if (m_hSerial != INVALID_HANDLE_VALUE)
    {
        // port ouvert. On ne peut aller plus loin
        bResult = false;

        m_LastError = SERIAL_ERROR_OPENED;
    }

    // ouverture du port
    if (bResult)
    {
        // on fonctionne en Overlap pour permettre les opérations de lecture et d'écriture
        // simultanées
        // Application de http://support.microsoft.com/kb/115831/en-us pour pouvoir ouvrir des ports
        // à partir de COM10
        std::string strPort("\\\\.\\");
        strPort += m_CurrentConfig.szPort;
        m_hSerial = CreateFile(  strPort.c_str()
                               , GENERIC_READ | GENERIC_WRITE
                               , 0
                               , NULL
                               , OPEN_EXISTING
                               , FILE_FLAG_OVERLAPPED
                               , NULL
                              );
        if (m_hSerial == INVALID_HANDLE_VALUE)
        {
            // impossible de se connecter
            bResult = false;
            m_LastError = SERIAL_ERROR_CANTOPENPORT;
        }
    }

    // positionnement des prm de comm
    if (bResult)
    {
        DCB dcb;
        GetCommState(m_hSerial, &dcb);

        dcb.BaudRate = m_CurrentConfig.ulSpeed;
        dcb.fParity = (m_CurrentConfig.Parity == SERIAL_NOPARITY ? false : true);
        dcb.Parity = aWin32Parity[m_CurrentConfig.Parity];
        dcb.StopBits = aWin32StopBits[m_CurrentConfig.StopBits];
        dcb.ByteSize = m_CurrentConfig.ucDataBits;
        dcb.fOutxCtsFlow = 0;
        dcb.fOutxDsrFlow = 0;
        dcb.fDtrControl  = 1;
        dcb.fDsrSensitivity = 0;
        dcb.fTXContinueOnXoff = 1;
        dcb.fOutX = 0;
        dcb.fInX = 0;
        dcb.fNull = 0;
        dcb.fRtsControl = 1;
        
        if (SetCommState(m_hSerial, &dcb) == 0)
        {
            // erreur de positionnement de paramètres
            bResult = false;
            m_LastError = SERIAL_ERROR_INVALIDCONFIG;
            Close();
        }
    }

    // positionnement des timeouts
    if (bResult)
    {
        COMMTIMEOUTS Timeouts;
        GetCommTimeouts(m_hSerial, &Timeouts);

        // read non bloquant
        Timeouts.ReadIntervalTimeout = MAXDWORD;
        Timeouts.ReadTotalTimeoutConstant = 0;
        Timeouts.ReadTotalTimeoutMultiplier = 0;

        //write bloquant
        Timeouts.WriteTotalTimeoutConstant = 0;
        Timeouts.WriteTotalTimeoutMultiplier = 0;

        if (SetCommTimeouts(m_hSerial, &Timeouts) == 0)
        {
            // erreur de positionnement de paramètres
            bResult = false;
            m_LastError = SERIAL_ERROR_INVALIDCONFIG;
            Close();
        }
    }

    // initialisation des données internes
    if (bResult)
    {
        if (pCallbackOnRecv == NULL)
        {
            // pas de callback. utilisation de la FIFO tournante
            m_hSemRead = ::CreateSemaphore(NULL, 0, m_ulReadFIFOSize, NULL);
            if (m_hSemRead == NULL)
            {
                // erreur de positionnement de paramètres
                bResult = false;
                m_LastError = SERIAL_ERROR_INVALIDCONFIG;
                Close();
            }
            else
            {
                m_pFIFORead = new unsigned char[m_ulReadFIFOSize];
                m_pPtrReceive = m_pFIFORead;
                m_pPtrRead = m_pFIFORead;
                m_ulOverRun = 0;
            }
        }
    }

    if (bResult)
    {
        // Positionnement du masque d'evenement
        SetCommMask(m_hSerial, EV_BREAK | EV_RXCHAR | EV_ERR);

        // Creation et demarrage du thread de reception
        m_pThreadRx = new CAdeThreadAsMember(StaticProcThreadRx, this);
        // démarrage du thread de lecture
        m_pThreadRx->BeginThread(nPriority);
    }

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
/// Fermeture de la connexion serie
///
/// \return false si pas ouvert (SERIAL_ERROR_NOTOPENED), true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeSerial::Close()
{
    // vérif si port déja ouvert
    if (m_hSerial == INVALID_HANDLE_VALUE)
    {
        // port fermé. On ne peut aller plus loin
        return false;
    }

    // arret du thread de lecture si le thread a ete cree
    if (NULL != m_pThreadRx)
    {
        m_pThreadRx->SendStop();
    }

    // On repositionne le mask de façon a sortir d'un WaitCommEvent s'il y en a un en cours
    SetCommMask(m_hSerial, EV_RXCHAR);

    // attente de l'arret effectif et destruction
    if (NULL != m_pThreadRx)
    {
        m_pThreadRx->WaitTerminate(100);
        delete m_pThreadRx;
        m_pThreadRx = NULL;
    }

    //fermeture du port
    CloseHandle(m_hSerial);
    m_hSerial = INVALID_HANDLE_VALUE;
    
    // ménage des données internes
    if (m_pFIFORead != NULL)
    {
        delete [] m_pFIFORead;
        m_pFIFORead = NULL;
    }
    m_pPtrRead = NULL;
    m_pPtrReceive = NULL;
    
    if (m_hSemRead != NULL)
    {
        CloseHandle(m_hSemRead);
        m_hSemRead = NULL;
    }

    m_pCallbackOnRecv = NULL;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// accès a la FIFO de lecture en mode temporisé
///
/// Parametres :
/// \param pucBuffer    Buffer de retour des données
/// \param pulSize      taille du buffer en entrée. nb octets lus en sortie
/// \param ulTimeout    timeout de lecture en ms
/// \param pulOverRun   remonte le nombre d'écrasement de caractère dans la FIFO
///
/// \return false si 
///                pas ouvert (SERIAL_ERROR_NOTOPENED)
///                pas en mode lecture temporisé (SERIAL_ERROR_INVALIDREADMODE)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeSerial::Read(unsigned char* pucBuffer, unsigned long* pulSize, unsigned long ulTimeout, unsigned long* pulOverRun)
{
    bool bResult = true;

    unsigned long ulSizeBuf = *pulSize;
    DWORD dwStartTime = GetTickCount();
    DWORD TimeToWait;
    bool bTimeout = false;

    *pulSize = 0;
    if (pulOverRun)
    {
        *pulOverRun = 0;
    }

    if (m_hSerial == INVALID_HANDLE_VALUE)
    {
        // port fermé. On ne peut aller plus loin
        bResult = false;
        m_LastError = SERIAL_ERROR_NOTOPENED;
    }

    if (bResult)
    {
        if (m_hSemRead == NULL)
        {
            // on est en mode callback. Fonction inutilisable
            bResult = false;
            m_LastError = SERIAL_ERROR_INVALIDREADMODE;
        }
    }

    if (bResult)
    {
        while ((*pulSize < ulSizeBuf) && (!bTimeout))
        {
            TimeToWait = (GetTickCount()-dwStartTime);
            if (TimeToWait > ulTimeout)
            {
                // on a atteint le timeout global : on fait une dernière lecture
                // et on sort
                TimeToWait = 0;
                bTimeout = true;
            }
            else
            {
                TimeToWait = ulTimeout - TimeToWait;
            }

            if (::WaitForSingleObject(m_hSemRead, TimeToWait) != WAIT_OBJECT_0)
            {
                // timeout ou erreur : on sort en timeout
                bTimeout = true;
            }
            else
            {
                // un octet à lire dans la FIFO
                pucBuffer[*pulSize] = *m_pPtrRead;
                ++m_pPtrRead;
                ++(*pulSize);

                // reset du pointeur de lecture dans le buffer cyclique
                if ((unsigned long)(m_pPtrRead - m_pFIFORead) >= m_ulReadFIFOSize)
                {
                    m_pPtrRead = m_pFIFORead;
                }
            }
        }
        // dans tous les cas, on indique l'overrun courant
        if (pulOverRun)
        {
            *pulOverRun = m_ulOverRun;
        }
    }

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
/// Ecriture sur la connexion serie
///
/// Parametres :
/// \param pucBuffer    Buffer des données à écrire
/// \param pulSize      taille du buffer en entrée. nb octets ecrits en sortie
///
/// \return false si 
///                pas ouvert (SERIAL_ERROR_NOTOPENED)
///                erreur systeme (SERIAL_ERROR_SYSTEM)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeSerial::Write(const unsigned char* pucBuffer, unsigned long* pulSize)
{
    DWORD dwRes;
    bool bResult = true;
    unsigned long ulSizeBuf = *pulSize;
    OVERLAPPED  Overlap;
    Overlap.hEvent = CreateEvent(NULL, true, false, NULL);
    Overlap.Offset = 0;
    Overlap.OffsetHigh = 0;
    DWORD dwLastError;

    if (m_hSerial == INVALID_HANDLE_VALUE)
    {
        // port fermé. On ne peut aller plus loin
        bResult = false;
        m_LastError = SERIAL_ERROR_NOTOPENED;
    }

    if (bResult)
    {
        if (WriteFile(m_hSerial, pucBuffer, ulSizeBuf, pulSize, &Overlap) == 0)
        {
            // vérification si IO Pending
            dwLastError = ::GetLastError();
            if ( dwLastError == ERROR_IO_PENDING)
            {
                // Write is pending.
                dwRes = WaitForSingleObject(Overlap.hEvent, INFINITE);
                switch(dwRes)
                {
                    // OVERLAPPED structure's event has been signaled. 
                    case WAIT_OBJECT_0:
                        if (!GetOverlappedResult(m_hSerial, &Overlap, pulSize, false))
                        {
                            bResult = false;
                            m_LastError = SERIAL_ERROR_SYSTEM;
                        }
                        else
                        {
                            // Write operation completed successfully.
                            //fRes = true;
                        }
                        break;
                    
                    default:
                        // An error has occurred in WaitForSingleObject.
                        // This usually indicates a problem with the
                        // OVERLAPPED structure's event handle.
                        bResult = false;
                        m_LastError = SERIAL_ERROR_SYSTEM;
                        break;
                }
            }
            else
            {
                // c'est une vraie erreur : on la repositionne car le GetLastError
                // l'a réinitialisée
                bResult = false;
                ::SetLastError (dwLastError);
                m_LastError = SERIAL_ERROR_SYSTEM;
            }
        }
    }

    CloseHandle(Overlap.hEvent );
    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
/// PILOTAGE MANUEL SIGNAUX DE CONTROLE
///
/// Parametres :
/// \param dwSignal : code de controle (signal + etat)
///
/// \return false si 
///                pas ouvert (SERIAL_ERROR_NOTOPENED)
///                erreur systeme (SERIAL_ERROR_SYSTEM)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeSerial::SetControlSignal(DWORD dwSignal)
{
    bool bResult = true;
    DWORD dwLastError;

    if (m_hSerial == INVALID_HANDLE_VALUE)
    {
        // port fermé. On ne peut aller plus loin
        bResult = false;
        m_LastError = SERIAL_ERROR_NOTOPENED;
    }

    if (bResult)
    {
        if (EscapeCommFunction(m_hSerial, dwSignal) == 0)
        {
            // vérification si IO Pending
            dwLastError = ::GetLastError();
            // c'est une vraie erreur : on la repositionne car le GetLastError
            // l'a réinitialisée
            bResult = false;
            ::SetLastError (dwLastError);
            m_LastError = SERIAL_ERROR_SYSTEM;
        }
    }
    return bResult;
}
////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions internes
////////////////////////////////////////////////////////////////////////////////

// fonction statique passe au thread de reception
UINT CAdeSerial::StaticProcThreadRx(LPVOID lpParam, HANDLE hEventStop)
{
    return ((CAdeSerial*)lpParam)->ProcThreadRx(hEventStop);
}
////////////////////////////////////////////////////////////////////////////////
/// thread de lecture
///
/// \return code retour du thread
///////////////////////////////////////////////////////////////////////////////
// corps du thread de reception
UINT CAdeSerial::ProcThreadRx(HANDLE hEventStop)
{
    unsigned char ucRead;
    unsigned long ulNbRead;
    unsigned long ulUnused;
    long lPreviousCount;
    OVERLAPPED  Overlap;

    Overlap.hEvent = CreateEvent(NULL, true, false, NULL);
    Overlap.Offset = 0;
    Overlap.OffsetHigh = 0;

    while (WaitForSingleObject(hEventStop, 0) == WAIT_TIMEOUT)
    {
        DWORD EvtMask = 0;
        if (WaitCommEvent(m_hSerial, &EvtMask, &Overlap) == 0)
        {
            // vérification si IO Pending
            if (::GetLastError() == ERROR_IO_PENDING)
            {
                if (GetOverlappedResult(m_hSerial, &Overlap, &ulUnused, true))
                {
                    // fin de traitement overlap
                }
            }
        }
        if (EvtMask == 0)
        {
            // Un SetMask a ete fait alors qu'on etait dans le wait => a priori, c'est la
            // fin du thread
        }
        if (EvtMask & EV_ERR)
        {
            // erreur
            if (m_pCallbackOnErr)
            {
                // On notifie a l'appelant
                m_pCallbackOnErr(m_lpParam);
            }
        }
        if (EvtMask & EV_BREAK)
        {
            // Break sur l'input => il faut arreter
            if (m_pCallbackOnBreak)
            {
                // On notifie a l'appelant
                m_pCallbackOnBreak(m_lpParam);
            }
        }
        if (EvtMask & EV_RXCHAR)
        {
            // Caractere reçu
            // Boucle de lecture des caracteres recu (lecture non bloquante)
            do
            {
                if (ReadFile(m_hSerial, &ucRead, 1, &ulNbRead, &Overlap) == 0)
                {
                    // vérification si IO Pending
                    if (::GetLastError() == ERROR_IO_PENDING)
                    {
                        if (GetOverlappedResult(m_hSerial, &Overlap, &ulNbRead, true))
                        {
                            // fin de traitement overlap
                        }
                        else
                        {
                            // une erreur
                            ulNbRead = 0;
                        }

                    }
                    else
                    {
                        // c'est une vraie erreur
                        ulNbRead = 0;
                    }
                }

                // si on a recu des données
                if (ulNbRead != 0)
                {
                    // un caractère recu
                    if (m_pCallbackOnRecv == NULL)
                    {
                        // mode FIFO
                        *m_pPtrReceive = ucRead;
                        // on n'incrémente la position que si la FIFO n'est pas saturée
                        if (ReleaseSemaphore(m_hSemRead, 1, &lPreviousCount) == 0)
                        {
                            // enregistrer le dépassement
                            ++m_ulOverRun;
                        }
                        else
                        {
                            // on peut incrémenter la position courante
                            ++m_pPtrReceive;
                            // reset du pointeur de réception dans le buffer cyclique
                            if ((unsigned long)(m_pPtrReceive - m_pFIFORead) >= m_ulReadFIFOSize)
                            {
                                m_pPtrReceive = m_pFIFORead;
                            }
                        }
                    }
                    else
                    {
                        // appel de la callback
                        m_pCallbackOnRecv(m_lpParam, ucRead);
                    }
                }
            } while (ulNbRead != 0);
        }
    }

    CloseHandle(Overlap.hEvent);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeSerial.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
