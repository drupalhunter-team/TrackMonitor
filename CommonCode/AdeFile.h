////////////////////////////////////////////////////////////////////////////////
/// @addtogroup UTIL
/// Gestion des fichiers
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CAdeFile qui defini un objet encapsulant les fonctions
/// d'ouverture et de fermeture d'un fichier (essentiellement pour ne plus avoir
/// a gerer la fermeture)
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeFile.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef ADE_FILE_H
#define ADE_FILE_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

#define RX_BUFFER_SIZE 16384

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////CByteReader
////////////////////////////////////////////////////////////////////////////////

// classe d'encapsulation des fichiers (pour ne pas avoir a gerer la fermeture)
// le fichier est automatiquement ferme dans le destructeur
// Acces sequentiel uniquement (pas de seek)
class CAdeFile
{
NON_COPYABLE(CAdeFile);
public:
    // Constructeur(s) //////////////////////
    CAdeFile(void);

    // Destructeur(s) //////////////////////
    virtual ~CAdeFile(void);

    static bool DeleteFile(const std::string& fileName) {return (_unlink(fileName.c_str()) != -1) ? true : false;};
    static unsigned long GetCRC32(const std::string& fileName);

    // Ouverture (meme parametre que _sopen_s excepte le file descriptor)
    bool Open(const std::string& fileName, int oflag, int shflag, int pmode);

    // acces au file descripteur
    int Fd(void) {return m_Fd;};

    // le fichier est-il correctement ouvert
    bool IsOpen(void) {if (m_Fd != -1) return true; return false;};

    // Lecture dans le fichier
    virtual int Read(void* buffer, unsigned int count) {return _read(m_Fd, buffer, count);};

    // Lecture de tout le contenu du fichier (renvoie la taille de ce qui a ete lu ou un nombre
    // negatif si une erreur s'est produite)
    virtual int ReadContent(boost::unique_ptr<unsigned char[]>* pBuffer);

    // Ecriture dans le fichier
    virtual int Write(const void* buffer, unsigned int count) {return _write(m_Fd, buffer, count);};

    // Longueur du fichier
    long Length(void) {return _filelength(m_Fd);};

    // forcer la fermeture
    virtual void Close(void);

    // primitive de lecture a utiliser uniquement si le fichier est binaire
    bool Read8U(unsigned char* pVal);
    bool Read16U(unsigned short* pVal);
    bool Read32U(unsigned long* pVal);
    bool Read32U(unsigned int* pVal);
    bool ReadDouble(double* pVal);
    // Une chaine est stockee dans un fichier sous la forme
    // taille de la chaine sur 2 octets puis contenu de la chaine
    bool ReadString(std::string* pStr);
    // Un buffer est stocke dans un fichier sous la forme
    // taille du buffer sur 4 octets puis contenu du buffer
    bool ReadBuffer(boost::unique_ptr<unsigned char[]>* pBuffer, unsigned* pSizeBuffer);
    bool ReadBuffer(unsigned char* pBuffer, unsigned sizeBuffer);
    // primitive d'ecriture a utiliser uniquement si le fichier est binaire
    bool Write8U(unsigned char val);
    bool Write16U(unsigned short val);
    bool Write32U(unsigned long val);
    bool WriteDouble(double val);
    // Une chaine est stockee dans un fichier sous la forme
    // taille de la chaine sur 2 octets puis contenu de la chaine
    bool WriteString(const std::string& str);
    // Un buffer est stocke dans un fichier sous la forme
    // taille du buffer sur 4 octets puis contenu du buffer
    bool WriteBuffer(unsigned char* buffer, unsigned sizeBuffer);

    // Obtention d'un nom de fichier temporaire
    static std::string GetTemporaryFileName(const std::string& strFilePrefix);

private:
    int  m_Fd;
};

// Specialisation pour gerer des fichiers ReadOnly ou WriteOnly (avec effacement si fichier existant)
class CAdeReadOnlyFile : public CAdeFile
{
NON_COPYABLE(CAdeReadOnlyFile);
public:
    // Constructeur par defaut
    CAdeReadOnlyFile(void) {};

    // Ouverture
    bool Open(const std::string& fileName, int oflag) {return CAdeFile::Open(fileName, oflag|O_RDONLY, _SH_DENYWR, 0);};

    // Ecriture dans un fichier impossible sur un fichier en lecture seule
    virtual int Write(const void* buffer, unsigned int count) {return -1;};

private:
};

class CAdeWriteOnlyFile : public CAdeFile
{
NON_COPYABLE(CAdeWriteOnlyFile);
public:
    // Constructeur par defaut
    CAdeWriteOnlyFile(void) {};

    // Ouverture
    bool Open(const std::string& fileName, int oflag) {return CAdeFile::Open(fileName, oflag|O_WRONLY|O_CREAT|O_TRUNC, _SH_DENYWR, _S_IREAD | _S_IWRITE);};

    // Lecture dans un fichier impossible sur un fichier en ecriture seule
    virtual int Read(void* buffer, unsigned int count) {return -1;};
    virtual int ReadContent(boost::unique_ptr<unsigned char[]>* pBuffer) {return -1;};

private:
};

// Specialisation pour gerer des fichiers binaires dont le contenu est protege par un CRC
// Le CRC se trouve en fin de fichier (4 octets)
class CAdeReadOnlyBinaryFileWithCRC : public CAdeReadOnlyFile
{
NON_COPYABLE(CAdeReadOnlyBinaryFileWithCRC);
public:
    // Constructeur par defaut
    CAdeReadOnlyBinaryFileWithCRC(void) {};

    // Ouverture du fichier avec check du CRC
    bool Open(const std::string& fileName);

    // Lecture dans le fichier (en fait dans le buffer interne, le fichier a ete lu en 1 fois au debut
    // pour verifier le CRC)
    virtual int Read(void* buffer, unsigned int count);

    // Lecture de tout le contenu du fichier (renvoie la taille de ce qui a ete lu ou un nombre
    // negatif si une erreur s'est produite)
    virtual int ReadContent(boost::unique_ptr<unsigned char[]>* pBuffer);

private:
    boost::unique_ptr<unsigned char[]> m_fileContent;      // buffer contenant le fichier (y compris le CRC)
    long                        m_lengthWithoutCRC; // taille de ce buffer sans le CRC
    long                        m_currPos;          // index sur l'emplacement courant (pour la lecture
                                                    // sequentielle : pas d'acces au fichier mais dans le buffer
                                                    // du fichier)
};

class CAdeWriteOnlyBinaryFileWithCRC : public CAdeWriteOnlyFile
{
NON_COPYABLE(CAdeWriteOnlyBinaryFileWithCRC);
public:
    // Constructeur par defaut
    CAdeWriteOnlyBinaryFileWithCRC(void) {};

    // Ouverture
    bool Open(const std::string& fileName);

    // Ecriture dans le fichier
    virtual int Write(const void* buffer, unsigned int count);

    // fermeture
    virtual void Close(void);

private:
    unsigned long m_remainder;  // pour le calcul incremental du CRC en sortie
};


#endif // ADE_FILE_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeFile.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
