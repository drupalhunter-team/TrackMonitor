////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMUNICATION
/// Communication sur liaison ethernet UDP
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CAdeUdp qui defini un objet de communication par
/// lien ethernet UDP (winsock)
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeUdp.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef ADE_UDP_H
#define ADE_UDP_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// Uniquement des includes projet : pas d'inclusion de fichier standard

class CAdeThreadAsMember;

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

#define RX_BUFFER_SIZE 16384

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////CByteReader
////////////////////////////////////////////////////////////////////////////////

class CAdeUdp
{
NON_COPYABLE(CAdeUdp);
public:
    typedef enum 
    {
        UE_OK,
        UE_WRONG_WINSOCK_VERSION,
        UE_SOCKET_NOT_OPENED,
        UE_SOCKET_ERROR,
        UE_BIND_ERROR,
        UE_SETSOCKOPT_ERROR,
        UE_SEND_ERROR,
        UE_CALLBACK_NULL,
        UE_ALREADY_OPENED
    }T_UDP_ERROR;

    typedef void (*T_CALLBACK)(CAdeUdp *pCnxUdp, LPVOID lpParam, char*  buffer, int len, const std::string& addrSrc, int portSrc);

    // Constructeur
    CAdeUdp();

    // Destructeur
    virtual ~CAdeUdp();

    // Permet de creer et d'initialiser le socket
    T_UDP_ERROR Open(const char* ipInterface, int port = -1, T_CALLBACK pCallback = NULL, LPVOID lparam = NULL, int nPriority = THREAD_PRIORITY_NORMAL);

    // Permet de fermer le socket
    void Close();

    // Indique si la connexion est ouverte
    bool IsOpen(void) {return (INVALID_SOCKET != m_socket) ? true : false;};

    // Permet l'emission de donnees a travers le socket UDP
    T_UDP_ERROR Send(const std::string& dest, int port, char*  buffer, int len);

    int GetPort(void) {return m_port;};

    int GetCptTxBytes(void)  {return m_cptTxBytes;};
    int GetCptRxBytes(void)  {return m_cptRxBytes;};
    int GetCptTxFrames(void) {return m_cptTxFrames;};
    int GetCptRxFrames(void) {return m_cptRxFrames;};
    void RazCpts(void){m_cptTxBytes = 0; m_cptRxBytes = 0; m_cptTxFrames = 0; m_cptRxFrames = 0;};


private:
    // fonction statique passe au thread de reception
    static UINT StaticProcThreadRx(LPVOID lpParam, HANDLE hEventStop);
    // corps du thread de reception
    UINT ProcThreadRx(HANDLE hEventStop);

    T_CALLBACK  m_pCallback;    // callback appelee sur reception d'un datagramme
    LPVOID      m_lParam;       // parametre utilisateur passe a cette callback
    SOCKET      m_socket;       // socket

    int     m_port;                      // port sur lequel on est a l'ecoute
    char    m_bufferRx[RX_BUFFER_SIZE];  // buffer de reception utilise par le thread de reception

    int     m_cptTxBytes;
    int     m_cptRxBytes;
    int     m_cptTxFrames;
    int     m_cptRxFrames;

    boost::unique_ptr<CAdeThreadAsMember> m_pThreadRx; // Thread de reception
};

#endif // ADE_UDP_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeUdp.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
