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
/// @version $Revision: 86 $
/// @date $Date: 2014-05-12 16:10:43 +0200 (lun., 12 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/utils.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef UTILS_H
#define UTILS_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

/// Obtention du temps ecoulé en ms depuis le demarrage du system
/// C'est un equivalent de la fonction GetTickCount de Windows mais en beaucoup
/// plus précis (GetTickCount a une précision de quelques ms)
DWORD AdeGetTickCountMs(void);

/// Attente d'un nombre de ms
/// C'est un equivalent de la fonction Sleep de Windows mais en
/// plus précis (Sleep a une précision de quelques ms)
void AdeWaitMs(DWORD dwMs);

// Obtention d'un temps UTC a partir d'un nombre de seconde depuis 2000 (format
// de stockage sur l'embarque pour les datations (evt, pert, ...)
time_t ConvertNbSecToTime(unsigned long  m_NbSecSince2000);

// fonction template de conversion d'un type vers l'autre
template <class out_type, class in_value>
out_type cast_stream(const in_value & t)
{
    stringstream ss;
    ss << t;
    out_type result;
    ss >> result; 
    return result;
}

std::string GetCurrentDateTimeAsString(void);
std::string GetCurrentDateTimeAsString(char *format);

// Conversion d'un buffer en une string representant le contenu de ce buffer en hexa decimal
std::string HexaToString(const unsigned char* buff, unsigned long buffSize);

// Conversion d'un buffer en une string representant le contenu de ce buffer en hexa decimal
void HexaToString(const unsigned char* buff, unsigned long buffSize, char* sz);

// Conversion d'une string representant le contenu d'un buffer en ce buffer
// C'est l'operation inverse de HexaToString (mais avec des params d'entree differents
bool CnvStrToBuffer(const std::string& str, boost::unique_ptr<unsigned char[]>* pBuffer);

// Conversion d'une string representant un nombre decimal en ce nombre
bool CnvStrToULong(const std::string& str, unsigned long* pRes);

// Conversion d'une string representant un nombre hexa decimal en ce nombre
bool CnvStrToHex(const std::string& str, unsigned long* pRes, unsigned nbMaxDigits = 8);

// Conversion d'une string representant un nombre binaire en ce nombre
bool CnvStrToBin(const std::string& str, unsigned long* pRes, unsigned nbMaxDigits = 32);

// Conversion d'une string representant une version X.Y.Z en ces n° de versions
bool Str2Version(const std::string& s, unsigned char* pX, unsigned char* pY,unsigned char* pZ);

/// Convertit une chaine en minuscule
void ToLower(std::string* pStr);

/// Convertit une chaine en majuscule
void ToUpper(std::string* pStr);

/// Calcul du CRC 16 du buffer passe en parametre
/// x^16 + x^15 + x^2 + 1
unsigned short CRC16_Compute(unsigned char* pBuff, unsigned buffSize);

/// Calcul du CRC 16 CCITT du buffer passe en parametre
/// x^16 + x^12 + x^5 + 1
unsigned short CRC16_CCITT_Compute(unsigned char *pBuff, unsigned long buffSize);

/// Calcul du CRC 32 du buffer passe en parametre
/// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + 1
unsigned long CRC32_Compute(unsigned char* pBuff, unsigned buffSize);

// calcul incremental d'un CRC32
unsigned long CRC32_InitRemainder(void);
void CRC32_Add(unsigned char* pBuff, unsigned buffSize, unsigned long* pRemainder);
unsigned long CRC32_GetFinal(unsigned long remainder);

void StartTimeCount(void);

void EndTimeCount(char*  Text);

#endif // UTILS_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/utils.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///