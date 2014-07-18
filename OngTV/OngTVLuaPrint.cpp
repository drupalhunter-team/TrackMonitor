// AdeviewLuaPrint : surcharge de la fonction luaB_print
// Note : le fichier LUA lbaselib.c a ete modifie pour faire reference a la nouvelle fonction

#include "stdafx.h"
#include "OngTV.h"
#include "MainFrm.h"
#include <ctype.h>

extern "C" 
{
    #include "lua.h" 
    #include "lauxlib.h" 
    #include "lualib.h"
}

extern "C" int AdeviewLuaPrint (lua_State* L) {

    char szTab[10] = "\t";
    CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    lua_getglobal(L, "tostring");
    for (i=1; i<=n; ++i) {
        const char* s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        if (s == NULL)
            return luaL_error(L, LUA_QL("tostring") " must return a string to "
                LUA_QL("print"));
        if (i>1) pMainFrame->SendToTraceWindow(szTab, CTraceBar::LUA_TRACE);
        pMainFrame->SendToTraceWindow((char*)s, CTraceBar::LUA_TRACE);
        lua_pop(L, 1);  /* pop result */
    }
    //  pMainFrame->SendToTraceWindow("\n", CTraceBar::LUA_TRACE);
    return 0;
}
