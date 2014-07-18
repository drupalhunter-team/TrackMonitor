////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CNX
/// Definition d'une connexion de maintenance
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe de base CCnx qui définit une interface
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: tphilibert $
/// @version $Revision: 268 $
/// @date $Date: 2014-06-03 14:59:51 +0200 (mar., 03 juin 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/Cnx.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef CNX_H
#define CNX_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
#include "utils.h"
#include "AdeFifo.h"


class CCnx
{
NON_COPYABLE(CCnx);

public:
    CCnx();
    virtual ~CCnx();

    // emission d'une trame sur la connexsion
    virtual bool SendFrame(const unsigned char * pFrame, unsigned long frameSize) = 0;

    // emission d'une trame brute sur la connexsion
    virtual bool SendRawFrame(const unsigned char * pFrame, unsigned long frameSize) = 0;

    // Lecture d'une trame reçue
    bool ReceiveFrame(unsigned char * pFrame, unsigned long * pFrameSize, DWORD* pTimeStamp, int timeoutMs = INFINITE);

    // Trace d'une trame
    void PrintFrame(const unsigned char * pFrame, unsigned long frameSize, bool isRxFrame);
    
    // fermeture de la connexion
    virtual void Close(void) = 0;

protected:
    class CRxFrame{
    public:
        // On date la trame a l'allocation
        CRxFrame() {frameSize = 0; m_timeStamp = AdeGetTickCountMs();};
        unsigned char frame[8400];      // de quoi contenir une trame de 8192 data + header 
        unsigned      frameSize;
        DWORD         m_timeStamp;  // timestamp
    };
    
   CAdeFifo<CRxFrame>  m_fifoRxFrame;          // Fifo des trames recues
   boost::unique_ptr<CRxFrame> m_pCurrRxFrame; // pointeur sur la trame en train d'être reconstituée
};

#endif //CNX_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/Cnx.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
