
local luasocket_version = '2.1.1'

local macosx_dylib = function(lua_version)
	configuration {'macosx','sharedlib'}
			prebuildcommands('rm ./libluasocket.'..lua_version..'.dylib; rm ./luasocket.so')
			postbuildcommands('ln -s ./lib/libluasocket.'..lua_version..'-' ..luasocket_version ..'.dylib ./libluasocket.'..lua_version..'.dylib;'
								..'ln -s ./libluasocket.'..lua_version..'.dylib ./luasocket.so')
			linkoptions {'-undefined dynamic_lookup'}
			buildoptions{'-fpic -fno-common -fvisibility=hidden'}
	configuration '*'
end

local lua_includes = '~/Documents/projects/lua_env/luaenv/lua_versions/lua_include/'
local include_51_dir = lua_includes .. 'lua51/'
local include_52_dir = lua_includes .. 'lua52/'

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
	
	project 'lua51_shared_socket'
		kind 'SharedLib'
		includedirs{ include_51_dir }
		targetname ('luasocket.51-' ..luasocket_version)
		targetdir './lib'	
		macosx_dylib '51'
			
	project 'lua52_shared_socket'
		kind 'SharedLib'
		defines 'LUA_COMPAT_MODULE'
		includedirs{ include_52_dir }
		targetname ('luasocket.52-' ..luasocket_version)
		targetdir './lib'
		macosx_dylib '52'

	project 'lua51_static_socket'
		kind 'StaticLib'
		defines 'LUA_COMPAT_MODULE'
		includedirs{ include_51_dir }
		targetname ('luasocket.51-' ..luasocket_version)
		targetdir './lib'
		
	project 'lua52_static_socket'
		kind 'StaticLib'
		defines 'LUA_COMPAT_MODULE'
		includedirs{ include_52_dir }
		targetname ('luasocket.52-' ..luasocket_version)
		targetdir './lib'

solution 'lua_socket_test'
	configurations{'Debug','Release'}
	kind 'ConsoleApp'
	language 'C'	
	flags 'ExtraWarnings'
	targetdir './lib/'
	files './test/test_lua_socket.c'
	
	configuration 'Debug'
		flags 'Symbols'
		
	configuration 'Release'
		flags 'Optimise'

	configuration{'macosx or linux'}
		buildoptions '-pedantic -Wshadow -Wextra -Wimplicit'
		
	configuration 'macosx'
		buildoptions '-mmacosx-version-min=10.1'

local lua_lib_dir = '/Users/apple/Documents/projects/lua_env/luaenv/lua_versions/lua_lib/'

	project 'lua52_static_test'
		defines 'LUA_COMPAT_MODULE'
		includedirs{ include_52_dir,'./src' }
		targetname ('luasocket_test.52-' ..luasocket_version )
		linkoptions { lua_lib_dir .. 'lua52/liblua.a' }
		links 'lua52_static_socket'
		
	project 'lua51_static_test'
		kind 'ConsoleApp'
		includedirs{ include_51_dir,'./src '}
		targetname ('luasocket_test.51-' ..luasocket_version )
		linkoptions { lua_lib_dir .. 'lua51/liblua.a' }
		links 'lua51_static_socket'