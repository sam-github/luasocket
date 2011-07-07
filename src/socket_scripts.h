#ifndef LUASOCKET_SCRIPTS_H_
#	define LUASOCKET_SCRIPTS_H_
struct lua_State;
LUASOCKET_API int luaopen_luasocket(struct lua_State *L);
LUASOCKET_API int luaopen_socket(struct lua_State *L);
LUASOCKET_API int luaopen_mime(struct lua_State *L);
#endif
