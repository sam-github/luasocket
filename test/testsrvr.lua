socket = require("socket");
host = host or "localhost";
port = port or "8383";
server = assert(socket.bind(host, port));
ack = "\n";

local lua_load_func = loadstring
if _VERSION == 'Lua 5.2' then
	lua_load_func = load
end

while 1 do
    print("server: waiting for client connection...");
    control = assert(server:accept());
    while 1 do 
        command = assert(control:receive());
        assert(control:send(ack));
        print(command);
		(lua_load_func(command))();
    end
end
