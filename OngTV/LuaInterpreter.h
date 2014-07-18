#pragma once

#include "AdeCriticalSection.h"

extern "C" 
{
    #include "lua.h" 
    #include "lstate.h" 
    #include "lauxlib.h" 
    #include "lualib.h"
}

class CAdeThreadAsMember;

class CLuaInterpreter
{
NON_COPYABLE(CLuaInterpreter);
public:
    typedef enum {EVT_END} T_INTERPRETER_EVENT;
    typedef void (*T_NOTIFICATIONCALLBACK)(LPVOID lpParam, T_INTERPRETER_EVENT evt);

    CLuaInterpreter();
    virtual ~CLuaInterpreter();

    // renvoie true si un script est en cours d'execution
    bool IsRunning(void) {return m_bIsRunning;};
    // renvoie true si l'interpreteur est arrete (mode pas à pas, breakpoint, ...)
    bool IsPaused(void);

    // Demarre l'interpreteur
    bool StartFromBuffer(char* szCode, char* szCodeName, T_NOTIFICATIONCALLBACK pNotificationCallback = NULL, LPVOID lpParam = NULL);
    bool StartFromFile(const std::string& fileName, T_NOTIFICATIONCALLBACK pNotificationCallback = NULL, LPVOID lpParam = NULL);

    // Arrete l'interpreteur
    void Stop(void) {m_bStop = true;};

    // attente de l'arret de l'execution
    void WaitTerminate(DWORD timeoutMs = INFINITE);

    bool StepInto(void) {};
    bool StepOver(void) {};
    bool StepOut(void) {};
    bool ToggleBreakPoint(unsigned lineNumber) {};

private:
    // fonction statique passe au thread de managemeent
    static UINT StaticProcThreadExec(LPVOID lpParam, HANDLE hEventStop);
    // corps du thread de managemeent
    UINT ProcThreadExec(HANDLE hEventStop);

    static void hook (lua_State* L, lua_Debug* ar);
    static void lineHook (lua_State* L, lua_Debug* ar);
    static void funcHook (lua_State* L, lua_Debug* ar);

    lua_State*  m_LuaState;
    std::string m_Code;
    std::string m_CodeName;
    std::string m_FileName;
    T_NOTIFICATIONCALLBACK   m_pNotificationCallback; // Callback appelee pour signaler les evenement de l'interpreteur
    LPVOID                   m_lpParam;               // parametre utilisateur
    bool        m_bIsRunning;
    bool        m_bStop;    // arret demande
    boost::unique_ptr<CAdeThreadAsMember> m_pThreadExec; // Thread d'execution

};

class CLuaInterpreterMgr
{
NON_COPYABLE(CLuaInterpreterMgr);
private :
    CLuaInterpreterMgr();
    virtual ~CLuaInterpreterMgr();
    static CLuaInterpreterMgr* pSingleInstance;

public:
    // obtention du pointeur sur la seule instance du manager
    static CLuaInterpreterMgr* ReadSingleInstance(void);

    // destruction de la seule instance
    static void DeleteSingleInstance(void);

    void NotifyStartOfExec(CLuaInterpreter* pLuaInterpreter);
    void NotifyEndOfExec(CLuaInterpreter* pLuaInterpreter);

private:
    // section critique utilisée pour protéger l'accès au dictionnaire
    CAdeCriticalSection m_criticalSection;
    // Ensemble des interpréteurs qui sont en train d'exécuter un script
    std::set<CLuaInterpreter*> m_setLuaInterpretersInExec;

};
