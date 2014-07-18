////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CNX
/// Definition d'une connexion de maintenance via une liaison UDP
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CCnxUdp qui defini une connexion de maintenance
/// utilisant une liaison UDP
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 90 $
/// @date $Date: 2014-05-13 10:41:27 +0200 (mar., 13 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxUdp.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef CNX_UDP_H
#define CNX_UDP_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// Uniquement des includes projet : pas d'inclusion de fichier standard
#include "AdeUdp.h"
#include "Cnx.h"


class CCnxUDP : public CCnx
{
NON_COPYABLE(CCnxUDP);
private:

public:
    CCnxUDP();
    virtual ~CCnxUDP();

    // ouverture de la connexion 
    bool Open(const std::string& localIP, int localPort, const std::string& destIP, int destPort);

    // emission d'une trame sur la connexsion
    virtual bool SendFrame(const unsigned char * pFrame, unsigned long frameSize);

    // emission d'une trame sur la connexsion (sans faire de formatage DLE STX ...
    virtual bool SendRawFrame(const unsigned char * pFrame, unsigned long frameSize);

    // fermeture de la connexion
    virtual void Close(void);

    std::string GetCnxErrorMessage() const;

private:
    // methode static qui appelle la suivante
    static void StaticCallbackUDP(CAdeUdp *pCnxUdp, LPVOID lpParam, char*  buffer, int len, const std::string& addrSrc, int portSrc);
    // methode qui prend en charge la reception des donnees sur le socket
    void CallbackUDP(char*  buffer, int len, const std::string& addrSrc, int portSrc);

    CAdeUdp   m_AdeUdp;                     // connexion UDP utilisee

    std::string m_destIP;                   // adresse IP du distant
    int         m_destPort;                 // port UDP distant
    int         m_lastError;                // derniere erreur

};

#endif //CNX_UDP_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxUdp.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
