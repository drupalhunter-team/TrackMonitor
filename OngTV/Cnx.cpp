////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CNX
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe CCnx
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 86 $
/// @date $Date: 2014-05-12 16:10:43 +0200 (lun., 12 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/Cnx.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// Puis les include projet
#include "Utils.h"
#include "Cnx.h"
#include "TestMgr.h"




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
CCnx::CCnx(void)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CCnx::~CCnx()
{
}


////////////////////////////////////////////////////////////////////////////////
/// Lecture d'une trame
///
/// Parametres :
/// \param pFrame     le buffer dans lequel ecrire cette trame
/// \param pFrameSize En entrée : la taille max que peut recevoir le buffer
///                   En sortie : la taille de la trame (0 si pas de trame reçue)
/// \param pTimeStamp timestamp de la trame
///
/// \return true si l'emission s'est bien deroulee, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CCnx::ReceiveFrame(unsigned char * pFrame, unsigned long * pFrameSize, DWORD* pTimeStamp, int timeoutMs)
{
    char szFunc[] = "CCnx::ReceiveFrame";

    CRxFrame* pRxFrame = m_fifoRxFrame.Pop(timeoutMs);   // pas d'attente
    if (NULL == pRxFrame)
    {
        // pas de trame dans la Fifo
        if (pFrameSize)
        {
            *pFrameSize = 0;
        }
        return false;
    }
    // mise a jour des informations
    if (pFrame && pFrameSize)
    {
        // On tronque le cas échéant
        if (pRxFrame->frameSize < *pFrameSize)
        {
            *pFrameSize = pRxFrame->frameSize;
        }
        memcpy_s(pFrame, *pFrameSize, pRxFrame->frame, *pFrameSize);
    }
    if (pTimeStamp)
    {
        *pTimeStamp = pRxFrame->m_timeStamp;
    }
    
    // on detruit la trame
    delete pRxFrame;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
/// Ecriture d'une trame dans la trace
///
/// Parametres :
/// \param pFrame     la trame
/// \param frameSize  la taille de cette trame
/// \param isRxFrame  true si frame reçue, false si émise
///
////////////////////////////////////////////////////////////////////////////////
void CCnx::PrintFrame(const unsigned char * pFrame, unsigned long frameSize, bool isRxFrame)
{
    char szTmp[150];
    if (isRxFrame)
    {
        sprintf_s(szTmp, sizeof(szTmp), "<= %d bytes : ", frameSize);
    }
    else
    {
        sprintf_s(szTmp, sizeof(szTmp), "=> %d bytes : ", frameSize);
    }
    // on se limite au 50 premiers octets de la trame
    if (frameSize > 50)
    {
        frameSize = 50;
    }
    HexaToString(pFrame, frameSize, &szTmp[strlen(szTmp)]);
    CTestMgr::ReadSingleInstance()->Print(szTmp);
}



////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/Cnx.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
