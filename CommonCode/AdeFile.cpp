////////////////////////////////////////////////////////////////////////////////
/// @addtogroup UTIL
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe CAdeFile
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeFile.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                  / Librairie)
#include <windows.h>
#include <string>
#include <stdio.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <strstream>
#include <boost/unique_ptr.hpp>
#include <boost/smart_ptr.hpp>

// Puis les include projet
#include "AdeNonCopyable.h"
#include "AdeFile.h"
#include "Utils.h"
#include "AdeTrace.h"

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
/// methode static : obtention du nom d'un fichier temporaire
////////////////////////////////////////////////////////////////////////////////
std::string CAdeFile::GetTemporaryFileName(const std::string& strFilePrefix)
{
    char szTempPath[MAX_PATH];
    char szTempFileName[MAX_PATH];
    std::string tempFileName;
    if (GetTempPath(MAX_PATH, szTempPath) != 0)
    {
        // on a recuperer le path sur le repertoire tampon => on cree
        // un nom de fichier unique
        if (GetTempFileName( szTempPath            // directory for tmp files
                           , strFilePrefix.c_str() // temp file name prefix 
                           , 0                     // create unique name 
                           , szTempFileName        // buffer for name 
                           ) != 0)
        {
            tempFileName = szTempFileName;
        }
    }
    return tempFileName;
}
////////////////////////////////////////////////////////////////////////////////
/// methode static : calcul du CRC d'un fichier
////////////////////////////////////////////////////////////////////////////////
unsigned long CAdeFile::GetCRC32(const std::string& fileName)
{
    unsigned char buffer[2048];
    int size;
    unsigned long remainder = CRC32_InitRemainder();

    CAdeFile file;
    if (file.Open(fileName, O_BINARY|O_RDONLY, _SH_DENYWR, 0) == false)
    {
        return remainder;
    }
    // On calcule 
    do {
        size = file.Read(buffer, sizeof(buffer));
        if (size > 0)
        {
            CRC32_Add(buffer, size, &remainder);
        }
    } while (size > 0);
    file.Close();
    return CRC32_GetFinal(remainder);
}

////////////////////////////////////////////////////////////////////////////////
/// Constructeur
////////////////////////////////////////////////////////////////////////////////
CAdeFile::CAdeFile():  m_Fd(-1)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CAdeFile::~CAdeFile()
{
    // Fermeture du fichier
    Close();
}
////////////////////////////////////////////////////////////////////////////////
/// Ouverture du fichier
///
/// Parametres : 
/// \param filename    nom du fichier
/// \param oflag       cf parametre de _sopen_s
/// \param shflag      cf parametre de _sopen_s
/// \param pmode       cf parametre de _sopen_s
///
/// \return true si le fichier est ouvert, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeFile::Open(const std::string& fileName, int oflag, int shflag, int pmode)
{
    if (m_Fd != -1)
    {
        // un fichier est deja ouvert
        return false;
    }
        //ouverture du fichier
    if(_sopen_s( &m_Fd
               , fileName.c_str()
               , oflag
               , shflag
               , pmode
               ) != 0)
    {
        // fichier pas ouvert
        m_Fd = -1;
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Fermeture du fichier
///
////////////////////////////////////////////////////////////////////////////////
void CAdeFile::Close()
{
    // fermeture du fichier s'il est ouvert
    if (m_Fd != -1)
    {
        _close(m_Fd);
        m_Fd = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Lecture de tout le contenu du fichier (renvoie la taille de ce qui a ete lu ou un nombre
/// negatif si une erreur s'est produite)
///
/// Parametres : 
/// \param buffer     [out]buffer dans lequel a ete ecrit le contenu du fichier
///
/// \return taille du fichier ou -1 si probleme de lecture
////////////////////////////////////////////////////////////////////////////////
int CAdeFile::ReadContent(boost::unique_ptr<unsigned char[]>* pBuffer)
{
    if (m_Fd == -1)
    {
        // fichier non ouvert
        return -1;
    }
    long fileSize = Length();
    if (fileSize <= 0)
    {
        pBuffer->reset();
        return 0;
    }
    pBuffer->reset(new unsigned char[fileSize]);
    // on appelle la methode de base Read
    int bytesRead = CAdeFile::Read(pBuffer->get(), fileSize);
    if (bytesRead != fileSize)
    {
        // liberation du buffer
        pBuffer->reset();
        return -1;
    }
    return bytesRead;
}

// primitive de lecture a utiliser uniquement si le fichier est binaire
bool CAdeFile::Read8U(unsigned char* pVal)
{
    if (Read(pVal, sizeof(*pVal)) == sizeof(*pVal))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::Read16U(unsigned short* pVal)
{
    if (Read(pVal, sizeof(*pVal)) == sizeof(*pVal))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::Read32U(unsigned long* pVal)
{
    if (Read(pVal, sizeof(*pVal)) == sizeof(*pVal))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::Read32U(unsigned int* pVal)
{
    unsigned long lval;
    if (Read(&lval, sizeof(lval)) == sizeof(lval))
    {
        *pVal = lval;
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::ReadDouble(double* pVal)
{
    if (Read(pVal, sizeof(*pVal)) == sizeof(*pVal))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::ReadString(std::string* pStr)
{
    // Une chaine est stockee avec la taille (sur 2 octets) puis la chaine elle-meme
    unsigned short size;
    if (Read16U(&size) == false)
    {
        // Impossible de relire la taille
        return false;
    }
    if (size == 0)
    {
        // chaine vide
        pStr->clear();
        return true;
    }
    // On lit maintenant le contenu de la chaine
    boost::scoped_array<unsigned char> buffer;
    buffer.reset(new unsigned char[size+1]);    // +1 pour le zero terminal
    // on appelle la methode de base Read
    if (Read(buffer.get(), size) != size)
    {
        // erreur de lecture
        return false;
    }
    buffer[size] = 0;   // On force le 0 terminal
    *pStr = (char*)buffer.get();
    return true;
}
// Version ou le buffer est alloue dynamiquement selon la taille lue dans le fichier
bool CAdeFile::ReadBuffer(boost::unique_ptr<unsigned char[]>* pBuffer, unsigned* pSizeBuffer)
{
    unsigned sizeBuffer;
    // Un buffer est stocke avec la taille (sur 4 octets) puis le contenu du buffer
    if (Read32U(&sizeBuffer) == false)
    {
        // Impossible de relire la taille
        return false;
    }
    *pSizeBuffer = sizeBuffer;
    if (sizeBuffer == 0)
    {
        // buffer vide
        pBuffer->reset(0);
        return true;
    }
    // On lit maintenant le contenu du buffer
    pBuffer->reset(new unsigned char[sizeBuffer]);    // +1 pour le zero terminal
    // on appelle la methode de base Read
    if (Read(pBuffer->get(), sizeBuffer) != sizeBuffer)
    {
        // erreur de lecture
        return false;
    }
    return true;
}
// Version ou le buffer est alloue avant l'appel, on verifie que la longueur lue dans le
// fichier est identique a celle passee en parametre
bool CAdeFile::ReadBuffer(unsigned char* pBuffer, unsigned sizeBuffer)
{
    // Un buffer est stocke avec la taille (sur 4 octets) puis le contenu du buffer
    unsigned sizeBufferInFile;
    if (Read32U(&sizeBufferInFile) == false)
    {
        // Impossible de relire la taille
        return false;
    }
    if (sizeBuffer != sizeBufferInFile)
    {
        // discordance
        return false;
    }
    // on appelle la methode de base Read
    if (Read(pBuffer, sizeBuffer) != sizeBuffer)
    {
        // erreur de lecture
        return false;
    }
    return true;
}
// primitive d'ecriture a utiliser uniquement si le fichier est binaire
bool CAdeFile::Write8U(unsigned char val)
{
    if (Write(&val, sizeof(val)) == sizeof(val))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::Write16U(unsigned short val)
{
    if (Write(&val, sizeof(val)) == sizeof(val))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::Write32U(unsigned long val)
{
    if (Write(&val, sizeof(val)) == sizeof(val))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::WriteDouble(double val)
{
    if (Write(&val, sizeof(val)) == sizeof(val))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CAdeFile::WriteString(const std::string& str)
{
    if (str.size() > 0xFFFFu)
    {
        // Chaine trop grande
        return false;
    }
    // Une chaine est stockee avec la taille (sur 2 octets) puis la chaine elle-meme
    unsigned short size = str.size();
    if (Write16U(size) == false)
    {
        // Erreur d'ecriture
        return false;
    }
    if (size == 0)
    {
        // chaine vide
        return true;
    }
    if (Write(str.c_str(), size) == size)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CAdeFile::WriteBuffer(unsigned char* buffer, unsigned sizeBuffer)
{
    // Un buffer est stocke avec la taille (sur 4 octets) puis le contenu du buffer
    if (Write32U(sizeBuffer) == false)
    {
        // Erreur d'ecriture
        return false;
    }
    if (sizeBuffer == 0)
    {
        // buffer vide
        return true;
    }
    if (Write(buffer, sizeBuffer) == sizeBuffer)
    {
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Ouverture du fichier
///
/// Parametres : 
/// \param filename    nom du fichier
///
/// \return true si le fichier est ouvert, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeReadOnlyBinaryFileWithCRC::Open(const std::string& fileName)
{
    char szFunc[] = "CAdeReadOnlyBinaryFileWithCRC::Open";
    if (IsOpen())
    {
        // fichier deja ouvert
        return false;
    }
    if (CAdeReadOnlyFile::Open(fileName, O_BINARY) == false)
    {
        // erreur d'ouverture du fichier
        return false;
    }
    // on lit tout le contenu du fichier
    long fileSize = CAdeReadOnlyFile::ReadContent(&m_fileContent);
    unsigned long crcInFile;
    if (fileSize < sizeof(crcInFile))
    {
        // il faut au moins le CRC dans le fichier => pb
        Close();
        return false;
    }

    m_lengthWithoutCRC = fileSize - sizeof(crcInFile);
    // on recupere le crc dans le buffer
    crcInFile = *(unsigned long*)&m_fileContent[m_lengthWithoutCRC];
    // on verifier que le fichier n'a pas ete altere
    unsigned long crcFile = CRC32_Compute(m_fileContent.get(), m_lengthWithoutCRC);
    if (crcInFile != crcFile)
    {
        // Fichier corrompu
        ADE_WARNING("file : " << fileName << " Crc attendu " << crcInFile << " crc obtenu " << crcFile);
        Close();
        return false;
    }
    // On initialise la position pour le parcours sequentiel
    m_currPos = 0;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Lecture de tout le contenu du fichier (renvoie la taille de ce qui a ete lu ou un nombre
/// negatif si une erreur s'est produite)
///
/// Parametres : 
/// \param buffer     [out]buffer dans lequel a ete ecrit le contenu du fichier
///
/// \return taille du fichier ou -1 si probleme de lecture
////////////////////////////////////////////////////////////////////////////////
int CAdeReadOnlyBinaryFileWithCRC::ReadContent(boost::unique_ptr<unsigned char[]>* pBuffer)
{
    if (IsOpen() == false)
    {
        // fichier non ouvert
        return -1;
    }
    // le contenu du fichier a deja ete lu lors de l'ouverture => on retourne ce contenu
    pBuffer->reset(new unsigned char[m_lengthWithoutCRC]);
    memcpy(pBuffer->get(), m_fileContent.get(), m_lengthWithoutCRC);
    return m_lengthWithoutCRC;
}

////////////////////////////////////////////////////////////////////////////////
/// Lecture dans le fichier
///
/// Parametres : 
/// \param buffer     [out]buffer dans lequel a ete ecrit le contenu du fichier
/// \param count      [in]nombre max d'octets a lire
///
/// \return taille du fichier ou -1 si probleme de lecture
////////////////////////////////////////////////////////////////////////////////
int CAdeReadOnlyBinaryFileWithCRC::Read(void* buffer, unsigned int count)
{
    if (IsOpen() == false)
    {
        // fichier non ouvert
        return -1;
    }
    if (m_currPos >= m_lengthWithoutCRC)
    {
        // fin de fichier
        return 0;
    }
    // On limite eventuellement si ce qui reste a lire dans le fichier est inferieur a ce qui est demande
    if (count > static_cast<unsigned int>(m_lengthWithoutCRC - m_currPos))
    {
        count = m_lengthWithoutCRC - m_currPos;
    }
    // On copie les donnees
    memcpy(buffer, &m_fileContent[m_currPos], count);
    // On "avance" dans le fichier
    m_currPos += count;
    // On renvoie le nombre d'octets effectivement lus
    return count;
}

////////////////////////////////////////////////////////////////////////////////
/// Ouverture du fichier
///
/// Parametres : 
/// \param filename    nom du fichier
///
/// \return true si le fichier est ouvert, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeWriteOnlyBinaryFileWithCRC::Open(const std::string& fileName)
{
    if (IsOpen())
    {
        // fichier deja ouvert
        return false;
    }
    // initailisation du remainder pour le calcul du CRC
    m_remainder = CRC32_InitRemainder();
    return CAdeWriteOnlyFile::Open(fileName, O_BINARY);
}

////////////////////////////////////////////////////////////////////////////////
/// Fermeture du fichier => on ecrit le CRC et on ferme
///
////////////////////////////////////////////////////////////////////////////////
void CAdeWriteOnlyBinaryFileWithCRC::Close()
{
    if (IsOpen() == false)
    {
        // fichier pas ouvert
        return;
    }
    // On ecrit le CRC
    unsigned long crc = CRC32_GetFinal(m_remainder);
    // on appelle la fonction de la classe dont on derive (et surtout pas la surcharge
    // qui accumule le CRC)
    CAdeWriteOnlyFile::Write(&crc, sizeof(crc));
    // Et on ferme le fichier
    CAdeWriteOnlyFile::Close();
}

////////////////////////////////////////////////////////////////////////////////
/// Ecriture dans le fichier binaire avec CRC
///
/// Parametres : 
/// \param buffer     buffer contenant les donnees a ecrire
/// \param count      Taille des donnes a ecrire
///
/// \return taille des donnees ecrites
////////////////////////////////////////////////////////////////////////////////
int CAdeWriteOnlyBinaryFileWithCRC::Write(const void* buffer, unsigned int count)
{
    // On accumule d'abord les donnees pour le calcul du CRC
    CRC32_Add((unsigned char*)buffer, count, &m_remainder);
    // et on ecrit dans le fichier
    return CAdeWriteOnlyFile::Write(buffer, count);
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeFile.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
