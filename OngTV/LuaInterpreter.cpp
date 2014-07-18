#include "stdafx.h"
#include "OngTV.h"
#include "MainFrm.h"
#include "LuaInterpreter.h"
#include "AdeThread.h"
#include "LuaDllMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" void ITFLUA_open (lua_State* tolua_S);
extern "C" void ITFLUA_close(void);

CLuaInterpreter::CLuaInterpreter()
{
    m_bIsRunning = false;
}

CLuaInterpreter::~CLuaInterpreter()
{
}


bool CLuaInterpreter::StartFromBuffer(char* szCode, char* szCodeName, T_NOTIFICATIONCALLBACK pNotificationCallback /*= NULL*/, LPVOID lpParam /*= NULL*/)
{
    if (m_bIsRunning)
    {
        return false;
    }
    // copie le code
    m_Code = szCode;
    m_CodeName = szCodeName;
    m_FileName.clear();
    m_pNotificationCallback = pNotificationCallback;
    m_lpParam               = lpParam;

    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    // effacement de la fenetre de sortie des traces LUA
    pMainFrame->ClearTrace(CTraceBar::LUA_TRACE);
    // et activation de la page
    pMainFrame->ShowTrace(CTraceBar::LUA_TRACE);

    // demarre le thread
    m_pThreadExec.reset(new CAdeThreadAsMember(StaticProcThreadExec, this));
    m_pThreadExec->BeginThread(THREAD_PRIORITY_NORMAL);
    return true;
}
bool CLuaInterpreter::StartFromFile(const std::string& fileName, T_NOTIFICATIONCALLBACK pNotificationCallback /*= NULL*/, LPVOID lpParam /*= NULL*/)
{
    if (m_bIsRunning)
    {
        return false;
    }
    // copie le code
    m_Code.clear();
    m_FileName = fileName;
    m_pNotificationCallback = pNotificationCallback;
    m_lpParam               = lpParam;

    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    // effacement de la fenetre de sortie des traces LUA
    pMainFrame->ClearTrace(CTraceBar::LUA_TRACE);
    // et activation de la page
    pMainFrame->ShowTrace(CTraceBar::LUA_TRACE);

    // demarre le thread
    m_pThreadExec.reset(new CAdeThreadAsMember(StaticProcThreadExec, this));
    m_pThreadExec->BeginThread(THREAD_PRIORITY_NORMAL);
    return true;
}

void CLuaInterpreter::WaitTerminate(DWORD timeoutMs /*= INFINITE*/)
{
    if (m_pThreadExec.get())
    {
        m_pThreadExec->WaitTerminate(timeoutMs);
    }
}
void CLuaInterpreter::lineHook (lua_State* L, lua_Debug* ar)
{
#if 0
    lua_getinfo(L, "lnuS", ar);
    m_pThis->m_pAr = ar;

    if ( ar->source[0] == '@' )
        CDebugger::GetDebugger()->LineHook(ar->source+1, ar->currentline);
#endif
}

void CLuaInterpreter::funcHook (lua_State* L, lua_Debug* ar)
{
#if 0
    lua_getinfo(L, "lnuS", ar);
    m_pThis->m_pAr = ar;

    const char* szSource=NULL;
    if ( ar->source[0] == '@' )
    {
        szSource=ar->source+1;
    }
    CDebugger::GetDebugger()->FunctionHook(szSource, ar->currentline, ar->event==LUA_HOOKCALL);
#endif
}
 
void CLuaInterpreter::hook (lua_State* L, lua_Debug* ar)
{
	CLuaInterpreter* itp= static_cast<CLuaInterpreter*>(L->user_param);
    // On regarde si une demande d'arrêt a été faite
    if (itp->m_bStop)
    {
        // arret demande
		luaL_error(L, "User stop");		// abort now
        return;
    }

#if 0
    switch(ar->event)
    {
    case LUA_HOOKTAILRET:
    case LUA_HOOKRET:
        funcHook(L,ar);
        break;
    case LUA_HOOKCALL:
        funcHook(L,ar);
        break;
    case LUA_HOOKLINE:
        lineHook(L,ar);
        break;
    }
#endif
}
// fonction statique passe au thread de management
UINT CLuaInterpreter::StaticProcThreadExec(LPVOID lpParam, HANDLE hEventStop)
{
    return ((CLuaInterpreter*)lpParam)->ProcThreadExec(hEventStop);
}
UINT CLuaInterpreter::ProcThreadExec(HANDLE hEventStop)
{
    static char szFunc[] = "CLuaInterpreter::ProcThreadExec";
    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;

    m_bStop = false;

    // Initialisation LUA
    m_LuaState = lua_open();
    if (m_LuaState == NULL)
    {
        ADE_ERROR("Unable to initialize LUA");
    }
    else
    {
        // on indique que l'execution commence
        m_bIsRunning = true;
        // On se lit au manager (afin de pouvoir être notifié si une deconnection intervient)
        CLuaInterpreterMgr::ReadSingleInstance()->NotifyStartOfExec(this);

        lua_gc(m_LuaState, LUA_GCSTOP, 0);  /* stop collector during initialization */
        luaL_openlibs(m_LuaState);  /* open libraries */
        ITFLUA_open(m_LuaState);
        // Puis on ouvre les autres DLL
        CLuaDllMgr::ReadSingleInstance()->OpenLibs(m_LuaState);

        lua_gc(m_LuaState, LUA_GCRESTART, 0);

        int ret;
        if (m_Code.size())
        {
            // chargement depuis un buffer
            ret = luaL_loadbuffer(m_LuaState, m_Code.c_str(), m_Code.size(), m_CodeName.c_str());
        }
        else
        {
            // chargement depuis un fichier
            ret = luaL_loadfile(m_LuaState, m_FileName.c_str());
        }
        if (ret != 0)
        {
            const char* e = lua_tostring(m_LuaState, -1);
            if (e == NULL)
            {
                char szTmp[] = "LUA: Unknown error in luaL_loadbuffer";
                pMainFrame->SendToTraceWindow(szTmp, CTraceBar::LUA_TRACE);
            }
            else
            {
                pMainFrame->SendToTraceWindow((char*)e, CTraceBar::LUA_TRACE);
            }
        }
        else
        {
            m_LuaState->user_param = (void *)this;
            lua_sethook(m_LuaState, hook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
            ret = lua_pcall(m_LuaState, 0, 0, 0);
            if (ret != 0)
            {
                const char* e = lua_tostring(m_LuaState, -1);
                if (e == NULL)
                {
                    char szTmp[] = "LUA: Unknown error in lua_pcall";
                    pMainFrame->SendToTraceWindow(szTmp, CTraceBar::LUA_TRACE);
                }
                else
                {
    #if 1
                    pMainFrame->SendToTraceWindow((char*)e, CTraceBar::LUA_TRACE);
    #else
                    const char* halt = "HALT: ";
                    bool is_halt = true;
                    for (int i = 0; i < 6 && is_halt; ++i)
                        if (halt[i] != e[i])
                            is_halt = false;
                    if (is_halt)
                        m_ErrorMessage = e;
                    else
                        m_ErrorMessage.Format("ERROR: %s", e);
    #endif
                }
            }
        }
        // puis on ferme tout
        lua_close(m_LuaState);
        ITFLUA_close();
        CLuaDllMgr::ReadSingleInstance()->CloseLibs();
        // On indique que l'exec est finie avant de faire le NotifyEndOfExec
        // car : 1. dans CLuaInterpreter::NotifyDisconnect on fait une attente actif jusqu'à voir
        //          m_bIsRunning = false
        //       2. le thread, lorqu'il va appelé NotifyEndOfExec risque d'être bloqué sur l'attente
        //          de section critique (peut-être prise par CLuaInterpreterMgr::NotifyDisconnect) donc il faut
        //          mettre a jour m_bIsRunning avant pour éviter un deadlock
        m_bIsRunning = false;
        // On supprime le lien avec le manager
        CLuaInterpreterMgr::ReadSingleInstance()->NotifyEndOfExec(this);
    }
    m_bIsRunning = false;
    // et on notifie la fin
    if (m_pNotificationCallback)
    {
        m_pNotificationCallback(m_lpParam, EVT_END);
    }
    return 0;

}


////////////////////////////////////////////////////////////////////////////////
/// Obtention de l'instance unique du gestionnaire d'interpreteurs
/// Il est utilisé afin de notifier la déconnection aux interpréteurs en cours
/// d'exécution
///
/// \return : pointeur sur le gestionnaire
////////////////////////////////////////////////////////////////////////////////
CLuaInterpreterMgr* CLuaInterpreterMgr::pSingleInstance = NULL;
CLuaInterpreterMgr* CLuaInterpreterMgr::ReadSingleInstance(void)
{
    if (pSingleInstance == NULL)
        pSingleInstance = new(CLuaInterpreterMgr);

    return pSingleInstance;
}
////////////////////////////////////////////////////////////////////////////////
/// Destruction du gestionnaire de connexions
////////////////////////////////////////////////////////////////////////////////
void CLuaInterpreterMgr::DeleteSingleInstance(void)
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
CLuaInterpreterMgr::CLuaInterpreterMgr()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Destructeur
////////////////////////////////////////////////////////////////////////////////
CLuaInterpreterMgr::~CLuaInterpreterMgr()
{
}

////////////////////////////////////////////////////////////////////////////////
// Fonction appelée par un interpréteur juste avant qu'il ne commence l'exécution
// d'un script => on l'insere dans le dico, ainsi il sera notifié si une déconnection
// intervient pendant qu'il exécute un script
////////////////////////////////////////////////////////////////////////////////
void CLuaInterpreterMgr::NotifyStartOfExec(CLuaInterpreter* pLuaInterpreter)
{
    // Entree en section critique pour protéger l'accès au dictionnaire
    CAdeSLock lock(m_criticalSection);

    std::set<CLuaInterpreter*>::iterator iter = m_setLuaInterpretersInExec.find(pLuaInterpreter);
    if (iter != m_setLuaInterpretersInExec.end())
    {
        // Deja dans le dico => on ne fait rien
        return;
    }
    // On l'insère dans le dico
    m_setLuaInterpretersInExec.insert(pLuaInterpreter);
}
////////////////////////////////////////////////////////////////////////////////
// Fonction appelée par un interpréteur juste après qu'il est fini l'exécution
// d'un script => il est supprimé du dico
////////////////////////////////////////////////////////////////////////////////
void CLuaInterpreterMgr::NotifyEndOfExec(CLuaInterpreter* pLuaInterpreter)
{
    // Entree en section critique pour protéger l'accès au dictionnaire
    CAdeSLock lock(m_criticalSection);

    std::set<CLuaInterpreter*>::iterator iter = m_setLuaInterpretersInExec.find(pLuaInterpreter);
    if (iter == m_setLuaInterpretersInExec.end())
    {
        // pas dans le dico => on ne fait rien
        return;
    }
    // On le supprime du dico
    m_setLuaInterpretersInExec.erase(iter);
}
