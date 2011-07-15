local socket,foo,bar = require"socket"
local mime = require"mime"
print("Hello from " .. socket._VERSION .. " and " .. mime._VERSION .. " using " .. _VERSION .. "!")
