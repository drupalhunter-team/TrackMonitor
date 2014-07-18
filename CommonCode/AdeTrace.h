////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TRACE
/// Gestionnaire de traces
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CAdeTrace qui defini le singleton du gestionnaire
/// de traces
/// Les fonctionnalites de trace sont majoritairement utilisees au moyen de macro
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeTrace.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef ADE_TRACE_H
#define ADE_TRACE_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////
//#define PREMOD_NOMDEF1      <valeur>        // commentaire
//typedef struct {
//    <type>   m1;         // commentaire
//    <type>  *m2;         // commentaire
//} MON_TYPE_EXPORTE;

// Version du composant de trace
#define ADE_TRACE_MAJOR_VERSION           1
#define ADE_TRACE_MINOR_VERSION           0

#define ADE_TRACE_MASK_DEFAULT        0xFFFFFFFF
#define ADE_TRACE_NBFILES_DEFAULT     1            
#define ADE_TRACE_CONSOLE_DEFAULT     0x00000001             
#define ADE_TRACE_LOGPATH_DEFAULT     "Log"            
#define ADE_TRACE_PURGEDELAY_DEFAULT  1

// 
// Utilisation standard du composant pour quelques cas typiques
// pour l'utiliser correctement , il faut
// - respecter les niveaux définis ci-apres dans le masque enregistré en registry
// - stocker le nom de la fonction appelante dans une variable szFunc 
//   ( Attention à l'erreur de compil' masquée !!!)

// Sinon, on peut tout a fait utiliser directement des appels à la fonction Send

#define ADE_TRACE_LEVEL_ERROR             0
#define ADE_TRACE_LEVEL_WARNING           1
#define ADE_TRACE_LEVEL_INFO              2
#define ADE_TRACE_LEVEL_DEBUG             3

#define ADE_TRACE_COLOR_ERROR             CAdeTrace::CONSOLE_COLOR_RED
#define ADE_TRACE_COLOR_WARNING           CAdeTrace::CONSOLE_COLOR_YELLOW
#define ADE_TRACE_COLOR_INFO              CAdeTrace::CONSOLE_COLOR_WHITE
#define ADE_TRACE_COLOR_DEBUG             CAdeTrace::CONSOLE_COLOR_GREY

#ifndef _MSC_VER
#define ADE_ERROR(param) \
{                                                                   \
    char szTrace [2000];                                             \
    std::ostrstream strTrace((char*)szTrace, sizeof(szTrace));          \
    strTrace << param << std::ends;                                      \
    CAdeTrace::ReadSingleInstance()->Send(1, ADE_TRACE_LEVEL_ERROR, ADE_TRACE_COLOR_ERROR, \
        "%s => %s", __FUNC__, szTrace);                         \
}

#define ADE_WARNING(param) \
{                                                                   \
    char szTrace [2000];                                             \
    std::ostrstream strTrace((char*)szTrace, sizeof(szTrace));          \
    strTrace << param << std::ends;                                      \
    CAdeTrace::ReadSingleInstance()->Send(1, ADE_TRACE_LEVEL_WARNING, ADE_TRACE_COLOR_WARNING, \
        "%s => %s", __FUNC__, szTrace);                         \
}

#define ADE_INFO(ulFile, param) \
{                                                                   \
    char szTrace [2000];                                             \
    std::ostrstream strTrace((char*)szTrace, sizeof(szTrace));          \
    strTrace << param << std::ends;                                      \
    CAdeTrace::ReadSingleInstance()->Send(ulFile, ADE_TRACE_LEVEL_INFO, ADE_TRACE_COLOR_INFO, \
        "%s => %s", __FUNC__, szTrace);                         \
}
#else
#define ADE_ERROR(param) \
{                                                                   \
    char szTrace [ADE_SERIAL_MAXTRACE_LENGTH];                      \
    std::ostrstream strTrace((char*)szTrace, sizeof(szTrace));          \
    strTrace << szFunc << " => " << param << std::ends;                                      \
    CAdeTrace::ReadSingleInstance()->Send(1, ADE_TRACE_LEVEL_ERROR, ADE_TRACE_COLOR_ERROR, \
        szTrace);                         \
}

#define ADE_WARNING(param) \
{                                                                   \
    char szTrace [ADE_SERIAL_MAXTRACE_LENGTH];                      \
    std::ostrstream strTrace((char*)szTrace, sizeof(szTrace));          \
    strTrace << szFunc << " => " << param << std::ends;                                      \
    CAdeTrace::ReadSingleInstance()->Send(1, ADE_TRACE_LEVEL_WARNING, ADE_TRACE_COLOR_WARNING, \
        szTrace);                         \
}

#define ADE_INFO(ulFile, param) \
{                                                                   \
    char szTrace [ADE_SERIAL_MAXTRACE_LENGTH];                      \
    std::ostrstream strTrace((char*)szTrace, sizeof(szTrace));          \
    strTrace << szFunc << " => " << param << std::ends;                                      \
    CAdeTrace::ReadSingleInstance()->Send(ulFile, ADE_TRACE_LEVEL_INFO, ADE_TRACE_COLOR_INFO, \
        szTrace);                         \
}

#define ADE_DEBUG(ulFile, param) \
{                                                                   \
    char szTrace [ADE_SERIAL_MAXTRACE_LENGTH];                      \
    std::ostrstream strTrace((char *)szTrace, sizeof(szTrace));          \
    strTrace << szFunc << " => " << param << std::ends;                                      \
    CAdeTrace::ReadSingleInstance()->Send(ulFile, ADE_TRACE_LEVEL_DEBUG, ADE_TRACE_COLOR_DEBUG, \
        szTrace);                         \
}
#endif

#if 0
#define ADE_ERROR(param) \
    CAdeTrace::ReadSingleInstance()->Send(1, ADE_TRACE_LEVEL_ERROR, ADE_TRACE_COLOR_ERROR, \
        "%s : ligne %d => param %d", __FILE__, __LINE__, param)

#define ADE_ERROR_STR(param_str) \
    CAdeTrace::ReadSingleInstance()->Send(1, ADE_TRACE_LEVEL_ERROR, ADE_TRACE_COLOR_ERROR, \
        "%s : ligne %d => param %s", __FILE__, __LINE__, param_str)

#define ADE_WARNING(param) \
    CAdeTrace::ReadSingleInstance()->Send(1, ADE_TRACE_LEVEL_WARNING, ADE_TRACE_COLOR_WARNING, \
        "%s : ligne %d => param %d", __FILE__, __LINE__, param)
#endif

#if 0

#define ADE_ERROR(param)

#define ADE_ERROR_STR(param_str)

#define ADE_ERROR_STRP(param_str, param)

#define ADE_WARNING(param)

#define ADE_WARNING_STR(param_str)

#define ADE_WARNING_STRP(param_str, param)

#define ADE_INFO(ulFile, param)

#define ADE_INFO_STR(ulFile, param_str)

#define ADE_INFO_STRP(ulFile, param_str, param)
#endif

#define AdeTrace_Error(ulFile, szErrorMsg) \
        Send(ulFile, ADE_TRACE_LEVEL_ERROR, ADE_TRACE_COLOR_ERROR, \
        "!!! %s - ERROR !!! %s", szFunc, szErrorMsg)

#define AdeTrace_Warning(ulFile, szWarnMsg) \
        Send(ulFile, ADE_TRACE_LEVEL_WARNING, ADE_TRACE_COLOR_WARNING, \
        "??? %s - WARNING ??? %s", szFunc, szWarnMsg)

#define AdeTrace_Entering(ulFile) \
        Send(ulFile, ADE_TRACE_LEVEL_INFO, ADE_TRACE_COLOR_INFO, \
        ">>> %s - Entering >>>", szFunc)

#define AdeTrace_Exiting(ulFile) \
        Send(ulFile, ADE_TRACE_LEVEL_INFO, ADE_TRACE_COLOR_INFO, \
        "<<< %s - Exiting <<<", szFunc)

#define AdeTrace_Info(ulFile, szMsg) \
        Send(ulFile, ADE_TRACE_LEVEL_INFO, ADE_TRACE_COLOR_INFO, \
        "--- %s - %s", szFunc, szMsg)

#define AdeTrace(ulFile, ulTraceLevel, Color, szMsg) \
        Send(ulFile, ulTraceLevel, Color, \
        "--- %s - %s", szFunc, szMsg)


// Constantes du composant de trace
#define ADE_TRACE_MAXFILES                32      // Nombre max de fichier
#define ADE_TRACE_MAX_MODULENAME_SIZE     32      // taille du nom du module
#define ADE_SERIAL_MAXTRACE_LENGTH        2048    // taille max d'une trace
#define ADE_TRACE_NBLINESINCONSOLE        4096    // nombre de ligne par défaut de la console

////////////////////////////////////////////////////////////////////////////////
//                                                           Variables exportees
////////////////////////////////////////////////////////////////////////////////
// Note : PREMOD est le prefixe du module en majuscule et court

////////////////////////////////////////////////////////////////////////////////
//                                                           Fonctions exportees
////////////////////////////////////////////////////////////////////////////////

class CAdeTrace
{
NON_COPYABLE(CAdeTrace);
public:
    typedef enum {
        CONSOLE_COLOR_BLACK =           0x00,
        CONSOLE_COLOR_DARKBLUE =        FOREGROUND_BLUE,
        CONSOLE_COLOR_DARKGREEN =       FOREGROUND_GREEN,
        CONSOLE_COLOR_DARKCYAN =        FOREGROUND_BLUE         | FOREGROUND_GREEN,
        CONSOLE_COLOR_DARKRED =         FOREGROUND_RED,
        CONSOLE_COLOR_DARKPURPLE =      FOREGROUND_BLUE         | FOREGROUND_RED,
        CONSOLE_COLOR_DARKYELLOW =      FOREGROUND_RED          | FOREGROUND_GREEN,
        CONSOLE_COLOR_GREY =            FOREGROUND_BLUE         | FOREGROUND_RED    | FOREGROUND_GREEN,
        CONSOLE_COLOR_DARKGREY =        FOREGROUND_INTENSITY,
        CONSOLE_COLOR_BLUE =            FOREGROUND_INTENSITY    | FOREGROUND_BLUE,
        CONSOLE_COLOR_GREEN =           FOREGROUND_INTENSITY    | FOREGROUND_GREEN,
        CONSOLE_COLOR_CYAN =            FOREGROUND_INTENSITY    | FOREGROUND_BLUE   | FOREGROUND_GREEN,
        CONSOLE_COLOR_RED =             FOREGROUND_INTENSITY    | FOREGROUND_RED,
        CONSOLE_COLOR_PURPLE =          FOREGROUND_INTENSITY    | FOREGROUND_BLUE   | FOREGROUND_RED,
        CONSOLE_COLOR_YELLOW =          FOREGROUND_INTENSITY    | FOREGROUND_RED    | FOREGROUND_GREEN,
        CONSOLE_COLOR_WHITE =           FOREGROUND_INTENSITY    | FOREGROUND_BLUE   | FOREGROUND_RED    | FOREGROUND_GREEN,
    } T_CONSOLE_COLOR;

    typedef enum {
        DATEFORMAT_DATETIME,
        DATEFORMAT_TICK,
    } T_DATEFORMAT;

    typedef enum {
        TRACE_NO_ERROR,
        TRACE_ERROR_SYSTEM,
        TRACE_ERROR_INVALIDFILE,
        TRACE_ERROR_NOCONSOLE,
    } T_ERROR;

    typedef void (*T_CB_TRACE)(LPVOID lpParam, unsigned long ulTraceLevel, char* szTrace, T_CONSOLE_COLOR Color);
    typedef void (*T_EXTERNAL_CB_TRACE)(LPVOID lpParam, unsigned long ulTraceLevel, char* szTrace);
    // obtention du pointeur sur la seule instance du manager
    static CAdeTrace* ReadSingleInstance(void);
    static void InitSingleInstance( const char*  szModuleName, const char*  szProductName, const char*  szProductVersion, const char*  szProductCopyright, const char*  szCompanyName
                                  , T_EXTERNAL_CB_TRACE externalCbTrace = NULL, T_CB_TRACE cbTrace = NULL, LPVOID lpParam = NULL, unsigned long ulNbLinesInConsole = ADE_TRACE_NBLINESINCONSOLE
                                  , unsigned long ulTraceMask = ADE_TRACE_MASK_DEFAULT, unsigned long ulFilesInConsole = ADE_TRACE_CONSOLE_DEFAULT);
    // destruction de la seule instance
    static void DeleteSingleInstance(void);

    void SetCallback(T_CB_TRACE cbTrace = NULL, LPVOID lpParam = NULL) {m_cbTrace = cbTrace; m_lpParam = lpParam;};
    bool Send(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szTrace);
    bool Send(unsigned long ulFileMask, unsigned long ulTraceLevel, const char*  szTrace);
    bool SendWithoutOutputToConsole(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szTrace);
    bool FormatAndSend(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szFormat, ...);
    bool FormatAndSend(unsigned long ulFileMask, unsigned long ulTraceLevel, const char*  szFormat, ...);
    bool FormatAndSendWithoutOutputToConsole(unsigned long ulFileMask, unsigned long ulTraceLevel, T_CONSOLE_COLOR Color, const char*  szFormat, ...);

    void DoRecordDateTime(bool bRecordDateTime, T_DATEFORMAT DateFormat);
    void DoRecordThreadId(bool bRecordThreadId);
    void InsertAutomaticCRLF(bool bInsertCRLF);

    T_ERROR GetLastError() {return m_LastError;};

private:
    static CAdeTrace* pSingleInstance;
    CAdeTrace( const char*  szModuleName
             , const char*  szProductName
             , const char*  szProductVersion
             , const char*  szProductCopyright
             , const char*  szCompanyName
             , T_EXTERNAL_CB_TRACE externalCbTrace
             , T_CB_TRACE cbTrace
             , LPVOID lpParam
             , unsigned long ulNbLinesInConsole
             , unsigned long ulTraceMask
             , unsigned long ulFilesInConsole);

    virtual ~CAdeTrace();
    
    bool InitFromRegistry(const char*  szModuleName, const char*  szCompanyName);
    void PurgeOldFiles();

    void OpenConsole(unsigned long ulNbLinesInConsole);
    void OpenTraceFiles();
    void OpenTraceFile(unsigned long ulNumFile);
    
    bool WriteToConsole(T_CONSOLE_COLOR Color, const char*  szMsg);
    bool WriteToFile(unsigned long ulFile, const char*  szMsg);
    bool WriteToAllFiles(const char*  szMsg);

    void CloseTraceFile(unsigned long ulFile);
    void CloseConsole();

    char m_szModuleName[ADE_TRACE_MAX_MODULENAME_SIZE+1];

    char m_szLogPath[MAX_PATH];
    unsigned long m_ulTraceMask;
    unsigned long m_ulNbFiles;
    unsigned long m_ulFilesInConsole;
    unsigned long m_ulPurgeDelay;

    bool m_bRecordDateTime;
    T_DATEFORMAT m_DateFormat;
    bool m_bRecordThreadId;
    bool m_bInsertCRLF;

    HANDLE m_ahFiles[ADE_TRACE_MAXFILES];
    HANDLE m_hConsole;

    HANDLE m_ahFilesMutex[ADE_TRACE_MAXFILES];
    HANDLE m_hConsoleMutex;

    T_ERROR m_LastError;

    // callback pour affichage de la trace (avec log dans le fichier)
    T_CB_TRACE          m_cbTrace;
    // callback pour redirection de la trace vers un prog externe qui se chargera
    //de tout => on ne traite rien (pas de log console, ni de log fichier)
    T_EXTERNAL_CB_TRACE m_externalCbTrace;
    LPVOID     m_lpParam;    // param passe a la callback
protected:

};


#endif // ADE_TRACE_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/CommonCode/AdeTrace.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
