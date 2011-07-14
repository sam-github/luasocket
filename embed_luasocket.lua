
	--uses lhf's lstrip
	local function lstrip_stripfile(fname)
		os.execute('./lstrip ' .. fname ..' > '..fname ..'_')
		local f = io.open(fname..'_')
		local s = f:read("*a")
		f:close()
		os.execute('rm ' ..fname ..'_')
		
		t = {s:byte(1,-1)}
		
		s = s:gsub("\\", "\\\\")
		s = s:gsub("\"", "\\\"")
		s = s:gsub("\n", "\\n")
		
		return s, t
	end
	
	local function writefilebytes(out,bytes,last)
		for k,v in ipairs(bytes) do
			out:write(v)
			if k == #bytes and last == true then return
			else out:write(',') end
		end
	end
	
	local write_embedded_scripts_ = function(out,scripts,strip_file_function)
		local w = function(...) out.write(out,...) end
		local sizes ={}
		local last_entry = #scripts
		w "static const char luasocket_scripts[] = {\n"						
		for index,entry in ipairs(scripts) do
			local file_name = entry.file_name
			print(file_name)
			w("\n\t/* " .. file_name .. " */\n")
			local _, t = strip_file_function("src/" .. file_name)
			local buffer_index = 0
			if index ~= 1 then 
				buffer_index = sizes[index-1].buffer_index + sizes[index-1].size
			end
			sizes[index] = {size = #t, ['buffer_index'] = buffer_index}
			writefilebytes(out, t, index == last_entry )
		end
	
		w("\t\n};\n");	

		w('enum script_sizes {\n')		
		for i,entry in ipairs(sizes) do
			w('\t' .. scripts[i].loader_name ..'_size = ' .. entry.size)
			if i ~= #sizes then w', ' end
			w'\n'
		end
		w("\t\n};\n");
		
		w('enum script_indexes {\n')		
		for i,entry in ipairs(sizes) do
			w('\t' .. scripts[i].loader_name ..'_index = ' .. entry.buffer_index)
			if i ~= #sizes then w', ' end
			w'\n'
		end
		w("\t\n};\n");
		
	end
	

local loader_constant_prefix = function(out)

	out:write([[
#include "lua.h"
#include "lauxlib.h"
#include "socket_scripts.h"
#include "luasocket.h"
#include "mime.h"
#include <string.h>

static void preload_error_if_stack_top_is_not_a_table(lua_State* L)
{
	if (lua_type(L,-1) != LUA_TTABLE )
		luaL_error(L, "Lua %d get_preload_table failed to retrieve the preload table. Stack top is %s\n"
		,LUA_VERSION_NUM,lua_typename(L,-1));
}

#if LUA_VERSION_NUM < 502
#	define LUA_OK 0
#	define LUA_ERRGCMM 1000 /*number not returned as an error for this Lua version*/
	static void get_preload_table(lua_State* L)
	{	
		lua_getglobal(L,"package");
		lua_getfield(L, -1, "preload");
		preload_error_if_stack_top_is_not_a_table(L);
	}
#else
	static void get_preload_table(lua_State* L)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "_PRELOAD");
		preload_error_if_stack_top_is_not_a_table(L);
	}
#endif

static int load_and_run_buffer(lua_State* L,int char_buff_index,size_t buff_size,char const* name)
{
	int const top = lua_gettop(L);
	int const res = luaL_loadbuffer(L,luasocket_scripts + char_buff_index,buff_size,name);
	if (res == LUA_OK)
	{
		lua_pcall(L, 0, LUA_MULTRET, 0);
		return top - lua_gettop(L);
	}
	else if(res == LUA_ERRSYNTAX)
		lua_pushliteral(L,"LUA_ERRSYNTAX");
	else if(res == LUA_ERRMEM)
		lua_pushliteral(L,"LUA_ERRMEM");	
	else if(res == LUA_ERRGCMM)
		lua_pushliteral(L,"LUA_ERRGCMM");
	
	lua_pushfstring(L,"ERROR: %s when loading %s :\n\t%s\n",lua_tostring(L,-1),name,lua_tostring(L,-2));
	return lua_error(L);
}

]])

end

local write_module_data = function(out,embedded_scripts)
	local w = function(...) out.write(out,...) end
	--mapping of names to loaders
	w([[
static luaL_Reg embedded_modules[] =
{
]]	)
	for index,entry in ipairs(embedded_scripts) do
		if index ~= 1 then w('\t,') else w('\t') end
		w('{"' .. entry.module_name .. '",luasocket_' .. entry.loader_name ..'_loader}\n')
	end
	
	w([[
	,{"socket.core",luaopen_socket_core}
	,{"mime.core",luaopen_mime_core}
	,{0,0}
};

static void luasocket_register_modules(lua_State *L,luaL_Reg* modules)
{
	int top = lua_gettop(L);
	get_preload_table(L);
	int loaders = lua_gettop(L);
	for (int i = 0; modules[i].name ;++i)
	{
		lua_pushstring(L,modules[i].name);
		lua_pushcclosure(L, modules[i].func, 0);
		lua_settable(L, loaders);
	}
	lua_settop(L, top);
}	

]]	)
end

local write_openlib_loaders = function(out,embedded_scripts)

	assert(embedded_scripts[1].file_name =='socket.lua')
	out:write([[
LUASOCKET_API int luaopen_luasocket(lua_State *L)
{
	luasocket_register_modules(L,embedded_modules);
	return 0;
}

/*
Called via "require 'socket'" when the library is a shared object or a symlink with the name
'socket.so' It registers the needed module loaders and runs the socket module returning the results
*/
LUASOCKET_API int luaopen_socket(lua_State *L)
{
	/*loader mapping is generated with the module 'socket' as index 0*/
	luasocket_register_modules(L,embedded_modules + 1);/*skip the socket module this function is it*/
	return luasocket_socket_loader(L);
}


]]	)
end



local write_loader_functions__ = function(out,embedded_scripts)

	loader_constant_prefix(out)
	
	local w = function(...) out.write(out,...) end
	--loader functions
	for index,entry in ipairs(embedded_scripts) do
		w(
'static int luasocket_' .. entry.loader_name .. '_loader(lua_State* L)\n'..
'{\n'..
	'\treturn load_and_run_buffer(L,' .. entry.loader_name .. '_index' .. ','.. entry.loader_name ..'_size' .. ',"embedded ' .. entry.file_name ..'");\n'..
'}\n'	)
	end
	
	write_module_data(out,embedded_scripts)
	write_openlib_loaders(out,embedded_scripts)

end




function doembed()

	local header = io.open('src/socket_scripts.h','w+b')
	header:write([[
#ifndef LUASOCKET_SCRIPTS_H_
#	define LUASOCKET_SCRIPTS_H_

struct lua_State;
int luaopen_luasocket(struct lua_State *L);

#endif
]]	)
	header:close()

	local file_module_loader = function(f,m) 
		local loader_name , _ = f:match('(.-)(%.)') 
		return {['file_name']= f,['module_name']= m,['loader_name'] = loader_name} 
	end

	local embedded_scripts =
	{	
		[1] = file_module_loader('socket.lua','socket'),
		[2] = file_module_loader('mime.lua','mime'),
		[3] = file_module_loader('ltn12.lua','ltn12'),
		[4] = file_module_loader('ftp.lua','socket.ftp'),
		[5] = file_module_loader('headers.lua','socket.headers'),
		[6] = file_module_loader('http.lua','socket.http'),
		[7] = file_module_loader('smtp.lua','socket.smtp'),
		[8] = file_module_loader('tp.lua','socket.tp'),
		[9] = file_module_loader('url.lua','socket.url'),
	}
	
	--[[
	The module socket can be special in that it registers the module loaders
	when the library is a shared object or a symlink named "socket.so".
	In this instance it does not register the socket loader as one is found
	using symbol lookup in the so.
	This is an index requirement in the generated file
	--]]
	assert(embedded_scripts[1].module_name =='socket')
	
	local out = io.open("src/socket_scripts.c", "w+b")
		out:write "/* This file is generated, edits to it will not persist if regenerated */ \n\n"
		local stripfile = lstrip_stripfile
							--lua_stripfile
							
		write_embedded_scripts_(out,embedded_scripts,stripfile)
		write_loader_functions__(out,embedded_scripts)
		
	out:close()
end

doembed()

