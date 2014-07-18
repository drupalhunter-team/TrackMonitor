
#include "StdAfx.h"
#include <shellapi.h>
#include "Resource.h"
#ifndef __cplusplus
#include "stdlib.h"
#endif

#include "tolua.h"

/* Exported function */
#include "OngTV.h"
#include "MainFrm.h"
#include "AdeFile.h"
#include "PathMgr.h"
#include "DlgGetVal.h"
#include "DlgGetValFromList.h"
#include "DlgGetValNum.h"
#include "utils.h"
#include "TestMgr.h"
#include "LuaInterpreter.h"
#include "CnxSerial.h"
#include "CnxUdp.h"

extern "C"
{

#define STATUS_OK                             0  ///< Pas de problème
#define STATUS_NOK                            1  ///< Echec 
#define STATUS_OVERFLOW                       2  ///< Overflow dans un buffer
#define STATUS_CNX_NOT_OPENED                 3  ///< la liaison n'est pas ouverte
#define STATUS_NO_FRAME_RECEIVED              4  ///< pas de trame reçue
#define STATUS_CRC_ERROR                      5  ///< trame reçue avec une erreur de CRC

// taille max des buffers RX/TX
#define BUFFER_SIZE 10000
static unsigned char rxBuffer[BUFFER_SIZE];
static unsigned char txBuffer[BUFFER_SIZE];

// taille de la trame recue (si 0, pas de trame recue)
static unsigned long rxBufferSize;

// pointeur sur la connexion ouverte (une seule a un instant t)
static boost::unique_ptr<CCnx> pCnx;

static char szLastErrorText[1000];
static char DefaultEmptyString[] = "";
static const char* My_tolua_tostring (lua_State* L, int narg, const char* def)
{
    const char *res = tolua_tostring (L, narg, def);
    if (NULL == res)
    {
        // On renvoie la valeur par defaut si tolua_tostring nous renvoie NULL
        res = def;
    }
    return res;
}
static int My_tolua_isnumberarray (lua_State* L, int lo, int* pDim, int def, tolua_Error* err)
{
    if (!tolua_istable(L,lo,def,err))
        return 0;
    // on va maintenant parcourir la table pour trouver sa dimension
    lua_pushnil(L);  /* first key */
    *pDim = 0;
    while (lua_next(L, lo) != 0)
    {
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
        ++(*pDim);
    }
    // on verifie ensuite qu'il s'agit bien d'un tableau de nombre
    return tolua_isnumberarray (L, lo, *pDim, def, err);
}

static int My_tolua_isstringarray (lua_State* L, int lo, int* pDim, int def, tolua_Error* err)
{
    if (!tolua_istable(L,lo,def,err))
        return 0;
    // on va maintenant parcourir la table pour trouver sa dimension
    lua_pushnil(L);  /* first key */
    *pDim = 0;
    while (lua_next(L, lo) != 0)
    {
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
        ++(*pDim);
    }
    // on verifie ensuite qu'il s'agit bien d'un tableau de string
    return tolua_isstringarray (L, lo, *pDim, def, err);
}
/* function to register type */
static void tolua_reg_types (lua_State* L)
{
}


static bool GetAndCheckUnsignedInteger(lua_State* L, int narg, unsigned long min, unsigned long max, double* pValue)
{
    double val = tolua_tonumber(L, narg, min-1); // => on passe une valeur par defaut en dehors des bornes
                                                 // afin de renvoyer faux si c'est la valeur par defaut qui nous
                                                 // est renvoyée (nombre pas bon)
    if ((((double)min) <= val) && (val <= ((double)max)))
    {
        *pValue = val;
        return true;
    }
    else
    {
        return false;
    }
}
static bool GetAndCheckUnsigned8BitsValue(lua_State* L, int narg, unsigned char* pValue, unsigned long min = 0, unsigned long max = 0xFF)
{
    double val;
    if (false == GetAndCheckUnsignedInteger(L, narg, min, max, &val))
    {
        return false;
    }
    else
    {
        *pValue = (unsigned char)val;
        return true;
    }
}

static bool GetAndCheckUnsigned16BitsValue(lua_State* L, int narg, unsigned short* pValue, unsigned long min = 0, unsigned long max = 0xFFFF)
{
    double val;
    if (false == GetAndCheckUnsignedInteger(L, narg, min, max, &val))
    {
        return false;
    }
    else
    {
        *pValue = (unsigned short)val;
        return true;
    }
}

static bool GetAndCheckUnsigned32BitsValue(lua_State* L, int narg, unsigned long* pValue, unsigned long min = 0, unsigned long max = 0xFFFFFFFF)
{
    double val;
    if (false == GetAndCheckUnsignedInteger(L, narg, min, max, &val))
    {
        return false;
    }
    else
    {
        *pValue = (unsigned long)val;
        return true;
    }
}

// fonction pour mettre en forme une erreur constatee sur les arguments d'entree d'une
// des fonctions LUA suivantes
static int BuildBadArgTypeErrorMsg(lua_State* L, const char *szFctName, tolua_Error* pErr)
{
    static char szTmp[100];
    if (NULL == pErr->type)
    {
        // erreur sur un check tolua_isnoobj
        sprintf_s(szTmp, sizeof(szTmp), "Bad number of arguments in '%s'", szFctName);
    }
    else
    {
        // erreur sur un type d'argument
        sprintf_s(szTmp, sizeof(szTmp), "#fIn '%s'", szFctName);
    }
    tolua_error(L, szTmp, pErr);
    return 0;
}
static int BuildBadArgValueErrorMsg(lua_State* L, const char *szFctName, const char *szArgName)
{
    static char szTmp[100];
    sprintf_s(szTmp, sizeof(szTmp), "incorrect value for arg '%s' in '%s'", szArgName, szFctName);
    tolua_error(L, szTmp, NULL);
    return 0;
}

// Ouverture d'une connexion
// Soit il y a deux paramètres et il s'agit de l'ouverture d'un port de communication série
// Input Param 1  : Nom du port (string)
// Input Param 2  : vitesse de communication (nombre)
// Soit il y a quatre paramètres et il s'agit de l'ouverture d'un port de communication UDP
// Input Param 1  : IP locale (string) - peut être vide
// Input Param 2  : port UDP local (nombre)
// Input Param 3  : IP destination (string)
// Input Param 4  : port UDP de destination (nombre)
// Output 1       : Code retour (nombre)
static int ITFLUA_OpenCnx(lua_State* L)
{
    char szFunc[] = "OpenCnx";
    tolua_Error tolua_err;
    bool isOpened;
    
    // On ferme une eventuelle connexion
    pCnx.reset();

    switch (lua_gettop(L))
    {
    case 2:
        {
            // ouverture d'un port serie
            if (   !tolua_isstring(L, 1, 0, &tolua_err)
                || !tolua_isnumber(L, 2, 0, &tolua_err)
               )
            {
                return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
            }
            std::string comPort(((char*) My_tolua_tostring(L, 1, DefaultEmptyString)));
            unsigned long speed = ((unsigned long) tolua_tonumber(L, 2, 0));
            CCnxSerial* pCnxSerial = new(CCnxSerial);
            isOpened = pCnxSerial->Open(comPort, speed);
            // et on sauvegarde la connexion
            pCnx.reset(pCnxSerial);
        }
        break;
    case 4:
        {
            // ouverture d'un canal UDP
            if (   !tolua_isstring(L, 1, 0, &tolua_err)
                || !tolua_isnumber(L, 2, 0, &tolua_err)
                || !tolua_isstring(L, 3, 0, &tolua_err)
                || !tolua_isnumber(L, 4, 0, &tolua_err)
               )
            {
                return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
            }
            std::string localIP(((char*) My_tolua_tostring(L, 1, DefaultEmptyString)));
            unsigned long localPort = ((unsigned long) tolua_tonumber(L, 2, 0));
            std::string destIP(((char*) My_tolua_tostring(L, 3, DefaultEmptyString)));
            unsigned long destPort = ((unsigned long) tolua_tonumber(L, 4, 0));
            CCnxUDP* pCnxUDP = new(CCnxUDP);
            isOpened = pCnxUDP->Open(localIP, localPort, destIP, destPort);
            // et on sauvegarde la connexion
            pCnx.reset(pCnxUDP);
        }
        break;
    default:
        tolua_err.type = NULL;
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    if (isOpened)
    {
        tolua_pushnumber(L, (double)STATUS_OK);
    }
    else
    {
        // On n'a pas réussion à ouvrir la connexion
        // on la libère
        pCnx.reset();
        tolua_pushnumber(L, (double)STATUS_NOK);
    }
    return 1;
}
// Fermeture d'une connexion
static int ITFLUA_CloseCnx(lua_State* L)
{
    // On ferme une eventuelle connexion
    pCnx.reset();
    return 0;
}
// Ecriture d'une trame (le CRC16 est rajouté par la fonction)
// Input Param 1  : nombre d'octets à écrire (présents dans le buffer de transmission)
// Output 1       : Code retour de la fonction (nombre)
static int ITFLUA_SendFrame(lua_State* L)
{
    char szFunc[] = "SendFrame";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    unsigned long nBytes = ((unsigned long) tolua_tonumber(L, 1, 0));

    if (NULL == pCnx.get())
    {
        // connexion pas ouverte
        tolua_pushnumber(L, (double)STATUS_CNX_NOT_OPENED);
        return 1;
    }
    unsigned short crc16;
    if (nBytes > (BUFFER_SIZE + sizeof(crc16)))    // place pour le CRC16
    {
        // trop grand
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
        return 1;
    }
    // On calcule le CRC
    crc16 = CRC16_Compute(txBuffer, nBytes);
    // On le copie dans le buffer
    memcpy(&txBuffer[nBytes], &crc16, sizeof(crc16));

    // On envoie la trame
    if (pCnx->SendFrame(txBuffer, nBytes + sizeof(crc16)))
    {
        tolua_pushnumber(L, (double)STATUS_OK);
    }
    else
    {
        tolua_pushnumber(L, (double)STATUS_NOK);
    }
    return 1;
}
// Ecriture d'une trame brute (ie sans formatage DLE sur une liaison série ni ajout du CRC16)
// Input Param 1  : nombre d'octets à écrire (présents dans le buffer de transmission)
// Output 1       : Code retour de la fonction (nombre)
static int ITFLUA_SendRawFrame(lua_State* L)
{
    char szFunc[] = "SendRawFrame";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    unsigned long nBytes = ((unsigned long) tolua_tonumber(L, 1, 0));

    if (NULL == pCnx.get())
    {
        // connexion pas ouverte
        tolua_pushnumber(L, (double)STATUS_CNX_NOT_OPENED);
        return 1;
    }
    if (nBytes > BUFFER_SIZE)
    {
        // trop grand
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
        return 1;
    }

    // On envoie la trame
    if (pCnx->SendRawFrame(txBuffer, nBytes))
    {
        tolua_pushnumber(L, (double)STATUS_OK);
    }
    else
    {
        tolua_pushnumber(L, (double)STATUS_NOK);
    }
    return 1;
}

// Lecture d'une trame
// Input Param 1  : timeout d'attente de la réception d'une trame (en ms)
// Output 1       : Code retour de la fonction (nombre)
// Output 2       : taille de la trame reçue (0 s'il n'y avait pas de trame reçue) - la trame est
//                  copiée dans le buffer de réception
// Output 3       : timestamp de la trame (0 si pas de trame reçue)
static int ITFLUA_ReceiveFrame(lua_State* L)
{
    char szFunc[] = "ReceiveFrame";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    unsigned long timeoutMs = ((unsigned long) tolua_tonumber(L, 1, 0));

    if (NULL == pCnx.get())
    {
        // connexion pas ouverte
        tolua_pushnumber(L, (double)STATUS_CNX_NOT_OPENED);
        return 1;
    }

    // On se met en attente d'une trame
    DWORD timeStamp;
    rxBufferSize = BUFFER_SIZE;
    if (pCnx->ReceiveFrame(rxBuffer, &rxBufferSize, &timeStamp, timeoutMs))
    {
        // OK, on a reçu une trame
        // On verifie le CRC
        unsigned short crc16; 
        if (rxBufferSize < sizeof(crc16))
        {
            // pas de place pour le CRC
            tolua_pushnumber(L, (double)STATUS_CRC_ERROR);
        }
        else
        {
            // On enlève la taille du CRC
            rxBufferSize -= sizeof(crc16);
            // On extrait le crc
            memcpy(&crc16, &rxBuffer[rxBufferSize], sizeof(crc16));
            if (CRC16_Compute(rxBuffer, rxBufferSize) == crc16)
            {
                tolua_pushnumber(L, (double)STATUS_OK);
            }
            else
            {
                // erreur de CRC
                tolua_pushnumber(L, (double)STATUS_CRC_ERROR);
            }
        }
        // dans tous les cas, on renvoie la taille des données reçues et le timestamp
        tolua_pushnumber(L, (double)rxBufferSize);
        tolua_pushnumber(L, (double)timeStamp);
    }
    else
    {
        // On n'a pas reçu de trame
        tolua_pushnumber(L, (double)STATUS_NO_FRAME_RECEIVED);
        tolua_pushnumber(L, (double)0);
        tolua_pushnumber(L, (double)0);
    }
    return 3;
}

// Ecriture d'un octet 8 bits non signes dans le buffer de transmission
// Input Param 1  : Offset dans le buffer (au sens C => 0 pour accéder au premier octet du buffer) (nombre)
// Input Param 2  : Valeur a ecrire (nombre)
// Output 1       : Code retour de la fonction (nombre)
static int ITFLUA_SetUByteInTxBuffer(lua_State* L)
{
    char szFunc[] = "SetUByteInTxBuffer";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnumber(L, 2, 0, &tolua_err)
        || !tolua_isnoobj(L, 3, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    unsigned long offsetInBytes = ((unsigned long) tolua_tonumber(L, 1, 0));
    unsigned char Val8;
    if (false == GetAndCheckUnsigned8BitsValue(L, 2, &Val8))
    {
        return BuildBadArgValueErrorMsg(L, szFunc, "ByteToWrite");
    }
    if ((offsetInBytes + sizeof(Val8)) > BUFFER_SIZE)
    {
        // depassement dans le buffer
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
    }
    else
    {
        // on ecrit l'octet dans le buffer de transmission
        txBuffer[offsetInBytes] = Val8;
        tolua_pushnumber(L, (double)STATUS_OK);
    }
    return 1;
}

// Ecriture d'un mot 16 bits non signes dans le buffer de transmission
// Input Param 1  : Offset dans le buffer (au sens C => 0 pour accéder au premier octet du buffer) (nombre)
// Input Param 2  : Valeur a ecrire (nombre)
// Output 1       : Code retour de la fonction (nombre)
static int ITFLUA_SetUShortInTxBuffer(lua_State* L)
{
    char szFunc[] = "SetUShortInTxBuffer";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnumber(L, 2, 0, &tolua_err)
        || !tolua_isnoobj(L, 3, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    unsigned long offsetInBytes = ((unsigned long) tolua_tonumber(L, 1, 0));
    unsigned short Val16;
    if (false == GetAndCheckUnsigned16BitsValue(L, 2, &Val16))
    {
        return BuildBadArgValueErrorMsg(L, szFunc, "ShortToWrite");
    }
    if ((offsetInBytes + sizeof(Val16)) > BUFFER_SIZE)
    {
        // depassement dans le buffer
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
    }
    else
    {
        // on ecrit le short dans le buffer de transmission
        memcpy(&txBuffer[offsetInBytes], &Val16, sizeof(Val16));
        tolua_pushnumber(L, (double)STATUS_OK);
    }
    return 1;
}

// Ecriture d'un mot 32 bits non signes dans le buffer de transmission
// Input Param 1  : Offset dans le buffer (au sens C => 0 pour accéder au premier octet du buffer) (nombre)
// Input Param 2  : Valeur a ecrire (nombre)
// Output 1       : Code retour de la fonction (nombre)
static int ITFLUA_SetULongInTxBuffer(lua_State* L)
{
    char szFunc[] = "SetULongInTxBuffer";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnumber(L, 2, 0, &tolua_err)
        || !tolua_isnoobj(L, 3, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    unsigned long offsetInBytes = ((unsigned long) tolua_tonumber(L, 1, 0));
    unsigned long Val32;
    if (false == GetAndCheckUnsigned32BitsValue(L, 2, &Val32))
    {
        return BuildBadArgValueErrorMsg(L, szFunc, "LongToWrite");
    }
    if ((offsetInBytes + sizeof(Val32)) > BUFFER_SIZE)
    {
        // depassement dans le buffer
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
    }
    else
    {
        // on ecrit le short dans le buffer de transmission
        memcpy(&txBuffer[offsetInBytes], &Val32, sizeof(Val32));
        tolua_pushnumber(L, (double)STATUS_OK);
    }
    return 1;
}

// Lecture d'un octet 8 bits non signes dans le buffer de réception
// Input Param 1  : Offset dans le buffer (au sens C => 0 pour accéder au premier octet du buffer) (nombre)
// Output 1       : Code retour de la fonction (nombre)
// Output 2       : Valeur lue  (nombre)
static int ITFLUA_GetUByteInRxBuffer(lua_State* L)
{
    char szFunc[] = "GetUByteInRxBuffer";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    unsigned char Val8;
    unsigned long offsetInBytes = ((unsigned long) tolua_tonumber(L, 1, 0));

    if ((offsetInBytes + sizeof(Val8)) > rxBufferSize)
    {
        // depassement dans le buffer
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
        return 1;
    }
    else
    {
        // on lit l'octet dans le buffer de reception
        Val8 = rxBuffer[offsetInBytes];
        tolua_pushnumber(L, (double)STATUS_OK);
        tolua_pushnumber(L, (double)Val8);
        return 2;
    }
}

// Lecture d'un mot 16 bits non signes dans le buffer de réception
// Input Param 1  : Offset dans le buffer (au sens C => 0 pour accéder au premier octet du buffer) (nombre)
// Output 1       : Code retour de la fonction (nombre)
// Output 2       : Valeur lue  (nombre)
static int ITFLUA_GetUShortInRxBuffer(lua_State* L)
{
    char szFunc[] = "GetUShortInRxBuffer";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    unsigned short Val16;
    unsigned long offsetInBytes = ((unsigned long) tolua_tonumber(L, 1, 0));

    if ((offsetInBytes + sizeof(Val16)) > rxBufferSize)
    {
        // depassement dans le buffer
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
        return 1;
    }
    else
    {
        // on lit l'octet dans le buffer de reception
        memcpy(&Val16, &rxBuffer[offsetInBytes], sizeof(Val16));
        tolua_pushnumber(L, (double)STATUS_OK);
        tolua_pushnumber(L, (double)Val16);
        return 2;
    }
}

// Lecture d'un mot 32 bits non signes dans le buffer de réception
// Input Param 1  : Offset dans le buffer (au sens C => 0 pour accéder au premier octet du buffer) (nombre)
// Output 1       : Code retour de la fonction (nombre)
// Output 2       : Valeur lue  (nombre)
static int ITFLUA_GetULongInRxBuffer(lua_State* L)
{
    char szFunc[] = "GetULongInRxBuffer";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    unsigned long Val32;
    unsigned long offsetInBytes = ((unsigned long) tolua_tonumber(L, 1, 0));

    if ((offsetInBytes + sizeof(Val32)) > rxBufferSize)
    {
        // depassement dans le buffer
        tolua_pushnumber(L, (double)STATUS_OVERFLOW);
        return 1;
    }
    else
    {
        // on lit l'octet dans le buffer de reception
        memcpy(&Val32, &rxBuffer[offsetInBytes], sizeof(Val32));
        tolua_pushnumber(L, (double)STATUS_OK);
        tolua_pushnumber(L, (double)Val32);
        return 2;
    }
}

// Appel de la boite de dialogue standard de windows pour l'ouverture d'un fichier
// Input Param 1  : Extension par defaut (string)
// Input Param 2  : Filtre de selection (string)
// Input Param 3  : Titre de la boite de dialogue (parametre optionnel) (string)
// Output 1       : Chaine de caracteres contenant le nom du fichier (vide si pas de selection) (string)
static int ITFLUA_FileOpenDialog(lua_State* L)
{
    char szFunc[] = "FileOpenDialog";
    tolua_Error tolua_err;
    char* szCaption = NULL;
    switch (lua_gettop(L))
    {
    case 3:
        if (!tolua_isstring(L, 3, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        szCaption = ((char*) My_tolua_tostring(L, 3, DefaultEmptyString));
    case 2:
        if (   !tolua_isstring(L, 1, 0, &tolua_err) 
            || !tolua_isstring(L, 2, 0, &tolua_err) 
           )
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        break;
    default:
        tolua_err.type = NULL;
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);

    }

    char szFileName[1024];
    char* szDefExt = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
    char* szFilter = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    szFileName[0] = 0;

    std::string fileName = theApp.SelectFile(TRUE, szDefExt, szFilter, NULL, szCaption);
    if(0 != fileName.size())
    {
        // on recupere son nom
        strncpy_s(szFileName, sizeof(szFileName), fileName.c_str(), _TRUNCATE);
    }

    tolua_pushstring(L,(const char*)szFileName);
    return 1;
}

// Appel de la boite de dialogue standard de windows pour la sauvegarde dans un fichier
// Input Param 1  : Extension par defaut (string)
// Input Param 2  : Filtre de selection (string)
// Input Param 3  : Titre de la boite de dialogue (parametre optionnel) (string)
// Output 1       : Chaine de caracteres contenant le nom du fichier (vide si pas de selection) (string)
static int ITFLUA_FileSaveDialog(lua_State* L)
{
    char szFunc[] = "FileSaveDialog";
    tolua_Error tolua_err;
    char* szCaption = NULL;
    switch (lua_gettop(L))
    {
    case 3:
        if (!tolua_isstring(L, 3, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        szCaption = ((char*) My_tolua_tostring(L, 3, DefaultEmptyString));
    case 2:
        if (   !tolua_isstring(L, 1, 0, &tolua_err) 
            || !tolua_isstring(L, 2, 0, &tolua_err) 
           )
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        break;
    default:
        tolua_err.type = NULL;
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);

    }

    char szFileName[1024];
    char* szDefExt = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
    char* szFilter = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    szFileName[0] = 0;  // chaine vide par defaut

    std::string fileName = theApp.SelectFile(FALSE, szDefExt, szFilter, NULL, szCaption);
    if(0 != fileName.size())
    {
        // on recupere son nom
        strncpy_s(szFileName, sizeof(szFileName), fileName.c_str(), _TRUNCATE);
    }

    tolua_pushstring(L,(const char*)szFileName);
    return 1;
}

// Affichage d'une messageBox (appel de la fonction MessageBox de l'API Windows)
// Input Param 1  : Texte de la Message Box (string)
// Input Param 2  : Titre de la Message Box (string)
// Input Param 3  : Type de la Message Box (cf MSDN pour les valeurs possibles - champ uType) (nombre) 
// Output 1       : Retour de la Message Box (cf MSDN pour les valeurs possibles) (nombre)
static int ITFLUA_MessageBox(lua_State* L)
{
    char szFunc[] = "MessageBox";
    tolua_Error tolua_err;
    if (   !tolua_isstring(L, 1, 0, &tolua_err) 
        || !tolua_isstring(L, 2, 0, &tolua_err) 
        || !tolua_isnumber(L, 3, 0, &tolua_err) 
        || !tolua_isnoobj(L, 4, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szText    = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
    char* szCaption = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    UINT uType      = ((UINT) tolua_tonumber(L, 3, 0));

    int tolua_ret = MessageBox(NULL, szText, szCaption, uType);

    tolua_pushnumber(L, (double)tolua_ret);
    return 1;
}

// Affichage d'une boite de dialogue permettant de recuperer une chaine de caracteres saisie par l'utilisateur
// Input Param 1  : Titre de la boite de dialogue a afficher (string)
// Input Param 2  : Label de la valeur a saisir (parametre optionnel) (string)
// Output 1       : Code retour de la fonction : STATUS_OK si l'utilisateur a appuye sur le bouton OK de la boite de dialogue (nombre)
// Output 2       : Chaine de caracteres saisie par l'utilisateur (string)
static int ITFLUA_DlgGetValString(lua_State* L)
{
    char szFunc[] = "DlgGetValString";
    char* szDlgTitle;
    char* szValueCaption = DefaultEmptyString;  // valeur par defaut si pas present
    tolua_Error tolua_err;
    switch (lua_gettop(L))
    {
    case 2:
        if (!tolua_isstring(L, 2, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        szValueCaption = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    case 1:
        if (!tolua_isstring(L, 1, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        szDlgTitle = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
        break;
    default:
        tolua_err.type = NULL;
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);

    }
    CDlgGetVal dlgGetVal(szDlgTitle, szValueCaption);
    if (dlgGetVal.DoModal() != IDOK)
    {
        tolua_pushnumber(L, (double)STATUS_NOK);
        tolua_pushstring(L,(const char*)"");
    }
    else
    {
        tolua_pushnumber(L, (double)STATUS_OK);
        tolua_pushstring(L,(const char*)LPCSTR(dlgGetVal.m_szValue));
    }
    return 2;
}

// Affichage d'une boite de dialogue permettant de recuperer un nombre saisi par l'utilisateur
// Input Param 1  : Titre de la boite de dialogue a afficher (string)
// Input Param 2  : Label de la valeur a saisir (parametre optionnel) (string)
// Output 1       : Code retour de la fonction : STATUS_OK si l'utilisateur a appuye sur le bouton OK de la boite de dialogue (nombre)
// Output 2       : Nombre saisi par l'utilisateur (nombre)
static int ITFLUA_DlgGetValNum(lua_State* L)
{
    char szFunc[] = "DlgGetValNum";
    char* szDlgTitle;
    char* szValueCaption = DefaultEmptyString;  // valeur par defaut si pas present
    tolua_Error tolua_err;
    switch (lua_gettop(L))
    {
    case 2:
        if (!tolua_isstring(L, 2, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        szValueCaption = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    case 1:
        if (!tolua_isstring(L, 1, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        szDlgTitle = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
        break;
    default:
        tolua_err.type = NULL;
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);

    }
    CDlgGetValNum dlgGetVal(szDlgTitle, szValueCaption);
    if (dlgGetVal.DoModal() != IDOK)
    {
        tolua_pushnumber(L, (double)STATUS_NOK);
        tolua_pushnumber(L, 0.0);
    }
    else
    {
        unsigned long num;
        // on convertit la valeur en un long
        if (CnvStrToULong(std::string(LPCSTR(dlgGetVal.m_szValue)), &num) == false)
        {
            // erreur de conversion
            tolua_pushnumber(L, (double)STATUS_NOK);
            tolua_pushnumber(L, 0.0);
        }
        else
        {
            tolua_pushnumber(L, (double)STATUS_OK);
            tolua_pushnumber(L, (double)num);
        }
    }
    return 2;
}
// Affichage d'une boite de dialogue proposant a l'utilisateur de faire un choix parmi plusieurs valeurs
// Input Param 1  : Titre de la boite de dialogue a afficher (string)
// Input Param 2  : Tableau des choix (tableau de chaines de caracteres)
// Output 1       : Code retour de la fonction : STATUS_OK si l'utilisateur a appuye sur le bouton OK de la boite de dialogue (nombre)
// Output 2       : Chaine de caracteres choisie (string)
// Output 3       : Index de cette chaine dans le tableau en entree
static int ITFLUA_DlgGetValStringFromList(lua_State* L)
{
    char szFunc[] = "DlgGetValStringFromList";
    tolua_Error tolua_err;
    int dim;
    if (   !tolua_isstring(L, 1, 0, &tolua_err) 
        || !My_tolua_isstringarray (L, 2, &dim, 0, &tolua_err)
        || !tolua_isnoobj(L, 3, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    char* szDlgTitle;
    szDlgTitle = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));

    // on va maintenant recuperer les chaines (les index des tableaux LUA commencent a 1)
    std::vector<std::string> listValues;
    int i = 1;
    while (i <= dim)
    {
        listValues.push_back(tolua_tofieldstring(L, 2, i, DefaultEmptyString));
        ++i;
    }

    CDlgGetValFromList dlgGetVal(szDlgTitle, listValues);
    if (dlgGetVal.DoModal() != IDOK)
    {
        tolua_pushnumber(L, (double)STATUS_NOK);
        tolua_pushstring(L,(const char*)"");
        tolua_pushnumber(L, (double)0);
    }
    else
    {
        tolua_pushnumber(L, (double)STATUS_OK);
        // on retourne la chaine selectionnee
        tolua_pushstring(L,(const char*)dlgGetVal.GetSelectedValue().c_str());
        // Et l'indice de cette chaine dans la liste
        tolua_pushnumber(L,(double)(dlgGetVal.GetIdxValue()+1));  // les index des tableaux LUA commencent a 1
    }
    return 3;
}

// Recuperation de la longueur d'un fichier (en octets)
// Input Param 1  : Nom du fichier (string)
// Output         : Taille du fichier (-1 si le fichier n'existe pas) (nombre)
static int ITFLUA_GetFileLength(lua_State* L)
{
    char szFunc[] = "GetFileLength";
    tolua_Error tolua_err;
    if (   !tolua_isstring(L, 1, 0, &tolua_err) 
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* fileName   = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));

    long fileSize = -1;
    CAdeReadOnlyFile file;
    if (file.Open(fileName, O_BINARY))
    {
        fileSize = file.Length();
    }
    tolua_pushnumber(L, (double)fileSize);
    return 1;
}

// Appel de la fonction AdeWaitMs (temporisation) - plus précis que Sleep
// Input Param 1  : Valeur de la temporisation en ms (nombre)
// Output         : NA
static int ITFLUA_Sleep(lua_State* L)
{
    char szFunc[] = "Sleep";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err) 
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    DWORD dwMs = ((DWORD) tolua_tonumber(L, 1, 0));

    AdeWaitMs(dwMs);

    return 0;
}
// Lancement d'une application externe (appel de la fonction ShellExecute de l'API Windows)
// Input Param 1  : Nom de l'application a lancer (string) 
// Output         : NA
static int ITFLUA_ShellExecute(lua_State* L)
{
    char szFunc[] = "ShellExecute";
    tolua_Error tolua_err;
    if (   !tolua_isstring(L, 1, 0, &tolua_err) 
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szFileToExecute = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
    std::string strFileToExecute(szFileToExecute);
    // on subsitue un eventuel alias
    CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&strFileToExecute);
    ShellExecute(NULL, NULL, strFileToExecute.c_str(), NULL, NULL, SW_SHOWNORMAL);
    return 0;
}
// Positionnement d'un alias
// Input Param 1  : Nom de l'alias (string)
// Input Param 2  : Path (pouvant lui même commencer par un alias) (string)
// Output         : NA
static int ITFLUA_SetPathAlias(lua_State* L)
{
    char szFunc[] = "SetPathAlias";
    tolua_Error tolua_err;
    if (   !tolua_isstring(L, 1, 0, &tolua_err) 
        || !tolua_isstring(L, 2, 0, &tolua_err) 
        || !tolua_isnoobj(L, 3, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szPathAlias = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
    char* szPath      = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    std::string strPathAlias = szPathAlias;
    std::string strPath      = szPath;
    // On remplace un eventuel alias dans le path
    CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&strPath);
    // et on positionne l'alias
    CPathMgr::ReadSingleInstance()->SetAlias(strPathAlias, strPath);
    return 0;
}
// Similaire a la commande dofile de LUA mais en passant un nom de fichier qui peut commencer par un alias
static int ITFLUA_dofileWithAlias (lua_State* L) {
  const char* fname = luaL_optstring(L, 1, NULL);
  std::string fileName(fname);
  // On y remplace un eventuel ALIAS
  CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&fileName);

  int n = lua_gettop(L);
  if (luaL_loadfile(L, fileName.c_str()) != 0) lua_error(L);
  lua_call(L, 0, LUA_MULTRET);
  return lua_gettop(L) - n;
}

// Indication du début d'un scénario
// Input Param 1  : Nom du scénario
// Input Param 2  : Path du répertoire dans lequel mettre le log
// Output         : NA
// Note : 2eme argument est optionnel
static int ITFLUA_ScenarioBegin(lua_State* L)
{
    char szFunc[] = "ScenarioBegin";
    tolua_Error tolua_err;
    
    char* szPathDirLog = NULL;
    // on regarde s'il y a un 2eme argument
    if (!tolua_isnoobj(L, 2, &tolua_err))
    {
        // on verifie qu'il s'agit d'une chaine
        if (!tolua_isstring(L, 2, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        // On verifie qu'il n'y en a pas un troisieme
        if (!tolua_isnoobj(L, 3, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        // on recupere le path optionnel
        // Note : on ne substitue pas un eventuel alias ici, c'est fait dans StartScenario
        szPathDirLog = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    }

    if (!tolua_isstring(L, 1, 0, &tolua_err))
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szTitle = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));

    if (false == CTestMgr::ReadSingleInstance()->StartScenario(szTitle, szPathDirLog, szLastErrorText, sizeof(szLastErrorText)))
    {
        tolua_error(L, szLastErrorText, NULL);
        return 0;
    }
    return 0;
}

// Indication de la fin d'un scénario
// Input Param 1  : NA
// Output         : NA
static int ITFLUA_ScenarioEnd(lua_State* L)
{
    char szFunc[] = "ScenarioEnd";
    tolua_Error tolua_err;
    if (!tolua_isnoobj(L, 1, &tolua_err))
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    if (false == CTestMgr::ReadSingleInstance()->StopScenario(szLastErrorText, sizeof(szLastErrorText)))
    {
        tolua_error(L, szLastErrorText, NULL);
        return 0;
    }
    return 0;
}

// Indication du début d'un test case
// Input Param 1  : Nom du test case
// Output         : NA
static int ITFLUA_TestCaseBegin(lua_State* L)
{
    char szFunc[] = "TestCaseBegin";
    tolua_Error tolua_err;
    
    if (   !tolua_isstring(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szTitle = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));

    if (false == CTestMgr::ReadSingleInstance()->StartTestCase(szTitle, szLastErrorText, sizeof(szLastErrorText)))
    {
        tolua_error(L, szLastErrorText, NULL);
        return 0;
    }
    return 0;
}

// Indication de la fin d'un test case
// Input Param 1  : NA
// Output         : NA
static int ITFLUA_TestCaseEnd(lua_State* L)
{
    char szFunc[] = "TestCaseEnd";
    tolua_Error tolua_err;
    if (!tolua_isnoobj(L, 1, &tolua_err))
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    if (false == CTestMgr::ReadSingleInstance()->StopTestCase(szLastErrorText, sizeof(szLastErrorText)))
    {
        tolua_error(L, szLastErrorText, NULL);
        return 0;
    }
    return 0;
}

// Vérification d'une condition booléenne
// Input Param 1  : valeur du booléen à tester (attendu à true)
// Input Param 2  : commentaire sur le test
// Output         : true si test OK, false sinon 
// Note : 2eme argument est optionnel
static int ITFLUA_VerifyThat(lua_State *L)
{
    char szFunc[] = "VerifyThat";
    char szTmp[1000];
    char* szComment = NULL;
    tolua_Error tolua_err;
    luaL_checkany(L, 1);
    // on regarde s'il y a un 2eme argument
    if (!tolua_isnoobj(L, 2, &tolua_err))
    {
        // on verifie qu'il s'agit d'une chaine
        if (!tolua_isstring(L, 2, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        // On verifie qu'il n'y en a pas un troisieme
        if (!tolua_isnoobj(L, 3, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        // on recupere le commentaire optionnel
        szComment = ((char*) My_tolua_tostring(L, 2, DefaultEmptyString));
    }
    if (!tolua_isboolean(L, 1, 0, &tolua_err))
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    // on construit le message de verification
    lua_Debug ar;
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    const char* szSource = &ar.source[0];
    if ( ar.source[0] == '@' )
    {
        szSource = &ar.source[1];
    }
    std::string strSource(szSource);
    CPathMgr::ReadSingleInstance()->SubstitutePathWithAlias(&strSource);
    if (szComment)
    {
        sprintf_s(szTmp, sizeof(szTmp), "In line %d of %s [%s] => ", ar.currentline, strSource.c_str(), szComment);
    }
    else
    {
        sprintf_s(szTmp, sizeof(szTmp), "In line %d of %s => ", ar.currentline, strSource.c_str());
    }
    
    bool isCheckOK;
    if (!lua_toboolean(L, 1))
    {
        strcat_s(szTmp, sizeof(szTmp), "FAILED");
        CTestMgr::ReadSingleInstance()->PrintError(szTmp);
        isCheckOK = false;
    }
    else
    {
        strcat_s(szTmp, sizeof(szTmp), "OK");
        CTestMgr::ReadSingleInstance()->Print(szTmp);
        isCheckOK = true;
    }
    tolua_pushboolean(L, isCheckOK);
    return 1;
}

// Vérification qu'une valeur analogique est égale à la valeur attendue (+/- une précision)
// test que la valeur à tester est dans l'intervalle [va-p..va+p]
// va valeur attendue / p précision
// Input Param 1  : valeur analogique à tester
// Input Param 2  : valeur analogique attendue
// Input Param 3  : précision
// Input Param 4  : commentaire sur le test
// Output         : true si test OK, false sinon 
// Note : le 4eme argument est optionnel
static int ITFLUA_VerifyAnalog(lua_State *L)
{
    char szFunc[] = "VerifyAnalog";
    char szTmp[1000];
    char szTmpBis[200];
    char* szComment = NULL;
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err)
        || !tolua_isnumber(L, 2, 0, &tolua_err)
        || !tolua_isnumber(L, 3, 0, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    // on regarde s'il y a un 4eme argument
    if (!tolua_isnoobj(L, 4, &tolua_err))
    {
        // on verifie qu'il s'agit d'une chaine
        if (!tolua_isstring(L, 4, 0, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        // On verifie qu'il n'y en a pas un cinquieme
        if (!tolua_isnoobj(L, 5, &tolua_err))
        {
            return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
        }
        // on recupere le commentaire optionnel
        szComment = ((char*) My_tolua_tostring(L, 4, DefaultEmptyString));
    }
    // on construit le message de verification
    lua_Debug ar;
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    const char* szSource = &ar.source[0];
    if ( ar.source[0] == '@' )
    {
        szSource = &ar.source[1];
    }
    std::string strSource(szSource);
    CPathMgr::ReadSingleInstance()->SubstitutePathWithAlias(&strSource);
    if (szComment)
    {
        sprintf_s(szTmp, sizeof(szTmp), "In line %d of %s [%s] => ", ar.currentline, strSource.c_str(), szComment);
    }
    else
    {
        sprintf_s(szTmp, sizeof(szTmp), "In line %d of %s => ", ar.currentline, strSource.c_str());
    }
    
    double value         = tolua_tonumber(L, 1, 0);
    double expectedValue = tolua_tonumber(L, 2, 0);
    double precision     = tolua_tonumber(L, 3, 0);
    bool isCheckOK;
    sprintf_s(szTmpBis, sizeof(szTmpBis), " Expected: %0.f (+/-%0.f) Obtained: %0.f", expectedValue, precision, value);
    if (   (value > (expectedValue+precision))
        || (value < (expectedValue-precision))
       )
    {
        strcat_s(szTmp, sizeof(szTmp), "FAILED");
        strcat_s(szTmp, sizeof(szTmp), szTmpBis);
        CTestMgr::ReadSingleInstance()->PrintError(szTmp);
        isCheckOK = false;
    }
    else
    {
        strcat_s(szTmp, sizeof(szTmp), "OK");
        strcat_s(szTmp, sizeof(szTmp), szTmpBis);
        CTestMgr::ReadSingleInstance()->Print(szTmp);
        isCheckOK = true;
    }
    tolua_pushboolean(L, isCheckOK);
    return 1;
}

// Armement d'un timer
// Input Param 1  : nom du timer
// Input Param 2  : valeur du timeout en ms
// Output         : NA
static int ITFLUA_TimerArm(lua_State* L)
{
    char szFunc[] = "TimerArm";
    tolua_Error tolua_err;
    
    if (   !tolua_isstring(L, 1, 0, &tolua_err)
        || !tolua_isnumber(L, 2, 0, &tolua_err)
        || !tolua_isnoobj(L, 3, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szTimer = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
    DWORD dwMs = ((DWORD) tolua_tonumber(L, 2, 0));

    if (false == CTestMgr::ReadSingleInstance()->TimerArm(szTimer, dwMs, szLastErrorText, sizeof(szLastErrorText)))
    {
        tolua_error(L, szLastErrorText, NULL);
        return 0;
    }
    return 0;
}

// Test d'un timer
// Input Param 1  : nom du timer
// Output 1       : true si le timer a expire, false sinon
// Output 2       : temps passé depuis l'armement du timer (en ms)
static int ITFLUA_TimerIsExpired(lua_State* L)
{
    char szFunc[] = "TimerIsExpired";
    tolua_Error tolua_err;
    
    if (   !tolua_isstring(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szTimer = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));
    bool isExpired;
    DWORD elapsedSinceArmMs;
    if (false == CTestMgr::ReadSingleInstance()->TimerIsExpired(szTimer, &isExpired, &elapsedSinceArmMs, szLastErrorText, sizeof(szLastErrorText)))
    {
        tolua_error(L, szLastErrorText, NULL);
        return 0;
    }
    else
    {
        tolua_pushboolean(L, isExpired);
        tolua_pushnumber(L, (double)elapsedSinceArmMs);
        return 2;
    }
}

// Attente de l'expiration d'un timer
// Input Param 1  : nom du timer
// Output         : NA
static int ITFLUA_TimerWaitUntilExpired(lua_State* L)
{
    char szFunc[] = "TimerWaitUntilExpired";
    tolua_Error tolua_err;
    
    if (   !tolua_isstring(L, 1, 0, &tolua_err)
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    char* szTimer = ((char*) My_tolua_tostring(L, 1, DefaultEmptyString));

    if (false == CTestMgr::ReadSingleInstance()->TimerWaitUntilExpired(szTimer, szLastErrorText, sizeof(szLastErrorText)))
    {
        tolua_error(L, szLastErrorText, NULL);
        return 0;
    }
    return 0;
}
// Obtention du temps ecoulé en ms depuis le démarrage du système
// Input Param   : NA
// Output        : temps ecoulé en ms depuis le démarrage du système
static int ITFLUA_GetTickCountMs(lua_State* L)
{
    char szFunc[] = "GetTickCountMs";
    tolua_Error tolua_err;
    if (!tolua_isnoobj(L, 1, &tolua_err))
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    tolua_pushnumber(L, (double)AdeGetTickCountMs());
    return 1;
}

// Renvoie la version d'OngTV (sous forme de chaîne de caractères)
// Input Param   : NA
// Output        : Version d'adeview "X.Y.Z"
static int ITFLUA_GetOngTVVersion(lua_State* L)
{
    char szFunc[] = "GetOngTVVersion";
    tolua_Error tolua_err;
    if (!tolua_isnoobj(L, 1, &tolua_err))
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }
    tolua_pushstring(L, theApp.m_versionInfos.GetProductVersion().c_str());
    return 1;
}


typedef struct {
    char*   name;
    double  value;
} T_CONSTANT;

typedef struct {
    char*         name;
    lua_CFunction function;
} T_FUNCTION;

// tableau dans lequel on declare les constantes que l'on veut que l'interpreteur LUA reconnaisse
static T_CONSTANT arrCst[] =
{
    {"STATUS_OK",                         STATUS_OK},
    {"STATUS_NOK",                        STATUS_NOK},
    {"STATUS_OVERFLOW",                   STATUS_OVERFLOW},
    {"STATUS_CNX_NOT_OPENED",             STATUS_CNX_NOT_OPENED},
    {"STATUS_NO_FRAME_RECEIVED",          STATUS_NO_FRAME_RECEIVED},
    {"STATUS_CRC_ERROR",                  STATUS_CRC_ERROR},
    {"MB_ABORTRETRYIGNORE",               MB_ABORTRETRYIGNORE},
    {"MB_CANCELTRYCONTINUE",              MB_CANCELTRYCONTINUE},
    {"MB_HELP",                           MB_HELP},
    {"MB_OK",                             MB_OK},
    {"MB_OKCANCEL",                       MB_OKCANCEL},
    {"MB_RETRYCANCEL",                    MB_RETRYCANCEL},
    {"MB_YESNO",                          MB_YESNO},
    {"MB_YESNOCANCEL",                    MB_YESNOCANCEL},
    {"MB_ICONEXCLAMATION",                MB_ICONEXCLAMATION},
    {"MB_ICONWARNING",                    MB_ICONWARNING},
    {"MB_ICONINFORMATION",                MB_ICONINFORMATION},
    {"MB_ICONASTERISK",                   MB_ICONASTERISK},
    {"MB_ICONQUESTION",                   MB_ICONQUESTION},
    {"MB_ICONSTOP",                       MB_ICONSTOP},
    {"MB_ICONERROR",                      MB_ICONERROR},
    {"MB_ICONHAND",                       MB_ICONHAND},
    {"MB_DEFBUTTON1",                     MB_DEFBUTTON1},
    {"MB_DEFBUTTON2",                     MB_DEFBUTTON2},
    {"MB_DEFBUTTON3",                     MB_DEFBUTTON3},
    {"MB_DEFBUTTON4",                     MB_DEFBUTTON4},
    {"IDABORT",                           IDABORT},
    {"IDCANCEL",                          IDCANCEL},
    {"IDCONTINUE",                        IDCONTINUE},
    {"IDIGNORE",                          IDIGNORE},
    {"IDNO",                              IDNO},
    {"IDOK",                              IDOK},
    {"IDRETRY",                           IDRETRY},
    {"IDTRYAGAIN",                        IDTRYAGAIN},
    {"IDYES",                             IDYES},
};

// Obtention de la chaine de caractères correspondant au code d'erreur passé en paramètre
// Input Param 1  : Code d'erreur
// Output         : Chaine correspondante
static int ITFLUA_GetErrorText(lua_State* L)
{
    char szFunc[] = "GetErrorText";
    tolua_Error tolua_err;
    if (   !tolua_isnumber(L, 1, 0, &tolua_err) 
        || !tolua_isnoobj(L, 2, &tolua_err)
       )
    {
        return BuildBadArgTypeErrorMsg(L, szFunc, &tolua_err);
    }

    double errorCode = tolua_tonumber(L, 1, 0);

    // on le recherche dans le tableau des constantes
    for (unsigned i = 0 ; i < NB_ELEMS_OF(arrCst) ; ++i)
    {
        if (arrCst[i].value == errorCode)
        {
            // Ok, on l'a trouvé, on vérifie juste que c'est un code d'erreur (ie commence par STATUS_)
            if (0 == strncmp("STATUS_", arrCst[i].name, strlen("STATUS_")))
            {
                // OK 
                tolua_pushstring(L, (const char*)arrCst[i].name);
                return 1;
            }
        }
    }

    // pas trouve
    tolua_pushstring(L, (const char*)"Unknown error code");
    return 1;
}


// tableau dans lequel on declare les fonctions que l'on veut pouvoier appeler depuis
// LUA
static T_FUNCTION arrFct[] = {
    {"OpenCnx",                           ITFLUA_OpenCnx},
    {"CloseCnx",                          ITFLUA_CloseCnx},
    {"SetUByteInTxBuffer",                ITFLUA_SetUByteInTxBuffer},
    {"SetUShortInTxBuffer",               ITFLUA_SetUShortInTxBuffer},
    {"SetULongInTxBuffer",                ITFLUA_SetULongInTxBuffer},
    {"GetUByteInRxBuffer",                ITFLUA_GetUByteInRxBuffer},
    {"GetUShortInRxBuffer",               ITFLUA_GetUShortInRxBuffer},
    {"GetULongInRxBuffer",                ITFLUA_GetULongInRxBuffer},
    {"GetFileLength",                     ITFLUA_GetFileLength},
    {"SendFrame",                         ITFLUA_SendFrame},
    {"SendRawFrame",                      ITFLUA_SendRawFrame},
    {"ReceiveFrame",                      ITFLUA_ReceiveFrame},
    {"Sleep",                             ITFLUA_Sleep},
    {"WaitMs",                            ITFLUA_Sleep},    // WaitMs synonime de Sleep
    {"FileOpenDialog",                    ITFLUA_FileOpenDialog},
    {"FileSaveDialog",                    ITFLUA_FileSaveDialog},
    {"MessageBox",                        ITFLUA_MessageBox},
    {"ShellExecute",                      ITFLUA_ShellExecute},
    {"dofileWithAlias",                   ITFLUA_dofileWithAlias},
    {"SetPathAlias",                      ITFLUA_SetPathAlias},
    {"DlgGetValString",                   ITFLUA_DlgGetValString},
    {"DlgGetValNum",                      ITFLUA_DlgGetValNum},
    {"DlgGetValStringFromList",           ITFLUA_DlgGetValStringFromList},

    {"ScenarioBegin",                     ITFLUA_ScenarioBegin},
    {"ScenarioEnd",                       ITFLUA_ScenarioEnd},
    {"TestCaseBegin",                     ITFLUA_TestCaseBegin},
    {"TestCaseEnd",                       ITFLUA_TestCaseEnd},
    {"VerifyThat",                        ITFLUA_VerifyThat},
    {"VerifyAnalog",                      ITFLUA_VerifyAnalog},
    {"TimerArm",                          ITFLUA_TimerArm},
    {"TimerIsExpired",                    ITFLUA_TimerIsExpired},
    {"TimerWaitUntilExpired",             ITFLUA_TimerWaitUntilExpired},
    {"GetTickCountMs",                    ITFLUA_GetTickCountMs},
    {"GetErrorText",                      ITFLUA_GetErrorText},
    {"GetOngTVVersion",                   ITFLUA_GetOngTVVersion},
};

/* Open lib function */
/*LUALIB_API*/ int luaopen_ITFLUA (lua_State* L)
{
    tolua_open(L);
    tolua_reg_types(L);
    tolua_module(L,NULL,0);
    tolua_beginmodule(L,NULL);

    // declaration des constantes
    for (unsigned i = 0 ; i < NB_ELEMS_OF(arrCst) ; ++i)
    {
        tolua_constant(L, arrCst[i].name, arrCst[i].value);
    }
    // declaration des fonctions
    for (unsigned i = 0 ; i < NB_ELEMS_OF(arrFct) ; ++i)
    {
        tolua_function(L, arrFct[i].name, arrFct[i].function);
    }
    
    tolua_endmodule(L);
    return 1;
}

/* Open tolua function */
void ITFLUA_open (lua_State* L)
{
    lua_pushcfunction(L, luaopen_ITFLUA);
    lua_pushstring(L, "ITFLUA");
    lua_call(L, 1, 0);
}

/* close */
void ITFLUA_close(void)
{
    // On ferme une éventuelle connexion
    pCnx.reset();
    CTestMgr::DeleteSingleInstance();
}

// fonction appellee dans luaView.cpp pour recuperer les mots clefs pour lesquels
// on veut une coloration syntaxique
// on ajoute a la chaine chacun des mots clefs precede par un espace
// fonction appellee dans luaView.cpp pour recuperer les mots clefs pour lesquels
// on veut une coloration syntaxique
// on ajoute a la chaine chacun des mots clefs precede par un espace
void ITFLUA_getKeywords (std::string *pStrFcts, std::string *pStrCsts)
{
    for (unsigned i = 0 ; i < NB_ELEMS_OF(arrCst) ; ++i)
    {
        *pStrCsts += " ";
        *pStrCsts += arrCst[i].name;
    }
    for (unsigned i = 0 ; i < NB_ELEMS_OF(arrFct) ; ++i)
    {
        *pStrFcts += " ";
        *pStrFcts += arrFct[i].name;
    }

}

}// extern "C"