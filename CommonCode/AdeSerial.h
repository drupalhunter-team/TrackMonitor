////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMUNICATION
/// Communication sur liaison serie
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CAdeSerial qui defini un objet de communication sur
/// liaison serie pour Win32
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeSerial.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef ADE_SERIAL_H
#define ADE_SERIAL_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

class CAdeThreadAsMember;

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////
#define ADE_SERIAL_DFLT_READ_FIFO_SIZE   100

////////////////////////////////////////////////////////////////////////////////
//                                                           Variables exportees
////////////////////////////////////////////////////////////////////////////////
// Note : PREMOD est le prefixe du module en majuscule et court

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

class CAdeSerial
{
NON_COPYABLE(CAdeSerial);
public:
    
    enum  T_ERROR{
        SERIAL_NO_ERROR,
        SERIAL_ERROR_SYSTEM,
        SERIAL_ERROR_NOTOPENED,
        SERIAL_ERROR_OPENED,
        SERIAL_ERROR_CANTOPENPORT,
        SERIAL_ERROR_INVALIDCONFIG,
        SERIAL_ERROR_INVALIDREADMODE,
    };
    
    typedef void (*T_CALLBACKRECV)(LPVOID lpParam, unsigned char ucReceived);
    typedef void (*T_CALLBACKBREAK)(LPVOID lpParam);
    typedef void (*T_CALLBACKERR)(LPVOID lpParam);
    
    enum  T_STOPBITS{
        SERIAL_ONESTOPBIT      = 0,
        SERIAL_ONE5STOPBITS    = 1,
        SERIAL_TWOSTOPBITS     = 2, 
    };
    
    enum  T_PARITY{
        SERIAL_EVENPARITY      = 0,
        SERIAL_MARKPARITY      = 1,
        SERIAL_NOPARITY        = 2,
        SERIAL_ODDPARITY       = 3,
        SERIAL_SPACEPARITY     = 4,
    };
    
    struct T_CONFIGURATION{
        char          szPort[20];
        unsigned long ulSpeed;
        unsigned char ucDataBits;
        T_STOPBITS    StopBits;
        T_PARITY      Parity;
    } ;

    CAdeSerial(unsigned long ulReadFIFOSize = ADE_SERIAL_DFLT_READ_FIFO_SIZE);
    virtual ~CAdeSerial();

    // positionnement de la configuration
    bool SetConfiguration(const T_CONFIGURATION& newConfig);
    // recuperation de la configuration
    T_CONFIGURATION GetConfiguration(void);
    
    // ouverture de la liaison
    bool Open(T_CALLBACKRECV pCallbackOnRecv = NULL, LPVOID lpParam = NULL, T_CALLBACKBREAK pCallbackOnBreak = NULL, T_CALLBACKERR pCallbackOnErr = NULL, int nPriority = THREAD_PRIORITY_NORMAL);
    // fermeture de la liaison
    bool Close();
    // lecture de caracteres sur la liaison
    bool Read(unsigned char* pucBuffer, unsigned long* pulSize, unsigned long ulTimeout, unsigned long* pulOverRun);
    // ecriture de caracteres sur la liaison
    bool Write(const unsigned char* pucBuffer, unsigned long* pulSize);

    // gestion des signaux de controle
    bool SetControlSignal(DWORD dwSignal);

    // gestion des erreurs
    T_ERROR GetLastError() { return m_LastError;};
    void SetLastError(T_ERROR LastError) { m_LastError = LastError;};
    HANDLE GethSerial() { return m_hSerial;};

protected:
    HANDLE  m_hSerial;      // handle sur le port COM
private:
    // fonction statique passe au thread de reception
    static UINT StaticProcThreadRx(LPVOID lpParam, HANDLE hEventStop);
    // corps du thread de reception
    UINT ProcThreadRx(HANDLE hEventStop);

    HANDLE  m_hSemRead;     // handle sur le semaphore indiquant le nombre de car disponibles dans la FIFO de
                            // reception

    BYTE*   m_pFIFORead;    // Fifo de reception
    BYTE*   m_pPtrReceive;  // pointeur d'ecriture dans la fifo de reception
    BYTE*   m_pPtrRead;     // pointeur de lecture dans la fifo de reception

    T_ERROR m_LastError;    // derniere erreur
    T_CALLBACKRECV  m_pCallbackOnRecv;   // pointeur sur la callback a appeler lorsqu'un caractere est recu
    T_CALLBACKBREAK m_pCallbackOnBreak;  // pointeur sur la callback a appeler sur reception d'un break
    T_CALLBACKERR   m_pCallbackOnErr;    // pointeur sur la callback a appeler sur la detection d'une erreur
    LPVOID m_lpParam;       // parametre utilisateur a passer a cette callback
    T_CONFIGURATION m_CurrentConfig;    // configuration courante

    unsigned long m_ulOverRun;              // compteur d'overun en reception
    unsigned long m_ulReadFIFOSize;         // taille de la FIFO de reception

    CAdeThreadAsMember* m_pThreadRx;        // Thread de reception
};


#endif // ADE_SERIAL_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeSerial.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
