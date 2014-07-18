////////////////////////////////////////////////////////////////////////////////
/// @addtogroup TEST_MGR
/// Definition du manager de test
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definition de la classe CTestMgr qui defini le singleton du manager
/// de test
///
///  \n \n <b>Copyright ADENEO</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/TestMgr.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////


// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef TEST_MGR_H
#define TEST_MGR_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////

#include "AdeFile.h"

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////


class CTestMgr
{
NON_COPYABLE(CTestMgr);
private :
    CTestMgr();
    virtual ~CTestMgr();
    static CTestMgr* pSingleInstance;
    // Liberation des ressources de l'objet
    void Garbage(void);

public:
    // obtention du pointeur sur la seule instance du manager
    static CTestMgr* ReadSingleInstance(void);

    // destruction de la seule instance
    static void DeleteSingleInstance(void);
    
    bool StartScenario(const char* szScenarioName, const char* szPathDirLog = NULL, char* const szError = NULL, const unsigned long szErrorSize = 0);
    bool StopScenario(char* const szError = NULL, const unsigned long szErrorSize = 0);
    bool StartTestCase(const char* szTestCaseName, char* const szError = NULL, const unsigned long szErrorSize = 0);
    bool StopTestCase(char* const szError = NULL, const unsigned long szErrorSize = 0);
    bool PrintError(const char* szError);
    bool Print(const char* sz);
    bool TimerArm(const char* szTimerName, const DWORD durationInMs, char* const szError = NULL, const unsigned long szErrorSize = 0);
    bool TimerIsExpired(const char* szTimerName, bool* const pIsExpired, DWORD* const pElapsedSinceArmMs, char* const szError = NULL, const unsigned long szErrorSize = 0);
    bool TimerWaitUntilExpired(const char* szTimerName, char* const szError = NULL, const unsigned long szErrorSize = 0);

private:

    CAdeFile     m_logFile;
    std::string  m_strScenarioName;
    std::string  m_strTestCaseName;
    unsigned int m_cntScenarioErrors;
    unsigned int m_cntTestCaseErrors;
    DWORD        m_scenarioStartTimeInMs;
    DWORD        m_testCaseStartTimeInMs;
    
    struct TCResult {
	    std::string  strTestCaseName;
        unsigned int cntTestCaseErrors;
        TCResult(const std::string& name, const unsigned int cnt) : strTestCaseName(name), cntTestCaseErrors(cnt) {};
    };
    std::vector<TCResult> m_arrTCResults;
    
    class CTimer {
        public:
        DWORD m_armingDateMs;
        DWORD m_expirationDateMs;
        CTimer(const DWORD armingMs, const DWORD expirationMs) : m_armingDateMs(armingMs), m_expirationDateMs(expirationMs) {};
    };
    typedef boost::ptr_map<std::string, CTimer> T_NAME2TIMER;
    T_NAME2TIMER m_MapTimers;

    // pour le log
    char m_szTrace[2000];                                            \
};

#endif // TEST_MGR_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/TestMgr.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///
