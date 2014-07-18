////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TEST_MGR
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
///
/// Ce fichier implemente la classe du gestionnaire de test
///
/// \n \n <b>Copyright ADENEO</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// D'abord les include standard (Chaine de compilation / Standard Adeneo
//                                                                     / Librairie)

#include "stdafx.h"
#include "OngTV.h"
#include <strstream>
#include "version.h"
#include "utils.h"
#include "MainFrm.h"
#include "PathMgr.h"
#include "TestMgr.h"

////////////////////////////////////////////////////////////////////////////////
//                                                               Defines & Types
////////////////////////////////////////////////////////////////////////////////

#define LOG(param) \
{                                                                   \
    std::ostrstream strTrace((char *)m_szTrace, sizeof(m_szTrace)); \
    strTrace << param << std::endl << std::ends;                    \
    m_logFile.Write(m_szTrace, (unsigned int)strlen(m_szTrace));    \
}

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
/// Obtention de l'instance unique du gestionnaire de connexions
///
/// \return : pointeur sur le gestionnaire
////////////////////////////////////////////////////////////////////////////////
CTestMgr* CTestMgr::pSingleInstance = NULL;
CTestMgr* CTestMgr::ReadSingleInstance(void)
{
    if (pSingleInstance == NULL)
        pSingleInstance = new(CTestMgr);

    return pSingleInstance;
}
////////////////////////////////////////////////////////////////////////////////
/// Destruction du gestionnaire de connexions
////////////////////////////////////////////////////////////////////////////////
void CTestMgr::DeleteSingleInstance(void)
{
    if (pSingleInstance != NULL)
    {
        delete pSingleInstance;
        pSingleInstance = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Constructeur
////////////////////////////////////////////////////////////////////////////////
CTestMgr::CTestMgr() : m_cntScenarioErrors(0), m_cntTestCaseErrors(0), m_scenarioStartTimeInMs(0)
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CTestMgr::~CTestMgr()
{
    Garbage();
}

////////////////////////////////////////////////////////////////////////////////
/// Liberation des ressources de l'objet
////////////////////////////////////////////////////////////////////////////////
void CTestMgr::Garbage (void)
{
    // on ferme un eventuel scenario
    StopScenario();
}


bool CTestMgr::PrintError(const char* szError)
{
    // on envoie sur la console de trace
    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    pMainFrame->SendToTraceWindow((char*)szError, CTraceBar::LUA_TRACE);
    if (m_logFile.IsOpen())
    {
        // un fichier de log est ouvert => on ecrit dedans
        if (m_strTestCaseName.size() > 0)
        {
            // si on est dans un TC, on indente de 4 espaces
            m_logFile.Write("    ", 4U);
            // et on increment le compteur d'erreurs du test case
            ++m_cntTestCaseErrors;
        }
        // on ecrit le temps ecoulé depuis le debut du scenario
        DWORD elapsed = AdeGetTickCountMs() - m_scenarioStartTimeInMs;
        sprintf_s(m_szTrace, sizeof(m_szTrace), "%6ld ms : ", elapsed);
        m_logFile.Write(m_szTrace, strlen(m_szTrace));
        // on ecrit l'erreur
        // et on incremente le compteur d'erreurs global
        m_logFile.Write(szError, strlen(szError));
        m_logFile.Write("\n", 1U);
        ++m_cntScenarioErrors;
    }
    return true;
}


bool CTestMgr::Print(const char* sz)
{
    if (m_logFile.IsOpen())
    {
        // un fichier de log est ouvert => on ecrit dedans
        // mais on n'envoie pas dans la console LUA
        if (m_strTestCaseName.size() > 0)
        {
            // si on est dans un TC, on indente de 4 espaces
            m_logFile.Write("    ", 4U);
        }
        // on ecrit le temps ecoulé depuis le debut du scenario
        DWORD elapsed = AdeGetTickCountMs() - m_scenarioStartTimeInMs;
        sprintf_s(m_szTrace, sizeof(m_szTrace), "%6ld ms : ", elapsed);
        m_logFile.Write(m_szTrace, strlen(m_szTrace));
        // on ecrit la chaine
        m_logFile.Write(sz, strlen(sz));
        m_logFile.Write("\n", 1U);
    }
    else
    {
        // pas de fichier ouvert, on envoie dans la console LUA
        CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
        pMainFrame->SendToTraceWindow((char*)sz, CTraceBar::LUA_TRACE);
    }
    return true;
}


bool CTestMgr::StartScenario(const char* szScenarioName, const char* szPathDirLog /*= NULL*/, char* const szError /*= NULL*/, const unsigned long szErrorSize /*= 0*/)
{
    if (m_logFile.IsOpen())
    {
        // un fichier de log est deja ouvert => cela veut dire qu'il y a deja un scenario en cours
        if (NULL != szError)
        {
            sprintf_s(szError, szErrorSize, "un scenario est deja ouvert");
        }
        return false;
    }

    // On construit le nom du fichier de log pour le scenario
    std::string strLogDirName;
    if (NULL == szPathDirLog)
    {
        // path par defaut
        strLogDirName = ALIAS_CNXPATH + std::string("\\ScenLog");
    }
    else
    {
        strLogDirName = szPathDirLog;
    }
    // on subsitue l'alias
    CPathMgr::ReadSingleInstance()->SubstituteAliasWithPath(&strLogDirName);
    // On cree le rep s'il n'est pas deja cree
    CreateDirectory(strLogDirName.c_str(), 0);

    // On construit le nom du fichier de log
    std::string strLogFileName = strLogDirName + std::string("\\") + std::string(szScenarioName) + GetCurrentDateTimeAsString("_%Y%m%d_%H%M") + std::string(".log");

    // On ouvre le fichier en ecriture
    if  (m_logFile.Open(strLogFileName.c_str(), O_TEXT|O_WRONLY|O_CREAT|O_TRUNC, _SH_DENYWR, _S_IREAD | _S_IWRITE) == false)
    {
        if (NULL != szError)
        {
            sprintf_s(szError, szErrorSize, "erreur d'ouverture du fichier de log %s", strLogFileName.c_str());
        }
        return false;
    }

    // On ecrit l'entete dans le log
    // Date du test
    LOG(GetCurrentDateTimeAsString());

    // On ecrit le nom et la version de l'outil de test
    HMODULE hModule = GetModuleHandle(NULL);
    CVersion versionInfos(hModule);
    LOG(versionInfos.GetProductName() << " Version " << versionInfos.GetProductVersion());
    
    //// On ecrit le CRC du binaire teste
    //CProcessor* pProc = CProcessorMgr::ReadSingleInstance()->GetFirstProcessor();
    //if (pProc->GetCRC().size())
    //{
    //    LOG("CRC of the binary under test: " << pProc->GetCRC().c_str());
    //}
    //
    // On ecrit le nom du scenario
    LOG("SCENARIO BEGIN " << szScenarioName);
    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    pMainFrame->SendToTraceWindow((char*)m_szTrace, CTraceBar::LUA_TRACE);
    // on saute une ligne
    LOG("");
    
    // on stocke le nom du scenario
    m_strScenarioName = szScenarioName;
    
    // on reinitialise le compteur d'erreur
    m_cntScenarioErrors = 0U;
    
    m_scenarioStartTimeInMs = AdeGetTickCountMs();

    return true;
}
bool CTestMgr::StopScenario(char* const szError /*= NULL*/, const unsigned long szErrorSize /*= 0*/)
{
    if (!m_logFile.IsOpen())
    {
        // fichier de log non ouvert => pas de scenario en cours
        if (NULL != szError)
        {
            sprintf_s(szError, szErrorSize, "pas de scenario en cours");
        }
        return false;
    }
    // s'il y a un test case en cours, on le ferme
    StopTestCase();
    
    // temps du test
    DWORD elapsed = AdeGetTickCountMs() - m_scenarioStartTimeInMs;
    if (0 == m_cntScenarioErrors)
    {
        // le scenario s'est termine sans erreur
        LOG("END OF " << m_strScenarioName << " SCENARIO ("<< elapsed << "ms) => SUCCESSFUL");
    }
    else
    {
        // le scenario s'est termine avec des erreurs
        LOG("END OF " << m_strScenarioName << " SCENARIO ("<< elapsed << "ms) => FAILED " << m_cntScenarioErrors << " errors detected");
    }
    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    pMainFrame->SendToTraceWindow((char*)m_szTrace, CTraceBar::LUA_TRACE);
    LOG("");
    // On imprime les resultats pour les TCs
    LOG("RESULTS SUMMARY");
    LOG("");
    char szTmp[100];
    for (unsigned i = 0 ; i < m_arrTCResults.size() ; ++i)
    {
        if (0 == m_arrTCResults[i].cntTestCaseErrors)
        {
            sprintf_s(szTmp, sizeof(szTmp), "%-20s SUCCESSFUL\n", m_arrTCResults[i].strTestCaseName.c_str());
        }
        else
        {
            sprintf_s(szTmp, sizeof(szTmp), "%-20s FAILED (%d errors)\n", m_arrTCResults[i].strTestCaseName.c_str(), m_arrTCResults[i].cntTestCaseErrors);
        }
        m_logFile.Write(szTmp, (unsigned int)strlen(szTmp));
        pMainFrame->SendToTraceWindow((char*)szTmp, CTraceBar::LUA_TRACE);
    }

    // et on ferme le fichier de log
    m_logFile.Close();
    // et on clear les résultats et les dico des timers et des données d'alarmes
    m_arrTCResults.clear();
    m_MapTimers.clear();
    return true;
}

bool CTestMgr::StartTestCase(const char* szTestCaseName, char* const szError /*= NULL*/, const unsigned long szErrorSize /*= 0*/)
{
    if (!m_logFile.IsOpen())
    {
        // fichier de log non ouvert => pas de scenario en cours
        if (NULL != szError)
        {
            sprintf_s(szError, szErrorSize, "pas de scenario en cours");
        }
        return false;
    }
    // s'il y a un test case en cours, on le ferme
    StopTestCase();
    
    // on verifie que ce test case n'a pas deja ete utilise
    for (unsigned i = 0 ; i < m_arrTCResults.size() ; ++i)
    {
        if (m_arrTCResults[i].strTestCaseName == szTestCaseName)
        {
            // Test case duplique
            if (NULL != szError)
            {
                sprintf_s(szError, szErrorSize, "Le test case %s est dupliqué", szTestCaseName);
            }
            return false;
        }
    }

    // On ecrit le nom du test case
    LOG("TEST CASE BEGIN " << szTestCaseName);
    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    pMainFrame->SendToTraceWindow((char*)m_szTrace, CTraceBar::LUA_TRACE);
    // on saute une ligne
    LOG("");
    
    // On stocke le nom du test case
    m_strTestCaseName = szTestCaseName;

    // on reinitialise le compteur d'erreur
    m_cntTestCaseErrors = 0U;

    m_testCaseStartTimeInMs = AdeGetTickCountMs();

    return true;
}
bool CTestMgr::StopTestCase(char* const szError /*= NULL*/, const unsigned long szErrorSize /*= 0*/)
{
    if (0 == m_strTestCaseName.size())
    {
        // pas de test case en cours
        if (NULL != szError)
        {
            sprintf_s(szError, szErrorSize, "pas de test case en cours");
        }
        return false;
    }
    
    // on saute une ligne
    LOG("");
    // temps du test
    DWORD elapsed = AdeGetTickCountMs() - m_testCaseStartTimeInMs;

    if (0 == m_cntTestCaseErrors)
    {
        // le scenario s'est termine sans erreur
        LOG("END OF " << m_strTestCaseName << " TEST CASE ("<< elapsed << "ms) => SUCCESSFUL");
    }
    else
    {
        // le scenario s'est termine avec des erreurs
        LOG("END OF " << m_strTestCaseName << " TEST CASE => ("<< elapsed << "ms) FAILED " << m_cntTestCaseErrors << " errors detected");
    }
    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    pMainFrame->SendToTraceWindow((char*)m_szTrace, CTraceBar::LUA_TRACE);
    // on saute une ligne
    LOG("");
    // on stocke le resultat
    m_arrTCResults.push_back(TCResult(m_strTestCaseName, m_cntTestCaseErrors));
    // on indique qu'on n'est plus dans un test case
    m_strTestCaseName.clear();
    return true;
}

bool CTestMgr::TimerArm(const char* szTimerName, const DWORD durationInMs, char* const szError /*= NULL*/, const unsigned long szErrorSize /*= 0*/)
{
    // on efface un eventuel timer portant ce nom
    m_MapTimers.erase(std::string(szTimerName));
    // on insere dans la map en calculant la date d'expiration
    DWORD arminDate = AdeGetTickCountMs();
    DWORD expirationDate = arminDate + durationInMs;
    m_MapTimers.insert(std::string(szTimerName), new CTimer(arminDate, expirationDate));
    return true;
}
bool CTestMgr::TimerIsExpired(const char* szTimerName, bool* const pIsExpired, DWORD* const pElapsedSinceArmMs, char* const szError /*= NULL*/, const unsigned long szErrorSize /*= 0*/)
{
    // on recherche le timer
    T_NAME2TIMER::iterator iter = m_MapTimers.find(std::string(szTimerName));
    if (iter == m_MapTimers.end())
    {
        // pas trouve => erreur
        if (NULL != szError)
        {
            sprintf_s(szError, szErrorSize, "Le timer %s n'existe pas", szTimerName);
        }
        return false;
    }
    else
    {
        // on calcule le temps ecoule depuis le temps d'armement
        *pElapsedSinceArmMs = AdeGetTickCountMs() - iter->second->m_armingDateMs;
        if (iter->second->m_expirationDateMs <= AdeGetTickCountMs())
        {
            // le timer a expire
            *pIsExpired = true;
        }
        else
        {
            // le timer n'a pas expire
            *pIsExpired = false;
        }
        return true;
    }
}
bool CTestMgr::TimerWaitUntilExpired(const char* szTimerName, char* const szError /*= NULL*/, const unsigned long szErrorSize /*= 0*/)
{
    // on recherche le timer
    T_NAME2TIMER::iterator iter = m_MapTimers.find(std::string(szTimerName));
    if (iter == m_MapTimers.end())
    {
        // pas trouve => erreur
        if (NULL != szError)
        {
            sprintf_s(szError, szErrorSize, "Le timer %s n'existe pas", szTimerName);
        }
        return false;
    }
    else
    {
        // on attend jusqu'a la date d'expiration
        DWORD actualDate = AdeGetTickCountMs();
        DWORD expirationDateMs = iter->second->m_expirationDateMs;
        if (expirationDateMs > actualDate)
        {
            DWORD timeToWaitMs = expirationDateMs - actualDate;
            AdeWaitMs(timeToWaitMs);
        }
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/TestMgr.cpp $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
