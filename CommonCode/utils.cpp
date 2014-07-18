////////////////////////////////////////////////////////////////////////////////
/// @addtogroup UTILS
/// Fonctions et objets utiles appellées par le framework de maintenance
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Fonctions utiles appellées par le framework de maintenance
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 395 $
/// @date $Date: 2014-07-09 17:51:30 +0200 (mer., 09 juil. 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/utils.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeset / Librairie)

#include <string>
#include <sstream>
#include <strstream>
#include <windows.h>
#include <time.h>
#include <boost/smart_ptr.hpp>
#include <boost/unique_ptr.hpp>

#include "AdeNonCopyable.h"
#include "utils.h"
#include "AdeTrace.h"


////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                             Prototypes des fonctions exportees
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// Obtention du temps ecoulé en ms depuis le demarrage du system
/// C'est un equivalent de la fonction GetTickCount de Windows mais en beaucoup
/// plus précis (GetTickCount a une précision de quelques ms)
/// 
/// Parametres :
///
/// \return time_t correspondant
////////////////////////////////////////////////////////////////////////////////
DWORD AdeGetTickCountMs(void)
{
    static LARGE_INTEGER Frequency;
    static bool bInit = false;
    if (false == bInit)
    {
        QueryPerformanceFrequency(&Frequency);
        // On passe en KHz (ms)
        Frequency.QuadPart = Frequency.QuadPart /  1000;
        bInit = true;
    }
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter(&currentCount) ;
    return (DWORD)(currentCount.QuadPart / Frequency.QuadPart);
}

////////////////////////////////////////////////////////////////////////////////
/// Attente d'un nombre de ms
/// C'est un equivalent de la fonction Sleep de Windows mais en
/// plus précis (Sleep a une précision de quelques ms)
/// Parametres :
/// dwMs        Nombre de ms
////////////////////////////////////////////////////////////////////////////////
void AdeWaitMs(DWORD dwMs)
{
    // pour essayer d'avoir le maximum de precision, on rajoute une petite boucle pour faire le bon delai
    // (le tick de windows est de l'ordre de 10/15 ms)
    DWORD expirationDate = AdeGetTickCountMs() + dwMs;
    if (dwMs > 30)
    {
        // on commence par un sleep pour ne pas monopoliser le CPU si on a a attendre plus de 30ms (temps superieur
        // a 2*tick de l'OS)
        Sleep(dwMs-30); // on se garde une marge de 30ms pour etre precis
    }
    // puis on termine par un polling
    while (AdeGetTickCountMs() < expirationDate) {}
}


////////////////////////////////////////////////////////////////////////////////
/// Obtention d'un temps UTC a partir d'un nombre de seconde depuis 2000 (format
/// de stockage sur l'embarque pour les datations (evt, pert, ...)
/// 
/// Parametres :
/// \param m_NbSecSince2000  temps UTC exprime en nombre de seconde depuis 2000
///
/// \return time_t correspondant
////////////////////////////////////////////////////////////////////////////////
#define NB_SEC_FROM_1970_TO_2000    946684800ul     // nombre de seconde entre le 1/01/1970 00:00 et le 1/01/200 00:00
                                                    // 1/01/1970 00:00 <=> time_t 0
time_t ConvertNbSecToTime(unsigned long  m_NbSecSince2000)
{
    return m_NbSecSince2000 + NB_SEC_FROM_1970_TO_2000;
}

////////////////////////////////////////////////////////////////////////////////
/// Renvoie le data/heure courante sous forme de string
/// 
/// \return chaine representant la date et l'heure
////////////////////////////////////////////////////////////////////////////////
std::string GetCurrentDateTimeAsString(void)
{
    return GetCurrentDateTimeAsString("%c");
}

////////////////////////////////////////////////////////////////////////////////
/// Renvoie le data/heure courante sous forme de string
/// 
/// \return chaine representant la date et l'heure
////////////////////////////////////////////////////////////////////////////////
std::string GetCurrentDateTimeAsString(char *format)
{
    // On recupere la date/heure courante
    setlocale( LC_ALL, "" );
    time_t current = time(NULL);
    tm loct;
    localtime_s(&loct, &current);
    char szTmp[100];
    // on recupere une chaine formattee
    strftime(szTmp, sizeof(szTmp), format, &loct);
    return(std::string(szTmp));
}

////////////////////////////////////////////////////////////////////////////////
// Conversion d'une string representant le contenu d'un buffer en ce buffer
// C'est l'operation inverse de HexaToString (mais avec des params d'entree differents
/// 
/// Parametres :
/// \param str         chaine a convertir
/// \param pBuffer     buffer
///
/// \return true si conversion reussie
////////////////////////////////////////////////////////////////////////////////
bool CnvStrToBuffer(const std::string& str, boost::unique_ptr<unsigned char[]>* pBuffer)
{
    unsigned strSize = str.size();
    if (0 == strSize)
    {
        // taille de chaine nulle => pb
        return false;
    }
    if (strSize & 1)
    {
        // taille de chaine impaire => pb
        return false;
    }
    unsigned bufferSize = strSize/2;    // 2 caracteres par octets
    pBuffer->reset(new unsigned char[bufferSize]);
    if (NULL == pBuffer->get())
    {
        //pb d'allocation
        return false;
    }
    for (unsigned i = 0; i < bufferSize; ++i)
    {
        switch (str[2*i])
        {
        case '0': (*pBuffer)[i] = 0x00; break;;
        case '1': (*pBuffer)[i] = 0x10; break;
        case '2': (*pBuffer)[i] = 0x20; break;
        case '3': (*pBuffer)[i] = 0x30; break;
        case '4': (*pBuffer)[i] = 0x40; break;
        case '5': (*pBuffer)[i] = 0x50; break;
        case '6': (*pBuffer)[i] = 0x60; break;
        case '7': (*pBuffer)[i] = 0x70; break;
        case '8': (*pBuffer)[i] = 0x80; break;
        case '9': (*pBuffer)[i] = 0x90; break;
        case 'A':
        case 'a': (*pBuffer)[i] = 0xa0; break;
        case 'B':
        case 'b': (*pBuffer)[i] = 0xb0; break;
        case 'C':
        case 'c': (*pBuffer)[i] = 0xc0; break;
        case 'D':
        case 'd': (*pBuffer)[i] = 0xd0; break;
        case 'E':
        case 'e': (*pBuffer)[i] = 0xe0; break;
        case 'F':
        case 'f': (*pBuffer)[i] = 0xf0; break;
        default: // caractere errone => on sort
            return false;
        }
        switch (str[(2*i)+1])
        {
        case '0': break;
        case '1': (*pBuffer)[i] |= 0x1; break;
        case '2': (*pBuffer)[i] |= 0x2; break;
        case '3': (*pBuffer)[i] |= 0x3; break;
        case '4': (*pBuffer)[i] |= 0x4; break;
        case '5': (*pBuffer)[i] |= 0x5; break;
        case '6': (*pBuffer)[i] |= 0x6; break;
        case '7': (*pBuffer)[i] |= 0x7; break;
        case '8': (*pBuffer)[i] |= 0x8; break;
        case '9': (*pBuffer)[i] |= 0x9; break;
        case 'A':
        case 'a': (*pBuffer)[i] |= 0xa; break;
        case 'B':
        case 'b': (*pBuffer)[i] |= 0xb; break;
        case 'C':
        case 'c': (*pBuffer)[i] |= 0xc; break;
        case 'D':
        case 'd': (*pBuffer)[i] |= 0xd; break;
        case 'E':
        case 'e': (*pBuffer)[i] |= 0xe; break;
        case 'F':
        case 'f': (*pBuffer)[i] |= 0xf; break;
        default: // caractere errone => on sort
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Convertit une chaine representant un nombre decimal non signe
/// 
/// Parametres :
/// \param str         chaine a convertir
/// \param res         resulat
///
/// \return true si conversion reussie
////////////////////////////////////////////////////////////////////////////////
bool CnvStrToULong(const std::string& str, unsigned long* pRes)
{
    unsigned idxCurr = 0;
    unsigned strSize = str.size();
    unsigned long result = 0;

    // suppression des blancs en queue et en tete
    while ((strSize) && (' ' == str[strSize-1]))
    {
        --strSize;
    }
    while ((strSize) && (' ' == str[idxCurr]))
    {
        --strSize;
        ++idxCurr;
    }

    if (strSize == 0)
    {
        // pas assez de digit dans la chaine
        return false;
    }

    while (strSize)
    {
        char digit = str[idxCurr];
        result = result * 10;
        switch (digit)
        {
        case '0': break;
        case '1': result += 1; break;
        case '2': result += 2; break;
        case '3': result += 3; break;
        case '4': result += 4; break;
        case '5': result += 5; break;
        case '6': result += 6; break;
        case '7': result += 7; break;
        case '8': result += 8; break;
        case '9': result += 9; break;
        default: // caractere errone => on sort
            return false;
        }
        ++idxCurr;
        --strSize;
    }
    // la conversion s'est bien deroulee
    // on affecte le resultat
    *pRes = result;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Convertit une chaine representant un nombre hexa
/// La chaine hexa doit etre au format [0x]HHHH : il doit y avoir au plus
/// nbMaxDigits caractères (Ox exclu) et au moins un caractere (0x exclu)
/// 
/// Parametres :
/// \param str         chaine a convertir
/// \param res         resulat
/// \param nbMaxDigits nombre max de digit
///
/// \return true si conversion reussie
////////////////////////////////////////////////////////////////////////////////
bool CnvStrToHex(const std::string& str, unsigned long* pRes, unsigned nbMaxDigits /*= 8*/)
{
    unsigned idxCurr = 0;
    unsigned strSize = str.size();
    unsigned long result = 0;

    // suppression des blancs en queue et en tete
    while ((strSize) && (' ' == str[strSize-1]))
    {
        --strSize;
    }
    while ((strSize) && (' ' == str[idxCurr]))
    {
        --strSize;
        ++idxCurr;
    }

    // on commence par passer un eventuel 0x en debut de chaine
    if (strSize > 2)
    {
        if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')))
        {
            // on les passe
            idxCurr = 2;
            strSize = strSize - 2;
        }
    }
    if (strSize > nbMaxDigits)
    {
        // trop de digit dans la chaine
        return false;
    }
    if (strSize == 0)
    {
        // pas assez de digit dans la chaine
        return false;
    }

    while (strSize)
    {
        char digit = str[idxCurr];
        result = result << 4;
        switch (digit)
        {
        case '0': break;
        case '1': result |= 0x1; break;
        case '2': result |= 0x2; break;
        case '3': result |= 0x3; break;
        case '4': result |= 0x4; break;
        case '5': result |= 0x5; break;
        case '6': result |= 0x6; break;
        case '7': result |= 0x7; break;
        case '8': result |= 0x8; break;
        case '9': result |= 0x9; break;
        case 'A':
        case 'a': result |= 0xa; break;
        case 'B':
        case 'b': result |= 0xb; break;
        case 'C':
        case 'c': result |= 0xc; break;
        case 'D':
        case 'd': result |= 0xd; break;
        case 'E':
        case 'e': result |= 0xe; break;
        case 'F':
        case 'f': result |= 0xf; break;
        default: // caractere errone => on sort
            return false;
        }
        ++idxCurr;
        --strSize;
    }
    // la conversion s'est bien deroulee
    // on affecte le resultat
    *pRes = result;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Convertit une chaine representant un nombre binaire
/// La chaine hexa doit etre au format BBBB : il doit y avoir au plus
/// nbMaxDigits caractères et au moins un caractere 
/// 
/// Parametres :
/// \param str         chaine a convertir
/// \param res         resulat
/// \param nbMaxDigits nombre max de digit
///
/// \return true si conversion reussie
////////////////////////////////////////////////////////////////////////////////
bool CnvStrToBin(const std::string& str, unsigned long* pRes, unsigned nbMaxDigits /*= 32*/)
{
    unsigned idxCurr = 0;
    unsigned strSize = str.size();
    unsigned long result = 0;

    // suppression des blancs en queue et en tete
    while ((strSize) && (' ' == str[strSize-1]))
    {
        --strSize;
    }
    while ((strSize) && (' ' == str[idxCurr]))
    {
        --strSize;
        ++idxCurr;
    }

    if (strSize > nbMaxDigits)
    {
        // trop de digit dans la chaine
        return false;
    }
    if (strSize == 0)
    {
        // pas assez de digit dans la chaine
        return false;
    }

    while (strSize)
    {
        char digit = str[idxCurr];
        result = result << 1;
        switch (digit)
        {
        case '0': break;
        case '1': result |= 0x1; break;
        default: // caractere errone => on sort
            return false;
        }
        ++idxCurr;
        --strSize;
    }
    // la conversion s'est bien deroulee
    // on affecte le resultat
    *pRes = result;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
///	hexaToString
/// \param buffer : tableau d'octets à convertir
/// \param buffSize : taille du tableau d'octets
///
/// \return string correspondante
////////////////////////////////////////////////////////////////////////////////
std::string HexaToString(const unsigned char* buff, unsigned long buffSize)
{
    boost::scoped_array<char> sz;  //chaine en sortie
    sz.reset(new char[buffSize*2+1]);   // taille de la chaine en sortie = taille du buffer * 2 + zero terminal

    HexaToString(buff, buffSize, sz.get());
    return std::string(sz.get());
}

////////////////////////////////////////////////////////////////////////////////
///	hexaToString
/// \param buffer : tableau d'octets à convertir
/// \param buffSize : taille du tableau d'octets
///
/// \return string correspondante
////////////////////////////////////////////////////////////////////////////////
void HexaToString(const unsigned char* buff, unsigned long buffSize, char* sz)
{
    static char arrByteToHex[256][3];
    static bool bInit = false;

    if (!bInit)
    {
        // Init du tableau de correspondance
        for (int i=0; i<256; ++i)
        {
            sprintf_s(arrByteToHex[i], sizeof(arrByteToHex[0]), "%02X", i);
        }

    }
    const unsigned char *pBuff = buff;
    int i;
    for (i = 0; buffSize > 0; --buffSize, ++pBuff, i+=2)
    {
        sz[i] = arrByteToHex[*pBuff][0];
        sz[i+1] = arrByteToHex[*pBuff][1];
    }

    // Ajout du 0 terminal
    sz[i] = '\0';
}

////////////////////////////////////////////////////////////////////////////////
///	Conversion d'une string representant une version X.Y.Z en ces n° de versions
/// \param s  :     string de laquelle extraire les X,Y,Z
/// \param pX :     pointeur sur X
/// \param pY :     pointeur sur Y
/// \param pZ :     pointeur sur Z
///
/// \return true si la conversion a reussie
////////////////////////////////////////////////////////////////////////////////
bool Str2Version(const std::string& s, unsigned char* pX, unsigned char* pY,unsigned char* pZ)
{
    bool res = true;

    std::string strX, strY, strZ;
    size_t pos1;
    size_t pos2;

    pos1 = s.find(".");    // position of "live" in str
    if (pos1!=std::string::npos)
    {
        strX= s.substr(0,pos1);

    }
    else
    {
        res = false;
    }

    pos2 = s.find(".",pos1+1);
    if (res && pos2!=std::string::npos && (pos2)>(pos1+1))
    {
        strY= s.substr(pos1+1,pos2-pos1-1);
    }
    else
    {
        res = false;
    }

    if(res && pos2+1<s.length())
    {
        strZ= s.substr(pos2+1,s.length()-pos2-1);
    }
    else
    {
        res = false;
    }

    if (res)
    {
        *pX = atoi(strX.c_str());
        *pY = atoi(strY.c_str());
        *pZ = atoi(strZ.c_str());
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////
/// Convertit une chaine en minuscule
/// 
/// Parametres :
/// \param str         chaine a convertir
////////////////////////////////////////////////////////////////////////////////
void ToLower(std::string* pStr)
{
    for (unsigned i = 0; i < (*pStr).size(); ++i)
    {
        char car = (*pStr)[i];
        if (('A' <= car) && (car <= 'Z'))
        {
            car -= 'A';
            car += 'a';
        }
        (*pStr)[i] = car;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Convertit une chaine en majuscule
/// 
/// Parametres :
/// \param str         chaine a convertir
////////////////////////////////////////////////////////////////////////////////
void ToUpper(std::string* pStr)
{
    for (unsigned i = 0; i < (*pStr).size(); ++i)
    {
        char car = (*pStr)[i];
        if (('a' <= car) && (car <= 'z'))
        {
            car -= 'a';
            car += 'A';
        }
        (*pStr)[i] = car;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// CRC16 x^16 + x^15 + x^2 + 1
////////////////////////////////////////////////////////////////////////////////

///< Table of CRC values for high-order byte
static const unsigned char auchCRCHi[] =
{
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ; 


///< Table of CRC values for low-order byte
static const unsigned char auchCRCLo[] =
{
 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;

////////////////////////////////////////////////////////////////////////////////
/// Calcul du CRC du buffer passe en parametre
///
/// \param pBuff       pointeur sur le premier caractere du buffer
/// \param buffSize    longueur du buffer
///
/// \return CRC calcule 
///
////////////////////////////////////////////////////////////////////////////////
unsigned short CRC16_Compute(unsigned char* pBuff, unsigned buffSize)
{
    unsigned char  uchCRCHi = 0x0;   // high CRC byte initialized;
    unsigned char  uchCRCLo = 0x0;   // low CRC byte initialized;
    unsigned uIndex;        // will index into CRC lookup table
    unsigned char curr;

    while (buffSize--)     // pass through message buffer  
    {
        curr = *pBuff++;
        uIndex = uchCRCHi ^ curr; // calculate the CRC
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex];
    }
    return uchCRCHi << 8 | uchCRCLo;
}
////////////////////////////////////////////////////////////////////////////////
/// CRC16 type CCITT x^16 + x^12 + x^5 + 1
////////////////////////////////////////////////////////////////////////////////

///< Table of CRC values for high-order byte
static const unsigned short crc16_ccitt_tab[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0, 
};

////////////////////////////////////////////////////////////////////////////////
/// Calcul du CRC du buffer passe en parametre
///
/// \param pBuff       pointeur sur le premier caractere du buffer
/// \param buffSize    longueur du buffer
///
/// \return CRC calcule 
///
////////////////////////////////////////////////////////////////////////////////
unsigned short CRC16_CCITT_Compute(unsigned char *pBuff, unsigned long buffSize)
{
	// initialisation
    unsigned short crc16_ccitt = 0xFFFFu;
	
    while (buffSize--) 
	{
		crc16_ccitt = crc16_ccitt_tab[((crc16_ccitt >> 8) ^ *pBuff) & 0xFFu] ^ (crc16_ccitt << 8);
        ++pBuff;
    }

    return crc16_ccitt;
}

////////////////////////////////////////////////////////////////////////////////
/// CRC32 x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + 1
////////////////////////////////////////////////////////////////////////////////

// The CRC parameters.  Currently configured for CCITT.
// Simply modify these to switch to another CRC standard.

#define POLYNOMIAL          0x04C11DB7
#define INITIAL_REMAINDER   0xFFFFFFFF
#define FINAL_XOR_VALUE     0xFFFFFFFF


#define WIDTH   (8 * sizeof(unsigned long))
#define TOPBIT  (1 << (WIDTH - 1))

// An array containing the pre-computed intermediate result for each
// possible byte of input.  This is used to speed up the computation.
static unsigned long  crcTable[256];

////////////////////////////////////////////////////////////////////////////////
/// Initialize the CRC lookup table.  This table is used
/// by crcCompute() to make CRC computation faster. 
///
/// \param 
///
/// \return
///
////////////////////////////////////////////////////////////////////////////////
static void CRC32_Init(void)
{
    static bool bInit = false;
    if (bInit)
    {
        return;
    }
    bInit = true;

    unsigned long  remainder;              
    unsigned long  dividend;              
    int    bit;                  

    // Perform binary long division, a bit at a time.
    for (dividend = 0; dividend < 256; ++dividend)
    {
        
        // Initialize the remainder.
        remainder = dividend << (WIDTH - 8);

        
        // Shift and XOR with the polynomial.
        for (bit = 0; bit < 8; ++bit)
        {
            // Try to divide the current data bit.
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = remainder << 1;
            }
        }

        // Save the result in the table.
        crcTable[dividend] = remainder;
    }
} 

////////////////////////////////////////////////////////////////////////////////
/// Calcul du CRC du buffer passe en parametre
///
/// \param pMessage     buffer to compute
/// \param nBytes       amount of data to compute
///
/// \return CRC calcule 
///
////////////////////////////////////////////////////////////////////////////////
unsigned long CRC32_Compute(unsigned char* pBuff, unsigned buffSize)
{
    unsigned char   byte;
    unsigned long  remainder = INITIAL_REMAINDER;
    unsigned char curr;

    CRC32_Init();

    while (buffSize--)     // pass through message buffer  
    {
        curr = *pBuff++;
        byte = (unsigned char) ((remainder >> (WIDTH - 8)) ^ curr);
        remainder = crcTable[byte] ^ (remainder << 8);
    }

     // The final remainder is the CRC result.
    return remainder ^ FINAL_XOR_VALUE;
}

unsigned long CRC32_InitRemainder(void)
{
    CRC32_Init();
    return INITIAL_REMAINDER;
}
void CRC32_Add(unsigned char* pBuff, unsigned buffSize, unsigned long* pRemainder)
{
    unsigned char byte;
    unsigned char curr;
    while (buffSize--)     // pass through message buffer  
    {
        curr = *pBuff++;
        byte = (unsigned char) (((*pRemainder) >> (WIDTH - 8)) ^ curr);
        *pRemainder = crcTable[byte] ^ ((*pRemainder) << 8);
    }
}
unsigned long CRC32_GetFinal(unsigned long remainder)
{
     // The final remainder is the CRC result.
    return remainder ^ FINAL_XOR_VALUE;
}

#define MAX_IMBRICARE_START 10
static DWORD arrStartTimer[MAX_IMBRICARE_START];
static unsigned nbStart = 0;

void StartTimeCount(void)
{
    char szFunc[] = "StartTimeCount";
    if (nbStart >= MAX_IMBRICARE_START)
    {
        ADE_ERROR("Too much imbricate start");
    }
    else
    {
        arrStartTimer[nbStart] = AdeGetTickCountMs();
        ++nbStart;
    }
}

void EndTimeCount(char*  Text)
{
    char szFunc[] = "EndTimeCount";
    if (nbStart == 0)
    {
        ADE_ERROR("No corresponding start time");
    }
    else
    {
        --nbStart;
        DWORD elapsed = AdeGetTickCountMs() - arrStartTimer[nbStart];
        ADE_WARNING(Text << " " << elapsed << " ms");
    }
}


////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/utils.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///

