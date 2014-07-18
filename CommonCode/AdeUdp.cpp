////////////////////////////////////////////////////////////////////////////////
/// @addtogroup COMMUNICATION
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe CAdeUdp
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 86 $
/// @date $Date: 2014-05-12 16:10:43 +0200 (lun., 12 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeUdp.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                  / Librairie)
#include <windows.h>
#include <string>
#include <strstream>
#include <boost/unique_ptr.hpp>

// Puis les include projet
#include "AdeNonCopyable.h"
#include "AdeUdp.h"
#include "AdeTrace.h"
#include "AdeThread.h"

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
/// Constructeur
////////////////////////////////////////////////////////////////////////////////
CAdeUdp::CAdeUdp():  m_lParam(NULL), m_socket(INVALID_SOCKET), m_pCallback(NULL)
{
    m_cptTxBytes  = 0;
    m_cptRxBytes  = 0;
    m_cptTxFrames = 0;
    m_cptRxFrames = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CAdeUdp::~CAdeUdp()
{
    Close();
}

////////////////////////////////////////////////////////////////////////////////
/// Ouverture d'une connexion UDP : permet de creer et d'initialiser un socket UDP
///
/// Parametres : 
/// \param ipInterface adresse ip de l'interface sur laquelle creer le socket
/// \param port        le port sur lequel doit attendre et envoyer le socket (si -1,
///                    le port est alloue dynamiquement)
/// \param pCallback   la fonction de callback a appeler lors de la reception de donnees
/// \param lparam      parametre a passer a cette callback
/// \param nPriority   priorite du thread de reception
///
/// \return 
///  UE_OK           : tout s'est bien deroule   
///  UE_CALLBACK_NULL : pas de callback passee en parametre
///  UE_WRONG_WINSOCK_VERSION : la version de winsock disponible sur le systeme 
///                             n'est pas suffisante
///  UE_SOCKET_ERROR : erreur a la creation du socket
///  UE_BIND_ERROR   : erreur lors du bind du socket 
///  UE_BROADCAST_ERROR : erreur lors du passage du socket en mode broadcast   
////////////////////////////////////////////////////////////////////////////////
CAdeUdp::T_UDP_ERROR CAdeUdp::Open(const char* ipInterface, int port/*= -1*/, T_CALLBACK pCallback/*= NULL*/, LPVOID lparam/*= NULL*/, int nPriority /*= THREAD_PRIORITY_NORMAL*/)
{
    static char szFunc[] = "CAdeUdp::Open";
    sockaddr_in local;
    char one = 1;

    if (m_pThreadRx.get())
    {
        if (m_pThreadRx->IsStarted())
        {
            // Le thread de reception est deja demarre => la connexion a deja ete ouverte
            return UE_ALREADY_OPENED;
        }
    }

    RazCpts();  // Raz des compteurs
    m_port      = port;
    m_pCallback = pCallback;
    m_lParam    = lparam;

    WORD wVersionRequested;
    WSADATA wsaData;

    // On demande a avoir la version 2.0 de Winsock
    wVersionRequested = MAKEWORD( 2, 0 );

    WSAStartup(wVersionRequested, &wsaData);

    if (LOBYTE(wsaData.wVersion) != 2 
        || HIBYTE(wsaData.wVersion) != 0 )
    {
        return UE_WRONG_WINSOCK_VERSION;
    }

    
    // Creation du socket
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (INVALID_SOCKET == m_socket)
    {
        return UE_SOCKET_ERROR;
    }

    // Bind
    memset(&local, 0, sizeof(struct sockaddr_in));
    local.sin_family        = AF_INET;
    if (!strcmp(ipInterface, ""))
    {
        local.sin_addr.s_addr   = INADDR_ANY;
        ADE_INFO(1, "IP Interface : " << INADDR_ANY << std::endl );
    }
    else
    {
        local.sin_addr.s_addr = inet_addr(ipInterface);
        ADE_INFO(1, "IP Interface : " << inet_ntoa(local.sin_addr) << std::endl );
    }
    if (port == -1)
    {
        // le port ecoute est alloue dynamiquement
        local.sin_port      = 0;
    }
    else
    {
        // le port ecoute est celui passe en parametre
        local.sin_port      = htons(m_port);
    }
    //local.sin_len           = sizeof(local);


    if (bind(m_socket, (sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
    {
        ADE_ERROR("Socket error : " << WSAGetLastError() );
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return UE_BIND_ERROR;
    }
    int nameLen = sizeof(local);
    if (getsockname(m_socket, (sockaddr *)&local, &nameLen) != SOCKET_ERROR)
    {
        // On recupere le n° du port (pour l'avoir s'il a ete alloue dynamiquement)
        m_port = ntohs(local.sin_port);
    }

    // Configuration du socket pour l'emission de paquet en broadcast
    if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one)) <0 )
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return UE_SETSOCKOPT_ERROR;
    }

    // Configuration du socket pour ne pas router (afin de forcer l'émission des paquets
    // sur l'interface physique)
    if (setsockopt(m_socket, SOL_SOCKET, SO_DONTROUTE, &one, sizeof(one)) <0 )
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return UE_SETSOCKOPT_ERROR;
    }

    // Creation et demarrage du thread de reception
    m_pThreadRx.reset(new CAdeThreadAsMember(StaticProcThreadRx, this));
    // démarrage du thread de lecture
    m_pThreadRx->BeginThread(nPriority);

    return UE_OK;
    
}

////////////////////////////////////////////////////////////////////////////////
/// Fermeture de la connexion UDP
////////////////////////////////////////////////////////////////////////////////
void CAdeUdp::Close()
{
    if (INVALID_SOCKET != m_socket)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    // destruction du thread de reception
    m_pThreadRx.reset();
}

////////////////////////////////////////////////////////////////////////////////
/// Envoie d'une trame sur la connexion UDP
///
/// Parametres : 
/// \param dest   l'adresse IP du destinataire (en format pointe)
/// \param port   port vers lequel emettre
/// \param buffer un pointeur sur les donnees a emettre
/// \param len    longueur des donnees a emettre
///
/// \return 
///  UE_OK : l'emission s'est deroulee normalement
///  UE_SEND_ERROR : une erreur est survenue 
///  UE_SOCKET_NOT_OPENED : le socket n'est pas ouvert    
////////////////////////////////////////////////////////////////////////////////
CAdeUdp::T_UDP_ERROR CAdeUdp::Send(const std::string& dest, int port, char* buffer, int len)
{
    static char szFunc[] =  "CAdeUdp::Send";
    int         nbBytesSent = 0;
    int         tmp = 0;
    T_UDP_ERROR result = UE_OK;
    sockaddr_in client;

    if (INVALID_SOCKET == m_socket)
    {
        ADE_ERROR("Socket non ouvert");
        return UE_SOCKET_NOT_OPENED;
    }
    
    memset(&client, 0, sizeof(sockaddr_in));

    client.sin_addr.s_addr = inet_addr(dest.c_str());
    client.sin_family      = AF_INET;
    client.sin_port        = htons(port);
    //client.sin_len         = sizeof(client);

    while ((nbBytesSent < len) && (tmp >=0))
    {
        tmp = sendto(m_socket, buffer + nbBytesSent, len-nbBytesSent, 0, 
                                           (sockaddr *)&client, sizeof(client));
        nbBytesSent+=tmp;
    }
    if (tmp<0)
    {
        result = UE_SEND_ERROR;
    }
    else
    {
        m_cptTxBytes += len;
        ++m_cptTxFrames;
    }

    return result;
}

// fonction statique passe au thread de reception
UINT CAdeUdp::StaticProcThreadRx(LPVOID lpParam, HANDLE hEventStop)
{
    return ((CAdeUdp*)lpParam)->ProcThreadRx(hEventStop);
}
////////////////////////////////////////////////////////////////////////////////
/// corps du thread, attend des donnees sur le socket et les transmets via le callback
/// 
/// \return code retour du thread
////////////////////////////////////////////////////////////////////////////////
// corps du thread de reception
UINT CAdeUdp::ProcThreadRx(HANDLE hEventStop)
{
    sockaddr_in client;
    int  clientLen;
    int  len;
    
    for (;;)
    {
        // On se met en attente de donnees sur le socket
        clientLen = sizeof(client);
        len = recvfrom(m_socket, m_bufferRx, sizeof(m_bufferRx), 0, (sockaddr *)&client,&clientLen);
        if (len > 0)
        {
            // On enregistre les donnees concernant le client
            int    clientPort = ntohs(client.sin_port);
            std::string clientAddr = inet_ntoa(client.sin_addr);
            m_cptRxBytes += len;
            ++m_cptRxFrames;

            // On transmet les donnees par callback
            if (m_pCallback)
            {
                m_pCallback(this, (void*)m_lParam, m_bufferRx, len, clientAddr, clientPort);
            }
        }
        else
        {
            if (len < 0)
            {
                int err = WSAGetLastError();
                if (WSAECONNRESET != err)
                {
                    // une erreur est survenue ou le socket a ete ferme
                    // Note on ne sort pas dans le cas de l'erreur WSAECONNRESET
                    return -1;
                }
            }
        }
    }    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeUdp.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
