--lifted from premake4
--[[
Copyright (c) 2003-2011 Jason Perkins and individual contributors.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3. Neither the name of the Premake nor the names of its contributors may be 
     used to endorse or promote products derived from this software without
     specific prior written permission.
	 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

--]]
--
-- Embed the Lua scripts into src/host/scripts.c as static data buffers.
-- I embed the actual scripts, rather than Lua bytecodes, because the 
-- bytecodes are not portable to different architectures, which causes 
-- issues in Mac OS X Universal builds.
--

	--uses lhf's lstrip
	local function stripfile(fname)
		os.execute('./lstrip ' .. fname ..' > '..fname ..'_')
		local f = io.open(fname..'_')
		local s = f:read("*a")
		f:close()
		os.execute('rm ' ..fname ..'_')
		
---		s = s:gsub('require%("socket%.core"%)','require"luasocket" ')
--		s = s:gsub('require("socket"])?','require\'luasocket\' ')
		
		-- escape double quote marks
		s = s:gsub("\\", "\\\\")
		-- escape double quote marks
		s = s:gsub("\"", "\\\"")
		
		s = s:gsub("\n", "\\n")
		return s
	end

	--pure lua implementation 
	--NOTE not fully checked and can not get too aggressive whichout a lexer as it is an ouput format
	local lua_stripfile = function(fname)
	
		local f = io.open(fname)
		local s = f:read("*a")
		f:close()
		
		--s = s:gsub('require%s*%(?"socket%.core"%s*%)?','require\'luasocket\' ')
		
		-- strip tabs
		s = s:gsub("[\t]", "")
		
		-- strip any CRs
		s = s:gsub("[\r]", "")
		
		-- strip out comments
		--s = s:gsub("\n%-%-[^\n]*", "")
		
		-- escape backslashes
		s = s:gsub("\\", "\\\\")

		-- strip out leading comments
		--s = s:gsub("^%-%-\n", "")
		s = s:gsub("%-%-+.-\n+", " ")
		
		-- strip duplicate line feeds
		s = s:gsub("\n+", "\n")
		--s = s:gsub("\n+", "\\n")
		-- escape line feeds
		s = s:gsub("\n", "\\n")
--		s = s:gsub('\n',' ')
		
		-- escape double quote marks
		s = s:gsub("\"", "\\\"")
		--s = s:gsub("\"", "'")
		
		s = s:gsub('%s+=','=')
		s = s:gsub('=%s+','=')
		
		s = s:gsub('if%s+%(','if(')
		s = s:gsub('%)%s+then',')then')
		s = s:gsub('for%s+%(','for(')
		s = s:gsub('%)%s+end',')end')
		s = s:gsub('%)%s+else',')else')
		s = s:gsub('%)%s+local',')local')
		
		s = s:gsub('%s+or%s+{',' or{')
		s = s:gsub('{%s+','{')
		s = s:gsub('%s+}','}')
		
		s = s:gsub('{%s+%[','{[')
		s = s:gsub(',%s+%[',',[')
		
		s = s:gsub("%s+%.%.%s+", "..")
		
		s = s:gsub('%s+~=','~=')
		s = s:gsub('~=%s+','~=')
		
		s = s:gsub('%s+>=','>=')
		s = s:gsub('>=%s+','>=')
		
		s = s:gsub("%s%s+", " ")

--smtp uses a string which contains ' , '		
--		s = s:gsub('%s+,',',')
--		s = s:gsub(',%s+',',') 
		 		
		return s
	end


	local function writeline(out, s, continues)
		out:write("\t\"")
		out:write(s)
		local towrite = "\",\n"
		if continues then towrite = "\"\n" end
		out:write(towrite)
	end
	
	
	local function writefile(out, fname, contents)
		--local max = 1024 --vs limit?
		local max = 509 
		--local total_literal_max = 509 --c90 total limit
		--local total_literal_max = 4095 --c99 total limit


		-- break up large strings to fit in Visual Studio's string length limit		
		local start = 1
		local len = contents:len()
		while start <= len do
			local n = len - start
			if n > max then n = max end
			local finish = start + n

			-- make sure I don't cut an escape sequence
			while contents:sub(finish, finish) == "\\" do
				finish = finish - 1
			end			

			writeline(out, contents:sub(start, finish), finish < len)
			start = finish + 1
		end		

	end



local constant_string_data = function(out)

	out:write(
[[#include "lua.h"
#include "lauxlib.h"
#include "socket_scripts.h"
#include "luasocket.h"
#include "mime.h"
#include <string.h>

#if LUA_VERSION_NUM < 502
#	define LUA_OK 0
static void luaL_preload(L)
{	
	lua_getglobal(L,"package");
	lua_getfield(L, -1, "preload");
}
#	define LUA_ERRGCMM 1000
#else
static void luaL_preload(lua_State* L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "_PRELOAD");
}
#endif

static int load_and_run_buffer(lua_State* L,int char_buff_index,char const* name)
{
	int const top = lua_gettop(L);
	int const res = luaL_loadbuffer(L,luasocket_scripts[char_buff_index],strlen(luasocket_scripts[char_buff_index]),name);
	if (res == LUA_OK)
	{
		lua_pcall(L, 0, LUA_MULTRET, 0);
		return top - lua_gettop(L);
	}
	else if(res == LUA_ERRSYNTAX)
		printf("LUA_ERRSYNTAX %s\n",lua_tostring(L,-1));
	else if(res == LUA_ERRMEM)
		printf("LUA_ERRMEM %s\n",lua_tostring(L,-1));	
	else if(res == LUA_ERRGCMM)
		printf("LUA_ERRGCMM %s\n",lua_tostring(L,-1));
	
	return 0;
}

static int luasocket_needs_loading(lua_State* L)
{
	int const top = lua_gettop(L);
	luaL_preload(L);
	lua_pushliteral(L,"url.lua");
	lua_gettable(L, -2);
	int const needs_loading = lua_type(L, -1);
	lua_settop(L, top);
	return needs_loading == LUA_TNIL;
}

]])

end


local write_functions = function(out,scripts)
	--write helper
	local p =function(...) out.write(out,...) end

	constant_string_data(out)

	--loader functions
	for k,v in ipairs(scripts) do
		local name , _ = v:match('(.-)(%.)') 
		p(
		'static int luasocket_' .. name .. '_loader(lua_State* L)\n'..
		'{\n'
		)
			if name == 'socket' then
				p '\tluaopen_socket_core(L);\n'
			elseif name == 'mime' then
				p '\tluaopen_mime_core(L);\n'
			end
			
			p('\treturn load_and_run_buffer(L,' .. k - 1 .. ',"embedded ' .. v ..'");\n'..
		'}\n')	
	end
	
	local socket_sub_module =
	{
		ftp = 1
		,headers = 1
		,http = 1
		,smtp = 1
		,tp = 1
		,url = 1
	}
	--returns a sub module if required
	local module_name = function(n)
		if socket_sub_module[n] then return 'socket.'..n end
		return n
	end
	
	
	
	--mapping of names to loaders
	p(
'static luaL_Reg embedded_modules[] =\n' ..
'{\n')
	for k,v in ipairs(scripts) do
		local name , _ = v:match('(.-)(%.)') 
		if k ~= 1 then p('\t,') else p('\t') end
		p('{"' .. module_name(name) .. '",luasocket_' .. name ..'_loader}\n')
	end
	
	p('\t,{0,0}\n'..
'};\n')	

	--intailiser which sets the loaders for modules
	p([[
LUASOCKET_API int luaopen_luasocket(lua_State *L)
{
	int top = lua_gettop(L);
	luaL_preload(L);
	int loaders = lua_gettop(L);
	for (int i = 0; embedded_modules[i].name ;++i)
	{
		lua_pushstring(L,embedded_modules[i].name);
		lua_pushcclosure(L, embedded_modules[i].func, 0);
		lua_settable(L, loaders);
	}
	lua_settop(L, top);
	return 0;
}
]] )

	--special functions to be used when there are symlinks in place mime.so and socket.so
	--and a call to require with 'socket' or 'mime' is used yet not require('luasocket')
	p 'LUASOCKET_API int luaopen_socket(lua_State *L)\n'
	p '{\n'
		p '\tif (luasocket_needs_loading(L)) luaopen_luasocket(L);\n'
		p '\treturn luasocket_socket_loader(L);\n'
	p '}\n'

	p 'LUASOCKET_API int luaopen_mime(lua_State *L)\n'
	p '{\n'
		p '\tif (luasocket_needs_loading(L)) luaopen_luasocket(L);\n'
		p '\treturn luasocket_mime_loader(L);\n'
	p '}\n\n'
	
end


local write_file_data = function(out,scripts)
	out:write "static const char* luasocket_scripts[] = {\n"
		
	for i,fn in ipairs(scripts) do
		print(fn)
		out:write("\n\t/* " .. fn .. " */\n")
		local s = stripfile("src/" .. fn)			
		writefile(out, fn, s)
	end
	
	out:write("\t\n};\n");	
end


function doembed()
	local scripts =
	{
		'socket.lua',
		'mime.lua',
		'ltn12.lua',
		'ftp.lua',
		'headers.lua',
		'http.lua',
		'smtp.lua',
		'tp.lua',
		'url.lua'
	}
		
	local out = io.open("src/socket_scripts.c", "w+b")
		out:write "/* This file is generated, edits to it will not persist if regenerated */ \n\n"
		write_file_data(out,scripts)
		write_functions(out,scripts)
	out:close()
end

doembed()

