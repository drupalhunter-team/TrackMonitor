////////////////////////////////////////////////////////////////////////////////
/// @addtogroup CNX
/// Definition d'une connexion de maintenance via une liaison serie
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CCnxSerial qui defini une connexion de maintenance
/// utilisant une liaison serie
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 90 $
/// @date $Date: 2014-05-13 10:41:27 +0200 (mar., 13 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxSerial.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef CNX_SERIAL_H
#define CNX_SERIAL_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// Uniquement des includes projet : pas d'inclusion de fichier standard
#include "AdeSerial.h"
#include "Cnx.h"


class CCnxSerial : public CCnx
{
NON_COPYABLE(CCnxSerial);
private:
    enum  T_READSTATE{                  
        WAIT_FOR_FIRST_DLE,             // en attente du premier DLE de la frame
        WAIT_FOR_STX,                   // en attente du STX de la frame
        WAIT_FOR_DATA_WITHOUT_DLE,      // en attente d'une donnee sans DLE precedemment
        WAIT_FOR_DATA_WITH_DLE,         // en attente d'une donnee avec DLE precedemment
    };

public:
    CCnxSerial();
    virtual ~CCnxSerial();

    // ouverture de la connexion 
    bool Open(const std::string& comPort, unsigned long speed);

    // emission d'une trame sur la connexsion
    virtual bool SendFrame(const unsigned char * pFrame, unsigned long frameSize);

    // emission d'une trame sur la connexsion (sans faire de formatage DLE STX ...
    virtual bool SendRawFrame(const unsigned char * pFrame, unsigned long frameSize);

    // fermeture de la connexion
    virtual void Close(void);

    CAdeSerial* GetAdeSerial(void) { return &m_AdeSerial;}

    std::string GetCnxErrorMessage() const;

private:
    static void StaticCallbackOnRecv(LPVOID lpParam, unsigned char ucReceived);
    static void StaticCallbackOnBreak(LPVOID lpParam);
    static void StaticCallbackOnErr(LPVOID lpParam);
    void CallbackOnRecv(unsigned char ucReceived);
    void CallbackOnBreak(void);
    void CallbackOnErr(void);

    CAdeSerial          m_AdeSerial;      // Classe lisant sur le port com
    T_READSTATE         m_CurrentState;   // Etat courant du reader de frame
    CAdeSerial::T_ERROR m_ErrorSerial;    // Erreur renvoyé lors de l'ouverture du port COM

};

#endif //CNX_SERIAL_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/CnxSerial.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
