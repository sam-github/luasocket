
local luasocket_version = '2.1.1'

local macosx_dylib = function(version)
	configuration {'macosx','sharedlib'}
			prebuildcommands('rm ./libluasocket.'..version..'.dylib; rm ./luasocket.so')
			postbuildcommands('ln -s ./lib/libluasocket.'..version..'-' ..luasocket_version ..'.dylib ./libluasocket.'..version..'.dylib;'
								..'ln -s ./libluasocket.'..version..'.dylib ./luasocket.so')
			linkoptions {'-undefined dynamic_lookup'}
			buildoptions{'-fpic -fno-common -fvisibility=hidden'}
	configuration '*'
end

solution 'lua_socket'
	configurations{'Debug','Release'}
	language 'C'
	
	flags 'ExtraWarnings'
	includedirs '/src'
	files
	{
		'src/luasocket.c'
		,'src/timeout.c'
		,'src/buffer.c'
		,'src/io.c'
    	,'src/auxiliar.c'
		,'src/options.c'
		,'src/inet.c'
		,'src/except.c'
		,'src/select.c'
		,'src/tcp.c'
		,'src/udp.c'
		,'src/lua_typeerror.c'
		,'src/socket_scripts.c'
		,'src/mime.c'
		,'src/*.lua'
	}

	configuration 'windows'
		files {'src/wsocket.h','src/wsocket.c'}
	configuration 'macosx or linux'
		files {'src/usocket.h','src/usocket.c'}
		
	configuration 'Debug'
		defines 'LUASOCKET_DEBUG'
		flags 'Symbols'
		
	configuration 'Release'
		flags 'Optimise'

	configuration{'macosx or linux'}
		buildoptions '-pedantic -Wshadow -Wextra -Wimplicit'
		
			
	--DLUASOCKET_API=__attribute__((visibility(\"default\")))"
	--"LUASOCKET_API=__declspec(dllexport)" 
	configuration 'macosx'
		buildoptions '-mmacosx-version-min=10.1'
		defines
		{
			--can I haz some crazy escaping please
			"\\\"LUASOCKET_API=__attribute__((visibility(\\\\\\\"default\\\\\\\")))\\\"",
			"\\\"MIME_API=__attribute__((visibility(\\\\\\\"default\\\\\\\")))\\\"",
		}
	
	project 'lua51_socket'
		kind 'SharedLib'
		includedirs{'~/Documents/projects/lua_env/luaenv/lua_versions/lua_include/lua51/'}
		targetname ('luasocket.51-' ..luasocket_version)
		targetdir './lib'	
		macosx_dylib '51'
			
	project 'lua52_socket'
		kind 'SharedLib'
		defines 'LUA_COMPAT_MODULE'
		includedirs{'~/Documents/projects/lua_env/luaenv/lua_versions/lua_include/lua52/'}
		targetname ('luasocket.52-' ..luasocket_version)
		targetdir './lib'
		macosx_dylib '52'

