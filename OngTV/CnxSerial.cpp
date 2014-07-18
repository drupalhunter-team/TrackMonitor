////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CNX
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe CCnxSerial
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 90 $
/// @date $Date: 2014-05-13 10:41:27 +0200 (mar., 13 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxSerial.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// Puis les include projet
#include "Utils.h"
#include "CnxSerial.h"



// valeurs des caracteres de controle utilises dans le protocole
#define STX             0x02
#define ETX             0x03
#define DLE             0x10

//nombre de caracteres a ajouter pour construire la frame
#define FORMAT_SIZE     4

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
CCnxSerial::CCnxSerial(void)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CCnxSerial::~CCnxSerial()
{
    Close();
}


////////////////////////////////////////////////////////////////////////////////
/// Ouverture de la connexion
///
/// Parametres :
/// \param comPort  nom du port de com
/// \param speed    vitesse de la LS
///
/// \return false si 
///                deja ouvert (SERIAL_ERROR_OPENED)
///                Erreur de connexion(SERIAL_ERROR_CANTOPENPORT)
///                Configuration invalide(SERIAL_ERROR_INVALIDCONFIG)
///                Erreur Système(SERIAL_ERROR_SYSTEM)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CCnxSerial::Open(const std::string& comPort, unsigned long speed)
{
    char szFunc[] = "CCnxSerial::Open";

    // recuperation de la configuration courante
    CAdeSerial::T_CONFIGURATION serialConfig = m_AdeSerial.GetConfiguration();

    // nom du port
   strcpy_s(serialConfig.szPort, sizeof(serialConfig.szPort), comPort.c_str());

    // vitesse
    serialConfig.ulSpeed = speed;
    
    // positionnement de la configuration
    m_AdeSerial.SetConfiguration(serialConfig);

    // ouverture de la liaison
    if (m_AdeSerial.Open(StaticCallbackOnRecv, this, StaticCallbackOnBreak, StaticCallbackOnErr)) 
    {
        m_CurrentState = WAIT_FOR_FIRST_DLE;
        return true;
    }
    else
    {
        ADE_ERROR("erreur ouverture de la liaison");
        m_ErrorSerial = m_AdeSerial.GetLastError();
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Fermeture de la connexion
////////////////////////////////////////////////////////////////////////////////
void CCnxSerial::Close(void)
{
    m_AdeSerial.Close();
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
bool CCnxSerial::SendFrame(const unsigned char * pFrame, unsigned long frameSize)
{
    char szFunc[] = "CCnxSerial::SendFrame";

    bool bResult = true;
    boost::scoped_array<unsigned char> pucToSend;
    unsigned char*  pucToSendCurr;
    unsigned long ulDleDataNumber = 0;
    unsigned long nCount = 0;
    unsigned long nNewCount = 2;

    if (frameSize == 0) 
    {
        //on n'envoie pas un message vide
        //inutile d'envoyer 4 caracteres pour rien !!!!
        bResult = false;
    }

    //Trace(true, mntFrame);

    if (bResult)
    {
        //Construction de la frame a envoyer
        //la taille maxi peut etre le double de la taille du buufer si toutes les donnees sont des DLE
        unsigned long ulTaille = (frameSize * 2) + FORMAT_SIZE;
        pucToSend.reset(new unsigned char[ulTaille]);
        pucToSendCurr = pucToSend.get();
        ulTaille = 0;

        //Debut de la frame
        *pucToSendCurr = DLE;
        ++pucToSendCurr;
        ++ulTaille;

        *pucToSendCurr = STX;
        ++pucToSendCurr;
        ++ulTaille;

        //Contenu du message
        for (nCount = 0 ; nCount < frameSize ; ++nCount)
        {
            *pucToSendCurr = pFrame[nCount];
            ++pucToSendCurr;
            ++ulTaille;
            if (pFrame[nCount] == DLE)
            {
                *pucToSendCurr = DLE;
                ++pucToSendCurr;
                ++ulTaille;
            }
        }

        //Fin de la frame
        *pucToSendCurr = DLE;
        ++pucToSendCurr;
        ++ulTaille;
        *pucToSendCurr = ETX;
        ++pucToSendCurr;
        ++ulTaille;

        //Ecriture sur le port
        bResult = m_AdeSerial.Write(pucToSend.get(), &ulTaille);
        PrintFrame(pucToSend.get(), ulTaille, false);
        if (!bResult)
        {
            m_ErrorSerial = m_AdeSerial.GetLastError();
            ADE_ERROR("erreur m_AdeSerial.Write(pucToSend, &ulTaille)");
        }
    }

    return bResult;
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
bool CCnxSerial::SendRawFrame(const unsigned char * pFrame, unsigned long frameSize)
{
    char szFunc[] = "CCnxSerial::SendRawFrame";

    bool bResult;

    //Ecriture sur le port
    bResult = m_AdeSerial.Write(pFrame, &frameSize);
    PrintFrame(pFrame, frameSize, false);
    if (!bResult)
    {
        m_ErrorSerial = m_AdeSerial.GetLastError();
        ADE_ERROR("erreur m_AdeSerial.Write(pucToSend, &ulTaille)");
    }

    return bResult;
}

///////////////////////////////////////////////////////////////////StaticCallbackOnRecv
/// fonction callback appelee par classe CAdeSerial
///
/// Parametres :
/// \param lpParam      Parametre de la callback : pointeur sur une connexion serie
/// \param ucReceived   octet recu par la classe AdeSerial
////////////////////////////////////////////////////////////////////////////////
void CCnxSerial::StaticCallbackOnRecv(LPVOID lpParam, unsigned char ucReceived)
{
    CCnxSerial * pAdeMntpSciProto = (CCnxSerial*) lpParam;
    pAdeMntpSciProto->CallbackOnRecv(ucReceived);
}
////////////////////////////////////////////////////////////////////////////////
/// methode qui prend en charge la reception des donnees sur la liaison (elle
/// est appelee (indirectement) par CAdeSerie pour notifier la réception d'un caractere
///
/// Parametres :
/// \param ucReceived   octet recu
////////////////////////////////////////////////////////////////////////////////
void CCnxSerial::CallbackOnRecv(unsigned char ucReceived)
{
    char szFunc[] = "CCnxSerial::CallbackOnRecv";

    // un caractère recu
    switch (m_CurrentState)
    {
        case WAIT_FOR_FIRST_DLE : 
        {
            //on attend le premier dle de la frame
            if (ucReceived == DLE)
            {
                m_CurrentState = WAIT_FOR_STX;
            }
            break;
        }
        case WAIT_FOR_STX : 
        {
            //on attend le premier stx de la frame
            if (ucReceived == STX)
            {
                m_CurrentState = WAIT_FOR_DATA_WITHOUT_DLE;
                // On se réalloue une trame 
                m_pCurrRxFrame.reset(new CRxFrame);
            }
            else
            {
                ADE_WARNING("STX attendu non recu");
                m_CurrentState = WAIT_FOR_FIRST_DLE;
            }
            break;
        }
        case WAIT_FOR_DATA_WITHOUT_DLE : 
        {
            if (ucReceived == DLE)
            {
                m_CurrentState = WAIT_FOR_DATA_WITH_DLE;
            }
            else
            {
                // On stocke la caractere
                if (m_pCurrRxFrame->frameSize == sizeof(m_pCurrRxFrame->frame))
                {
                    //le message depasse la taille max
                    //on abandonne le message
                    ADE_WARNING("depassement de la taille max d'un message");
                    m_CurrentState = WAIT_FOR_FIRST_DLE;
                }
                else
                {
                    // on stocke le caractere
                    m_pCurrRxFrame->frame[m_pCurrRxFrame->frameSize] = ucReceived;
                    m_pCurrRxFrame->frameSize++;
                }
            }
            break;
        }
        case WAIT_FOR_DATA_WITH_DLE : 
        {
            m_CurrentState = WAIT_FOR_DATA_WITHOUT_DLE;
            if (ucReceived == STX)
            {
                // sequence de debut d'une nouvelle trame => on abandonne
                // celle en cours de constitution
                ADE_WARNING("nouvelle trame => abandon d'une trame en cours de constitution");
                m_CurrentState = WAIT_FOR_FIRST_DLE;
            }
            else if (ucReceived == ETX)
            {
                // Fin de la trame
                // on la stocke dans la FIFO
                PrintFrame(m_pCurrRxFrame->frame, m_pCurrRxFrame->frameSize, true);
                m_fifoRxFrame.Push(m_pCurrRxFrame.release(), true); // l'element est détruit si on ne peut pas l'insérer
                m_CurrentState = WAIT_FOR_FIRST_DLE;
            }
            else if (ucReceived == DLE)
            {
                // DLE double, on stocke donc un DLE et on avance dans le buffer
                if (m_pCurrRxFrame->frameSize == sizeof(m_pCurrRxFrame->frame))
                {
                    //le message depasse la taille max
                    //on abandonne le message
                    ADE_WARNING("depassement de la taille max d'un message");
                    m_CurrentState = WAIT_FOR_FIRST_DLE;
                }
                else
                {
                    // on stocke le caractere
                    m_pCurrRxFrame->frame[m_pCurrRxFrame->frameSize] = ucReceived;
                    m_pCurrRxFrame->frameSize++;
                }
            }
            else
            {
                // ce n'est pas bon, on abandonne la trame en cours et on 
                // se remet en attente d'un debut de trame
                m_CurrentState = WAIT_FOR_FIRST_DLE;
                ADE_WARNING("caractere non attendu apres un DLE");
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////StaticCallbackOnBreak
/// fonction callback appelee par classe CAdeSerial
///
/// Parametres :
/// \param lpParam      Parametre de la callback : pointeur sur une connexion serie
////////////////////////////////////////////////////////////////////////////////
void CCnxSerial::StaticCallbackOnBreak(LPVOID lpParam)
{
    CCnxSerial * pAdeMntpSciProto = (CCnxSerial*) lpParam;
    pAdeMntpSciProto->CallbackOnBreak();
}
////////////////////////////////////////////////////////////////////////////////
/// methode qui prend en charge la reception d'un break sur la liaison (elle
/// est appelee (indirectement) par CAdeSerie pour notifier la réception d'un break
////////////////////////////////////////////////////////////////////////////////
void CCnxSerial::CallbackOnBreak(void)
{
}

///////////////////////////////////////////////////////////////////StaticCallbackOnErr
/// fonction callback appelee par classe CAdeSerial
///
/// Parametres :
/// \param lpParam      Parametre de la callback : pointeur sur une connexion serie
////////////////////////////////////////////////////////////////////////////////
void CCnxSerial::StaticCallbackOnErr(LPVOID lpParam)
{
    CCnxSerial * pAdeMntpSciProto = (CCnxSerial*) lpParam;
    pAdeMntpSciProto->CallbackOnErr();
}
////////////////////////////////////////////////////////////////////////////////
/// methode qui prend en charge le traitement d'une erreur bas niveau (elle
/// est appelee (indirectement) par CAdeSerie pour notifier une erreur
////////////////////////////////////////////////////////////////////////////////
void CCnxSerial::CallbackOnErr(void)
{
    char szFunc[] = "CCnxSerial::CallbackOnErr";
    // On abandonne la reception en cours et on se remet en attente d'un
    // debut de trame
    m_CurrentState = WAIT_FOR_FIRST_DLE;
    ADE_WARNING("erreur SCI");
}

////////////////////////////////////////////////////////////////////////////////
/// permet de connaitre la derniere erreur qui s'est produite
///
/// \return une chaine contenant l'intitule de l'erreur
////////////////////////////////////////////////////////////////////////////////
std::string CCnxSerial::GetCnxErrorMessage() const
{
    switch (m_ErrorSerial)
    {
        case CAdeSerial::SERIAL_NO_ERROR : return "Port COM ouvert avec succès"; break;
        case CAdeSerial::SERIAL_ERROR_SYSTEM : return "Erreur système à l'ouverture du port COM"; break;
        case CAdeSerial::SERIAL_ERROR_NOTOPENED : return "Port COM non ouvert"; break;
        case CAdeSerial::SERIAL_ERROR_OPENED : return "Port COM déjà ouvert"; break;
        case CAdeSerial::SERIAL_ERROR_CANTOPENPORT : return "Impossible d'ouvrir le port COM. Il est peut être ouvert par une autre application."; break;
        case CAdeSerial::SERIAL_ERROR_INVALIDCONFIG : return "Configuration du port COM invalide"; break;
        case CAdeSerial::SERIAL_ERROR_INVALIDREADMODE : return "Configuration en lecture du port COM invalide"; break;
        default : return "Erreur inconnue";
    }
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxSerial.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
