extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tclass.h"


Tst_A* Tst_A::last;
Tst_B* Tst_B::last;
Tst_C* Tst_C::last;

int main ()
{
	Tst_B* b = new Tst_B;         // instance used in Lua code
	int  tolua_tclass_open (lua_State*);

	lua_State* L = lua_open();
	luaL_openlibs(L);
	tolua_tclass_open(L);

	luaL_dofile(L,"tclass.lua");

	lua_close(L);

	delete b;
	return 0;
}

