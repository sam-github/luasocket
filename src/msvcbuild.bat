@setlocal
@set MYCOMPILE=cl /I"..\..\lua\src" /nologo /MD /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /D "_MBCS"
@set MYLINK=link /INCREMENTAL:NO /nologo /NOLOGO /LIBPATH:"..\..\out"

@set BASE_DEPENDENCIES="lua51.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib"

@set SOCKET_DEPENDENCIES=%BASE_DEPENDENCIES%

@set SOCKETSRCFILES=auxiliar.c buffer.c except.c inet.c io.c luasocket.c options.c select.c tcp.c timeout.c udp.c wsocket.c

@rem compile socket.dll
cd src
del /Q *.dll
%MYCOMPILE% /D "LUASOCKET_EXPORTS" /D "LUASOCKET_API=__declspec(dllexport)" /D "LUASOCKET_DEBUG" %SOCKETSRCFILES%
%MYLINK% /DLL %SOCKET_DEPENDENCIES% /out:socket.dll *.obj
del /Q *.obj

@rem compile mime.dll
%MYCOMPILE% /D "MIME_EXPORTS" /D "MIME_API=__declspec(dllexport)" mime.c
%MYLINK% /DLL %BASE_DEPENDENCIES% /out:mime.dll mime.obj
del mime.obj

