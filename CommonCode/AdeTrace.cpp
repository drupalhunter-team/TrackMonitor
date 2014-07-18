////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TRACE
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implémente la classe du gestionnaire de trace (CAdeTrace)
///
/// \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeTrace.cpp $
/// @endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                     / Librairie)
#include <windows.h>

#include <time.h>
#include <stdio.h>
#include <string>
#include <strstream>

// Puis les include projet
#include "AdeNonCopyable.h"
#include "AdeTrace.h"


////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////
//typedef struct {
//    <type>   m1;        // commentaire
//    <type>  m2;         // commentaire
//} MON_TYPE_INTERNE;
#define ADE_TRACE_HEADER_FORMAT "\
*******************************************************************************\r\n\
**  ADENEO Standard Library - Trace Component\r\n\
**  version %d.%d\r\n\
**  (built on %s - %s)\r\n\
*******************************************************************************\r\n\
**  Copyright ADENEO 2005\r\n\
*******************************************************************************\r\n\
" // pas de CRLF en fin : ajouté automatiquement dans la config par défaut
#define ADE_TRACE_HEADER_FORMAT_PRODUCT "\
*******************************************************************************\r\n\
**  ADENEO Standard Library - Trace Component\r\n\
**  Product %s Version %s\r\n\
*******************************************************************************\r\n\
**  Copyright %s\r\n\
*******************************************************************************\r\n\
" // pas de CRLF en fin : ajouté automatiquement dans la config par défaut

// Configuration en registry
#define ADE_TRACE_MASK_ENTRY          "TraceMask"             
#define ADE_TRACE_NBFILES_ENTRY       "NbFiles"             
#define ADE_TRACE_CONSOLE_ENTRY       "ConsoleMask"             
#define ADE_TRACE_LOGPATH_ENTRY       "LogPath"             
#define ADE_TRACE_PURGEDELAY_ENTRY    "PurgeDelay"             

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

CAdeTrace* CAdeTrace::pSingleInstance = NULL;

////////////////////////////////////////////////////////////////////////////////
/// Obtention de l'instance unique du gestionnaire de traces
/// La fonction InitSingleInstance doit avoir ete appellee au prealable
///
/// \return : pointeur sur le gestionnaire
////////////////////////////////////////////////////////////////////////////////
CAdeTrace* CAdeTrace::ReadSingleInstance(void)
{
    return pSingleInstance;
}
////////////////////////////////////////////////////////////////////////////////
/// Obtention de l'instance unique du gestionnaire de traces
///
/// Parametres :
/// \param szModuleName le nom a utiliser pour nommer les fichiers et les clefs
///                     dans la base de registres
/// \param szProductName      Nom du logiciel qui utilise le composant (mis dans le header)
/// \param szProductVersion   Version de ce logiciel (mis dans le header)
/// \param szProductCopyright Copyright de ce logiciel (mis dans le header)
/// \param szCompanyName      Nom de la societe
/// \param cbTrace      callback a appeler lors de l'enregistrement d'une trace
///                     (peut etre NULL)
/// \param lpParam      donnee utilisateur a passer a cette callback
/// \param ulNbLinesInConsole nombre de lignes a afficher dans la console de trace
///
/// \return : pointeur sur le gestionnaire
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::InitSingleInstance( const char*  szModuleName
                                  , const char*  szProductName
                                  , const char*  szProductVersion
                                  , const char*  szProductCopyright
                                  , const char*  szCompanyName
                                  , T_EXTERNAL_CB_TRACE externalCbTrace /*= NULL*/
                                  , T_CB_TRACE cbTrace /*= NULL*/
                                  , LPVOID lpParam /*= NULL*/
                                  , unsigned long ulNbLinesInConsole /*= ADE_TRACE_NBLINESINCONSOLE*/
                                  , unsigned long ulTraceMask /* = ADE_TRACE_MASK_DEFAULT */
                                  , unsigned long ulFilesInConsole /* = ADE_TRACE_CONSOLE_DEFAULT */)
{
    if (pSingleInstance == NULL)
        pSingleInstance = new CAdeTrace( szModuleName
                                       , szProductName
                                       , szProductVersion
                                       , szProductCopyright
                                       , szCompanyName
                                       , externalCbTrace
                                       , cbTrace
                                       , lpParam
                                       , ulNbLinesInConsole
                                       , ulTraceMask
                                       , ulFilesInConsole);
}
////////////////////////////////////////////////////////////////////////////////
/// Destruction du gestionnaire de traces
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::DeleteSingleInstance(void)
{
    if (pSingleInstance != NULL)
    {
        delete pSingleInstance;
        pSingleInstance = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Constructeur prive
///
/// Parametres :
/// \param szModuleName le nom a utiliser pour nommer les fichiers et les clefs
///                     dans la base de registes
/// \param szProductName      Nom du logiciel qui utilise le composant (mis dans le header)
/// \param szProductVersion   Version de ce logiciel (mis dans le header)
/// \param szProductCopyright Copyright de ce logiciel (mis dans le header)
/// \param szCompanyName      Nom de la societe
/// \param externalCbTrace    callback a appeler lors de l'enregistrement d'une trace
///                     (peut etre NULL)
/// \param cbTrace      callback a appeler lors de l'enregistrement d'une trace
///                     (peut etre NULL)
/// \param lpParam      donnee utilisateur a passer a cette callback
/// \param ulNbLinesInConsole nombre de lignes a afficher dans la console de trace
/////////////////////////////////////////////////////////////////////////////////
CAdeTrace::CAdeTrace( const char*  szModuleName
                    , const char*  szProductName
                    , const char*  szProductVersion
                    , const char*  szProductCopyright
                    , const char*  szCompanyName
                    , T_EXTERNAL_CB_TRACE externalCbTrace
                    , T_CB_TRACE cbTrace
                    , LPVOID lpParam
                    , unsigned long ulNbLinesInConsole
                    , unsigned long ulTraceMask
                    , unsigned long ulFilesInConsole) :
    
    m_ulTraceMask(ulTraceMask),
    m_ulNbFiles(ADE_TRACE_NBFILES_DEFAULT),
    m_ulFilesInConsole(ulFilesInConsole),
    m_ulPurgeDelay(ADE_TRACE_PURGEDELAY_DEFAULT),
    m_bRecordDateTime(true),
    m_DateFormat(DATEFORMAT_DATETIME),
    m_bRecordThreadId(false),
    m_bInsertCRLF(true),
    m_LastError(TRACE_NO_ERROR),
    m_externalCbTrace(externalCbTrace),
    m_cbTrace(cbTrace),
    m_lpParam(lpParam)
{
    if (m_externalCbTrace)
    {
        // Les traces vont etre prises en charge par un outil externe => on ne fait rien
        return;
    }
    if (strlen(szModuleName) > ADE_TRACE_MAX_MODULENAME_SIZE)
    {
        strncpy_s(m_szModuleName, sizeof(m_szModuleName), szModuleName, ADE_TRACE_MAX_MODULENAME_SIZE);
        m_szModuleName[ADE_TRACE_MAX_MODULENAME_SIZE] = '\0';
    }
    else
    {
        if (strcmp(szModuleName, "") == 0)
        {
            strncpy_s(m_szModuleName, sizeof(m_szModuleName), "AdeTrace", _TRUNCATE);
        }
        else
        {
            strncpy_s(m_szModuleName, sizeof(m_szModuleName), szModuleName, _TRUNCATE);
        }
    }

    strncpy_s(m_szLogPath, sizeof(m_szLogPath), ADE_TRACE_LOGPATH_DEFAULT, _TRUNCATE);

    int i;
    for (i = 0; i < ADE_TRACE_MAXFILES; ++i)
    {
        m_ahFiles[i] = INVALID_HANDLE_VALUE;
        m_ahFilesMutex[i] = CreateMutex(NULL, false, NULL);
    }
    m_hConsole = INVALID_HANDLE_VALUE;
    m_hConsoleMutex = CreateMutex(NULL, false, NULL);

    // lecture de la config en registry
    InitFromRegistry(szModuleName, szCompanyName);

    // purge des anciennes traces
    PurgeOldFiles();

    // ouverture de console si des lignes dans la console
	if (ulNbLinesInConsole != 0)
    {
        OpenConsole(ulNbLinesInConsole); //pas besion de console
    }
    OpenTraceFiles();

    // tracé du header
    {
        char szDest[ADE_SERIAL_MAXTRACE_LENGTH];
        if (szProductName)
        {
            _snprintf_s(szDest, sizeof(szDest), _TRUNCATE, ADE_TRACE_HEADER_FORMAT_PRODUCT, szProductName, szProductVersion, szProductCopyright);
        }
        else
        {
            _snprintf_s(szDest, sizeof(szDest), _TRUNCATE, ADE_TRACE_HEADER_FORMAT, ADE_TRACE_MAJOR_VERSION, ADE_TRACE_MINOR_VERSION, __DATE__, __TIME__);
        }
        WriteToAllFiles(szDest);
        if (m_hConsole != INVALID_HANDLE_VALUE)
        {
            // console active
            WriteToConsole(CONSOLE_COLOR_WHITE, szDest);
        }
    }

    // les fichiers ne sont pas maintenus ouverts pour permettre des accès externes
    for (i = 0; i < ADE_TRACE_MAXFILES; ++i)
    {
        if (m_ahFiles[i] != INVALID_HANDLE_VALUE)
        {
            CloseTraceFile(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur prive
////////////////////////////////////////////////////////////////////////////////
CAdeTrace::~CAdeTrace()
{
    if (m_externalCbTrace)
    {
        // Les traces ont etre prises en charge par un outil externe => on ne fait rien
        return;
    }
    CloseConsole();
    CloseHandle(m_hConsoleMutex);

    for (int i = 0; i < ADE_TRACE_MAXFILES; ++i)
    {
        if (m_ahFiles[i] != INVALID_HANDLE_VALUE)
        {
            CloseTraceFile(i);
        }
        CloseHandle(m_ahFilesMutex[i]);
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Fonction de trace. Le premier prototype permet l'utilisation d'une couleur par défaut (blanc)
/// La fonction effectue la trace si le niveau demandé est activé dans le masque.
/// La trace se fait dans le(s)fichier(s) indiqué(s).
/// Si le fichier est tracé en console, elle se fait aussi dans la  console dans la couleur indiquée.
///
/// Parametres :
/// \param ulFileMask   masque des fichiers de trace destination
/// \param ulTraceLevel niveau de trace
/// \param Color        couleur de la trace dans la console
/// \param szFormat     chaine de formatage de la trace 
/// \param ...          paramètre optionnels de la chaine de formatage
///
/// \return false si
///                  Erreur systeme (LastError = TRACE_ERROR_SYSTEM)
///                  Fichier invalide (LastError = TRACE_ERROR_INVALIDFILE)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeTrace::Send(unsigned long ulFileMask, unsigned long ulTraceLevel, const char*  szTrace)
{
    return Send(ulFileMask, ulTraceLevel, CONSOLE_COLOR_WHITE, szTrace);
}

bool CAdeTrace::Send(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szTrace)
{
    if (m_externalCbTrace)
    {
        // Les traces sont etre prises en charge par un outil externe => on lui transmet la trace
        m_externalCbTrace(m_lpParam, ulTraceLevel, (char*)szTrace);
        return true;
    }
    bool bResult = true;
    if (SendWithoutOutputToConsole(ulFileMask, ulTraceLevel, Color, szTrace))
    {
        if ((ulFileMask  & m_ulFilesInConsole) != 0)
        {
            // au moins un fichier dans lequel on trace est sur la console
            if (m_cbTrace)
            {
                // appel de la callback de trace si elle existe
                m_cbTrace(m_lpParam, ulTraceLevel, (char*)szTrace, Color);
            }
            if (m_hConsole != INVALID_HANDLE_VALUE)
            {
                // console ouverte, on envoie dessus
                bResult = WriteToConsole(Color, szTrace);
            }
        }
    }
    return bResult;
}

bool CAdeTrace::FormatAndSend(unsigned long ulFileMask, unsigned long ulTraceLevel, const char*  szFormat, ...)
{
    char szDest[ADE_SERIAL_MAXTRACE_LENGTH];
    va_list argList;
    va_start(argList, szFormat);
    _vsnprintf_s(szDest, sizeof(szDest), _TRUNCATE, szFormat, argList);
    va_end(argList);
    return Send(ulFileMask, ulTraceLevel, CONSOLE_COLOR_WHITE, szDest);
}

bool CAdeTrace::FormatAndSend(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szFormat, ...)
{
    char szDest[ADE_SERIAL_MAXTRACE_LENGTH];
    va_list argList;
    va_start(argList, szFormat);
    _vsnprintf_s(szDest, sizeof(szDest), _TRUNCATE, szFormat, argList);
    va_end(argList);
    return Send(ulFileMask, ulTraceLevel, Color, szDest);
}
bool CAdeTrace::FormatAndSendWithoutOutputToConsole(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szFormat, ...)
{
    char szDest[ADE_SERIAL_MAXTRACE_LENGTH];
    va_list argList;
    va_start(argList, szFormat);
    _vsnprintf_s(szDest, sizeof(szDest), _TRUNCATE, szFormat, argList);
    va_end(argList);
    if (m_externalCbTrace)
    {
        // Les traces sont etre prises en charge par un outil externe => on lui transmet la trace
        m_externalCbTrace(m_lpParam, ulTraceLevel, szDest);
        return true;
    }
    return SendWithoutOutputToConsole(ulFileMask, ulTraceLevel, Color, szDest);
}

bool CAdeTrace::SendWithoutOutputToConsole(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szTrace)
{
    bool bResult = true;

    if ((0x00000001 << ulTraceLevel) & m_ulTraceMask)
    {
        char szDest[ADE_SERIAL_MAXTRACE_LENGTH];

        if (bResult)
        {
            // trace à effectuer
            char szDate[26 + 3]; // taille en dur : liée au SDK : 
                                 // fonction asctime renvoie une chaine de 26 octets \0 compris. +3 pour ' : '
            char szTID[2 + 8 + 3 + 1]; // 2 pour '0x'. +8 pour taille d'un DWORD en hexa. +3 pour ' - '. +1 pour \0
            char szCRLF[1 + 1 + 1]; // 1 pour '\n'. +1 pour \0. +1 pour 0

            // ajout éventuel de la date
            if (m_bRecordDateTime)
            {
                switch (m_DateFormat)
                {
                    case DATEFORMAT_DATETIME:
                    {
                        time_t tNow;
                        time(&tNow);
                        struct tm lt;
                        localtime_s(&lt, &tNow);
                        asctime_s(szDate, sizeof(szDate), &lt);
                        // suppression de l' \n final
                        szDate[strlen(szDate)-1] = '\0';
                        strncat_s(szDate, sizeof(szDate), " : ", _TRUNCATE);
                    }
                    break;
                    case DATEFORMAT_TICK:
                    {
                        sprintf_s(szDate, sizeof(szDate), "%ld : ", GetTickCount());
                    }
                    break;
                }
            }
            else
            {
                // pas de date : szDate vaut ""
                szDate[0] = '\0';
            }

            // ajout éventuel du TID
            if (m_bRecordThreadId)
            {
                sprintf_s(szTID, sizeof(szTID), "0x%08X - ", GetCurrentThreadId());
            }
            else
            {
                // pas de TID : szTID vaut ""
                szTID[0] = '\0';
            }

            // ajout éventuel du CRLF
            if (m_bInsertCRLF)
            {
                szCRLF[0] = '\r';
                szCRLF[1] = '\n';
                szCRLF[2] = '\0';
                ////sprintf(szCRLF, "\r\n");
            }
            else
            {
                // pas de CRLF : szCRLF vaut ""
                szCRLF[0] = '\0';
            }

            // formatage final !!!
            _snprintf_s(szDest, sizeof(szDest), _TRUNCATE, "%s%s%s%s", szDate, szTID, szTrace, szCRLF);
        }

        if (bResult)
        {
            for (int i = 0; i < ADE_TRACE_MAXFILES && bResult; ++i)
            {
                if (((ulFileMask >> i) & 0x00000001) == 0x00000001)
                {
                    // les fichiers ne sont pas maintenus ouverts pour permettre des accès externes
                    WaitForSingleObject(m_ahFilesMutex[i], INFINITE);

                    OpenTraceFile(i);
                    bResult = WriteToFile(i, szDest);
                    CloseTraceFile(i);
                    ReleaseMutex(m_ahFilesMutex[i]);
                }
            }
        }

    }
    return bResult;

}

////////////////////////////////////////////////////////////////////////////////
/// paramètre la présence de la date au début de chaque trace
///
/// Parametres :
/// \param bRecordDateTime flag indiquant si la date est ajoutée
/// \param DateFormat      format de la date : date/heure ou Ticks
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::DoRecordDateTime(bool bRecordDateTime, T_DATEFORMAT DateFormat)
{
    m_bRecordDateTime = bRecordDateTime;
    m_DateFormat = DateFormat;
}

////////////////////////////////////////////////////////////////////////////////
/// paramètre la présence du TID au début de chaque trace
///
/// Parametres :
/// \param bRecordThreadId : flag indiquant si le TID est ajouté
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::DoRecordThreadId(bool bRecordThreadId)
{
    m_bRecordThreadId = bRecordThreadId;
}

////////////////////////////////////////////////////////////////////////////////
/// paramètre la présence d'un CRLF en fin de trace
//
/// Parametres :
/// \param bInsertCRLF : flag indiquant si le CRLF est ajouté
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::InsertAutomaticCRLF(bool bInsertCRLF)
{
    m_bInsertCRLF = bInsertCRLF;
}


////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions internes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// initialise les paramètres depuis la registry s'ils n'existent pas les crée 
/// avec les valeurs par défaut !!!
///
/// \return true si l'initialisation s'est bien deroulee, false sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeTrace::InitFromRegistry(const char*  szModuleName, const char*  szCompanyName)
{
    char szTraceKey[MAX_PATH+1];
    HKEY hKey;
    bool bResult = true;
    DWORD dwSize;
    DWORD dwData;
    DWORD dwType;
    char  szData[MAX_PATH+1];

    // on a change l'emplacement de la clef, on detruit une eventuelle clef a l'ancien emplacement pour ne
    // pas induire l'utilisateur en erreur
    _snprintf_s(szTraceKey, sizeof(szTraceKey), _TRUNCATE, "SOFTWARE\\Adeneo\\StandardLibrary\\%s\\Trace", m_szModuleName);
    if (::RegOpenKey(   HKEY_LOCAL_MACHINE, 
                        szTraceKey, 
                        &hKey) 
        == ERROR_SUCCESS)
    {
        // clef existante : on la detruit
        ::RegDeleteKey(HKEY_LOCAL_MACHINE, szTraceKey);
    }



    _snprintf_s(szTraceKey, sizeof(szTraceKey), _TRUNCATE, "Software\\%s\\%s\\Trace", szCompanyName, szModuleName);

    if (::RegOpenKey(   HKEY_CURRENT_USER, 
                        szTraceKey, 
                        &hKey) 
        != ERROR_SUCCESS)
    {
        // clé inexistante : on la crée
        bResult = (::RegCreateKey(  HKEY_CURRENT_USER, 
                                    szTraceKey, 
                                    &hKey) 
                   == ERROR_SUCCESS);
    }

    // Trace Mask
    if (bResult)
    {
        dwSize = sizeof(DWORD);
        dwType = REG_DWORD;
        if (::RegQueryValueEx(  hKey, 
                                ADE_TRACE_MASK_ENTRY, 
                                NULL, 
                                &dwType, 
                                (BYTE*)&dwData, 
                                &dwSize) 
            != ERROR_SUCCESS)
        {
            bResult = (::RegSetValueEx( hKey, 
                                        ADE_TRACE_MASK_ENTRY, 
                                        NULL, 
                                        REG_DWORD, 
                                        (CONST BYTE*)&m_ulTraceMask, 
                                        dwSize) 
                       == ERROR_SUCCESS);
        }
        else
        {
            m_ulTraceMask = dwData;
        }
    }

    // Nb files
    if (bResult)
    {
        dwSize = sizeof(DWORD);
        dwType = REG_DWORD;
        if (::RegQueryValueEx(  hKey, 
                                ADE_TRACE_NBFILES_ENTRY, 
                                NULL, 
                                &dwType, 
                                (BYTE*)&dwData, 
                                &dwSize) 
            != ERROR_SUCCESS)
        {
            bResult = (::RegSetValueEx( hKey, 
                                        ADE_TRACE_NBFILES_ENTRY, 
                                        NULL, 
                                        REG_DWORD, 
                                        (CONST BYTE*)&m_ulNbFiles, 
                                        dwSize) 
                       == ERROR_SUCCESS);
        }
        else
        {
            m_ulNbFiles = dwData;
        }
    }

    // Console Mask
    if (bResult)
    {
        dwSize = sizeof(DWORD);
        dwType = REG_DWORD;
        if (::RegQueryValueEx(  hKey, 
                                ADE_TRACE_CONSOLE_ENTRY, 
                                NULL, 
                                &dwType, 
                                (BYTE*)&dwData, 
                                &dwSize) 
            != ERROR_SUCCESS)
        {
            bResult = (::RegSetValueEx( hKey, 
                                        ADE_TRACE_CONSOLE_ENTRY, 
                                        NULL, 
                                        REG_DWORD, 
                                        (CONST BYTE*)&m_ulFilesInConsole, 
                                        dwSize) 
                       == ERROR_SUCCESS);
        }
        else
        {
            m_ulFilesInConsole = dwData;
        }
    }

    // Purge Delay
    if (bResult)
    {
        dwSize = sizeof(DWORD);
        dwType = REG_DWORD;
        if (::RegQueryValueEx(  hKey, 
                                ADE_TRACE_PURGEDELAY_ENTRY, 
                                NULL, 
                                &dwType, 
                                (BYTE*)&dwData, 
                                &dwSize) 
            != ERROR_SUCCESS)
        {
            bResult = (::RegSetValueEx( hKey, 
                                        ADE_TRACE_PURGEDELAY_ENTRY, 
                                        NULL, 
                                        REG_DWORD, 
                                        (CONST BYTE*)&m_ulPurgeDelay, 
                                        dwSize) 
                       == ERROR_SUCCESS);
        }
        else
        {
            m_ulPurgeDelay = dwData;
        }
    }

    // Log path
    if (bResult)
    {
        dwSize = MAX_PATH;
        dwType = REG_EXPAND_SZ;
        if (::RegQueryValueEx(  hKey, 
                                ADE_TRACE_LOGPATH_ENTRY, 
                                NULL, 
                                &dwType, 
                                (BYTE*)szData, 
                                &dwSize) 
            != ERROR_SUCCESS)
        {
            bResult = (::RegSetValueEx( hKey, 
                                        ADE_TRACE_LOGPATH_ENTRY, 
                                        NULL, 
                                        REG_EXPAND_SZ, 
                                        (CONST BYTE*)m_szLogPath, 
                                        dwSize) 
                       == ERROR_SUCCESS);
        }
        else
        {
            strncpy_s(m_szLogPath, sizeof(m_szLogPath), szData, _TRUNCATE);
        }
    }

    ::RegCloseKey(hKey);

    return bResult;

}

////////////////////////////////////////////////////////////////////////////////
/// supprime les fichiers trop vieux. Effectue le renommage
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::PurgeOldFiles()
{
    DWORD dwFirstFileIndex = 0;
    DWORD dwLastFileIndex = 0;
    int iNumFile = 0;
    char szFileName[MAX_PATH];
    char szNewFileName[MAX_PATH];
    HANDLE hFile;
    WIN32_FIND_DATA FileData;
    //FILETIME ftPurgeLimit;
    //SYSTEMTIME stPurgeLimit;
    
    // conversion d'un chemin relatif en chemin absolu
    if ((m_szLogPath[0]=='\0') || ((strstr(":\\",m_szLogPath) == NULL) && (m_szLogPath[0]!='\\')))
    {
        char szDest[MAX_PATH+1];
        char szCurDir[MAX_PATH+1];
        ::GetCurrentDirectory(MAX_PATH, szCurDir);
        _snprintf_s(szDest, sizeof(szDest), _TRUNCATE, "%s\\%s",  szCurDir, m_szLogPath);
        strncpy_s(m_szLogPath, sizeof(m_szLogPath), szDest, _TRUNCATE);
    }
    // ajout éventuel de l'\'
    if (m_szLogPath[strlen(m_szLogPath)-1] != '\\')
    {
        strncat_s(m_szLogPath, sizeof(m_szLogPath), "\\", _TRUNCATE);
    }

    // création éventuelle du répertoire
    CreateDirectory(m_szLogPath, NULL);

    
    /*purge des fichiers inutile
    // calcul de la date limite de purge
    GetLocalTime(&stPurgeLimit);
    stPurgeLimit.wDay -= (WORD)m_ulPurgeDelay;
    SystemTimeToFileTime(&stPurgeLimit, &ftPurgeLimit);

    // purge des fichiers.
    // on considère que les fichiers sont nommés chronologiquement
    sprintf(szFileName, "%s%s0.%d.log", m_szLogPath, m_szModuleName, dwFirstFileIndex);
    for (hFile = FindFirstFile(szFileName, &FileData); hFile != INVALID_HANDLE_VALUE;  hFile = FindFirstFile(szFileName, &FileData))
    {
        if ((hFile != INVALID_HANDLE_VALUE) && (CompareFileTime(&FileData.ftLastWriteTime, &ftPurgeLimit) == -1))
        {
            // on supprime tous les fichiers de même nom
            iNumFile = 0;
            do 
            {
                sprintf(szFileName, "%s%s%d.%d.log", m_szLogPath, m_szModuleName, iNumFile, dwFirstFileIndex);
                ++iNumFile;
            } while(DeleteFile(szFileName) != 0);
            // on incrémente le numéro du fichier
            ++dwFirstFileIndex;
            sprintf(szFileName, "%s%s0.%d.log", m_szLogPath, m_szModuleName, dwFirstFileIndex);
        }
        else
        {
            // on a atteint la limite de purge ou plus de fichiers : on sort de la boucle
            break;
        }
    }
*/
    // renommage des fichiers.restants
    // on considère que les fichiers sont nommés chronologiquement
    dwLastFileIndex = dwFirstFileIndex;
    _snprintf_s(szFileName, sizeof(szFileName), _TRUNCATE, "%s%s0.%d.log", m_szLogPath, m_szModuleName, dwLastFileIndex);
    for (hFile = FindFirstFile(szFileName, &FileData); hFile != INVALID_HANDLE_VALUE;  hFile = FindFirstFile(szFileName, &FileData))
    {
        if ((hFile != INVALID_HANDLE_VALUE) )
        {
            // on renomme tous les fichiers de même nom
            iNumFile = 0;
            do 
            {
                _snprintf_s(szFileName, sizeof(szFileName), _TRUNCATE, "%s%s%d.%d.log", m_szLogPath, m_szModuleName, iNumFile, dwLastFileIndex);
                _snprintf_s(szNewFileName, sizeof(szNewFileName), _TRUNCATE, "%s%s%d.%d.log", m_szLogPath, m_szModuleName, iNumFile, dwLastFileIndex-dwFirstFileIndex);
                ++iNumFile;
            } while(MoveFile(szFileName, szNewFileName) != 0);
            // on incrémente le numéro du fichier
            ++dwLastFileIndex;
            _snprintf_s(szFileName, sizeof(szFileName), _TRUNCATE, "%s%s0.%d.log", m_szLogPath, m_szModuleName, dwLastFileIndex);
        }
    }
    FindClose(hFile);

    // renommage des derniers fichiers de trace
    iNumFile = 0;
    do 
    {
        _snprintf_s(szFileName, sizeof(szFileName), _TRUNCATE, "%s%s%d.log", m_szLogPath, m_szModuleName, iNumFile);
        _snprintf_s(szNewFileName, sizeof(szNewFileName), _TRUNCATE, "%s%s%d.%d.log", m_szLogPath, m_szModuleName, iNumFile, dwLastFileIndex-dwFirstFileIndex);
        ++iNumFile;
    } while(MoveFile(szFileName, szNewFileName) != 0);

}

////////////////////////////////////////////////////////////////////////////////
/// si utilisée, ouvre la console et la configure
///
/// Parametres :
/// \param ulNbLinesInConsole  taille de la console
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::OpenConsole(unsigned long ulNbLinesInConsole)
{
    if (m_ulFilesInConsole != 0)
    {
        // on utilise la console
        if (AllocConsole())
        {
            COORD dwSize;
            dwSize.X = 80;
            dwSize.Y = (short)ulNbLinesInConsole;

            m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            SetConsoleTitle(m_szModuleName);
            SetConsoleScreenBufferSize(m_hConsole, dwSize);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
/// ouvre les fichiers de trace utilisés avec création
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::OpenTraceFiles()
{
    char szFileName[MAX_PATH+1];

    // création des fichiers
    for (int i = 0; i < (int)m_ulNbFiles; ++i)
    {
        _snprintf_s(szFileName, sizeof(szFileName), _TRUNCATE, "%s%s%d.log", m_szLogPath, m_szModuleName, i);
        m_ahFiles[i] = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// ouvre un fichier de trace 
///
/// Parametres :
/// \param ulNumFile  numéro de fichier
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::OpenTraceFile(unsigned long ulNumFile)
{
    char szFileName[MAX_PATH+1];

    _snprintf_s(szFileName, sizeof(szFileName), _TRUNCATE, "%s%s%d.log", m_szLogPath, m_szModuleName, ulNumFile);
    m_ahFiles[ulNumFile] = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    SetFilePointer(m_ahFiles[ulNumFile], 0, NULL, FILE_END);

}

////////////////////////////////////////////////////////////////////////////////
/// ecrit dans la console si file masque correspond à ConsoleMask
/// l'écriture est "Multithread-safe"
///
/// Parametres :
/// \param Color  couleur d'écriture
/// \param szMsg  message écrit
///
/// \return false si
///                  Erreur système (TRACE_ERROR_SYSTEM)
///                  Console non active (TRACE_ERROR_NOCONSOLE)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeTrace::WriteToConsole(T_CONSOLE_COLOR Color, const char*  szMsg)
{
    bool bResult = true;
    DWORD dwWritten;
    
    WaitForSingleObject(m_hConsoleMutex, INFINITE);

    if (m_hConsole == INVALID_HANDLE_VALUE)
    {
        bResult = false;
        m_LastError = TRACE_ERROR_NOCONSOLE;
    }

    if (bResult)
    {
        if (SetConsoleTextAttribute(m_hConsole, (WORD)Color) == 0)
        {
            bResult = false;
            m_LastError = TRACE_ERROR_SYSTEM;
        }
    }
    
    if (bResult)
    {
        if (WriteConsole(m_hConsole, (LPVOID)szMsg, (DWORD)strlen(szMsg), &dwWritten, NULL) == 0)
        {
            bResult = false;
            m_LastError = TRACE_ERROR_SYSTEM;
        }
        if(m_bInsertCRLF)
        {
            if (WriteConsole(m_hConsole, "\r\n", 2, &dwWritten, NULL) == 0)
            {
                bResult = false;
                m_LastError = TRACE_ERROR_SYSTEM;
            }
        }
    }

    ReleaseMutex(m_hConsoleMutex);
    return bResult;
}


////////////////////////////////////////////////////////////////////////////////
/// écrit dans le fichier s'il est actif
/// l'écriture est "Multithread-safe"
///
/// Parametres :
/// \param ulFile : numéro de fichier où tracer
/// \param szMsg : message écrit
///
/// \return false si
///                  Erreur système (TRACE_ERROR_SYSTEM)
///                  Fichier non actif (TRACE_ERROR_INVALIDFILE)
///         true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeTrace::WriteToFile(unsigned long ulFile, const char*  szMsg)
{
    bool bResult = true;
    DWORD dwWritten;


    if (m_ahFiles[ulFile] == INVALID_HANDLE_VALUE)
    {
        bResult = false;
        m_LastError = TRACE_ERROR_INVALIDFILE;
    }

    if (bResult)
    {
        if (WriteFile(m_ahFiles[ulFile], (LPVOID)szMsg, (DWORD)strlen(szMsg), &dwWritten, NULL) == 0)
        {
            bResult = false;
            m_LastError = TRACE_ERROR_SYSTEM;
        }
    }

   return bResult;
}


////////////////////////////////////////////////////////////////////////////////
/// écrit dans tous les fichiers actifs
/// l'écriture est "Multithread-safe"
//
/// Parametres :
/// \param szMsg : message écrit
///
/// \return false si Erreur système (TRACE_ERROR_SYSTEM), true sinon
////////////////////////////////////////////////////////////////////////////////
bool CAdeTrace::WriteToAllFiles(const char*  szMsg)
{
    bool bResult = true;

    for (int i = 0; i < (int)m_ulNbFiles && bResult; ++i)
    {
        WaitForSingleObject(m_ahFilesMutex[i], INFINITE);
        bResult = WriteToFile(i, szMsg);
        ReleaseMutex(m_ahFilesMutex[i]);
    }
    return bResult;
}

////////////////////////////////////////////////////////////////////////////////
/// ferme le fichier
///
/// Parametres :
/// \param ulFile fichier a fermer
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::CloseTraceFile(unsigned long ulFile)
{
    // attente de fin d'écriture
    WaitForSingleObject(m_ahFilesMutex[ulFile], INFINITE);
    if (m_ahFiles[ulFile] != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_ahFiles[ulFile]);
        m_ahFiles[ulFile] = INVALID_HANDLE_VALUE;
    }
    ReleaseMutex(m_ahFilesMutex[ulFile]);
}

////////////////////////////////////////////////////////////////////////////////
/// ferme la console
////////////////////////////////////////////////////////////////////////////////
void CAdeTrace::CloseConsole()
{
    // attente de fin d'écriture
    WaitForSingleObject(m_hConsoleMutex, INFINITE);
    if (m_hConsole != INVALID_HANDLE_VALUE)
    {
        FreeConsole();
        m_hConsole = INVALID_HANDLE_VALUE;
    }
    ReleaseMutex(m_hConsoleMutex);
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeTrace.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
