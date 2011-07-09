#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "../src/socket_scripts.h"
#include <stdio.h>
int main(int argc, char * argv[])
{
	char * file = 0;
	if(argc < 2)
	{
		file = "./test/hello.lua";
		printf("requires a file to run, trying to load %s instead\n",file);
	}
	else {
		file = argv[1];
	}

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_luasocket(L);
	if( luaL_dofile(L,file) != 0)
	{
		printf("%s\n",lua_tostring(L,-1));
		return 1;
	}
	return 0;
}
