////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CNX
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe CCnxUDP
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 90 $
/// @date $Date: 2014-05-13 10:41:27 +0200 (mar., 13 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxUdp.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// Puis les include projet
#include "Utils.h"
#include "CnxUdp.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                     / Librairie)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Constructeur
////////////////////////////////////////////////////////////////////////////////
CCnxUDP::CCnxUDP(void)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CCnxUDP::~CCnxUDP()
{
    Close();
}


////////////////////////////////////////////////////////////////////////////////
/// Ouverture de la connexion
///
/// Parametres :
///
/// \return true si l'ouverture s'est bien deroulee, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CCnxUDP::Open(const std::string& localIP, int localPort, const std::string& destIP, int destPort)
{
    char szFunc[] = "CCnxUDP::Open";

    // cas d'un serveur, le port sur lequel il ecoute est alloue
    // statiquement
    m_lastError = m_AdeUdp.Open(localIP.c_str(), localPort, StaticCallbackUDP, (LPVOID)this);

    m_destIP   = destIP;
    m_destPort = destPort;

    return (m_lastError == CAdeUdp::UE_OK) ? true : false;
}


////////////////////////////////////////////////////////////////////////////////
/// Fermeture de la connexion
////////////////////////////////////////////////////////////////////////////////
void CCnxUDP::Close(void)
{
    m_AdeUdp.Close();
}

////////////////////////////////////////////////////////////////////////////////
/// Emission d'une trame sur la connexion
///
/// Parametres :
/// \param pFrame     la trame a emettre
/// \param frameSize  la taille de cette trame
///
/// \return true si l'emission s'est bien deroulee, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CCnxUDP::SendFrame(const unsigned char * pFrame, unsigned long frameSize)
{
    // pas de formatage sur UDP
    return SendRawFrame(pFrame, frameSize);
}
////////////////////////////////////////////////////////////////////////////////
/// Emission d'une trame sur la connexion sans faire de formattage DLE ETX ...
///
/// Parametres :
/// \param pFrame     la trame a emettre
/// \param frameSize  la taille de cette trame
///
/// \return true si l'emission s'est bien deroulee, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CCnxUDP::SendRawFrame(const unsigned char * pFrame, unsigned long frameSize)
{
    char szFunc[] = "CCnxUDP::SendFrame";

    m_lastError = m_AdeUdp.Send( m_destIP
                               , m_destPort
                               , (char*)pFrame
                               , frameSize
                               );
    PrintFrame(pFrame, frameSize, false);
    
    return (m_lastError == CAdeUdp::UE_OK) ? true : false;
}
////////////////////////////////////////////////////////////////////////////////
/// permet de connaitre la derniere erreur qui s'est produite
///
/// \return une chaine contenant l'intitule de l'erreur
////////////////////////////////////////////////////////////////////////////////
std::string CCnxUDP::GetCnxErrorMessage() const
{
    return std::string("Erreur n°") + std::string("m_lastError");
}

////////////////////////////////////////////////////////////////////////////////
/// fonction callback appelee par classe CAdeUDP
///
/// Parametres :
/// \param pCnxUdp      Pointeur sur la connexion UDP qui appelle cette callback
/// \param lpParam      Parametre de la callback : pointeur sur une connexion de maintenance Udp
/// \param buffer       Buffer contenant les donnees recues
/// \param len          nombre d'octets recus
/// \param addrSrc      string representant l'adresse IP de l'emetteur du message
/// \param portSrc      no du port de l'emetteur
////////////////////////////////////////////////////////////////////////////////
void CCnxUDP::StaticCallbackUDP(CAdeUdp *pCnxUdp, LPVOID lpParam, char*  buffer, int len, const std::string& addrSrc, int portSrc)
{
    CCnxUDP * pAdeMntUDPProto = (CCnxUDP*) lpParam;
    pAdeMntUDPProto->CallbackUDP(buffer, len, addrSrc, portSrc);
}


////////////////////////////////////////////////////////////////////////////////
/// methode qui prend en charge la reception des donnees sur le socket (elle
/// est appelee (indirectement) par CAdeUdp pour notifier la réception d'une trame
///
/// Parametres :
/// \param buffer       Buffer contenant les donnees recues
/// \param len          nombre d'octets recus
/// \param addrSrc      string representant l'adresse IP de l'emetteur du message
/// \param portSrc      no du port de l'emetteur
////////////////////////////////////////////////////////////////////////////////
void CCnxUDP::CallbackUDP(char*  buffer, int len, const std::string& addrSrc, int portSrc)
{
    char szFunc[] = "CCnxUDP::CallbackUDP";

    // On s'alloue une trame
    m_pCurrRxFrame.reset(new CRxFrame);

    // on verifie la longueur
    if ((unsigned)len > sizeof(m_pCurrRxFrame->frame))
    {
        ADE_WARNING("trame recue trop grande :" << len);
        return;
    }

    // On a reçu des donnees, on les envoie pour traitement
    memcpy_s(m_pCurrRxFrame->frame, sizeof(m_pCurrRxFrame->frame), buffer, len);
    m_pCurrRxFrame->frameSize = len;
    // on pousse la trame dans la FIFO
    PrintFrame(m_pCurrRxFrame->frame, m_pCurrRxFrame->frameSize, true);
    m_fifoRxFrame.Push(m_pCurrRxFrame.release(), true); // l'element est détruit si on ne peut pas l'insérer
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxUdp.cpp $
////////////////////////////////////////////////////////////////////////////////


///
/// @}
///
