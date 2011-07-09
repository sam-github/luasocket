/* This file is generated, edits to it will not persist if regenerated */ 

static const char* luasocket_scripts[] = {

	/* socket.lua */
	"local base=_G local string=require(\"string\")local math=require(\"math\")local socket=require(\"socket.core\")module(\"socket\")function connect(address,port,laddress,lport)if address==\"*\"then address=\"0.0.0.0\"end local addrinfo,err=socket.dns.getaddrinfo(address);if not addrinfo then return nil,err end local sock,err;if addrinfo[1].family==\"inet\"then sock,err=socket.tcp()else sock,err=socket.tcp6()end if not sock then return nil,err end if laddress then local res,err=sock:bind(laddress,lport,-1)if"
	" not res then return nil,err end end local res,err=sock:connect(address,port)if not res then return nil,err end return sock end function bind(host,port,backlog)if host==\"*\"then host=\"0.0.0.0\"end local addrinfo,err=socket.dns.getaddrinfo(host);if not addrinfo then return nil,err end local sock,err;if addrinfo[1].family==\"inet\"then sock,err=socket.tcp()else sock,err=socket.tcp6()end if not sock then return nil,err end sock:setoption(\"reuseaddr\",true)local res,err=sock:bind(host,port)if not res then "
	"return nil,err end res,err=sock:listen(backlog)if not res then return nil,err end return sock end try=newtry()function choose(table)return function(name,opt1,opt2)if base.type(name)~=\"string\"then name,opt1,opt2=\"default\",name,opt1 end local f=table[name or\"nil\"]if not f then base.error(\"unknown key (\"..base.tostring(name)..\")\",3)else return f(opt1,opt2)end end end sourcet={}sinkt={}BLOCKSIZE=2048 sinkt[\"close-when-done\"]=function(sock)return base.setmetatable({getfd=function()return sock:getfd"
	"()end,dirty=function()return sock:dirty()end},{__call=function(self,chunk,err)if not chunk then sock:close()return 1 else return sock:send(chunk)end end})end sinkt[\"keep-open\"]=function(sock)return base.setmetatable({getfd=function()return sock:getfd()end,dirty=function()return sock:dirty()end},{__call=function(self,chunk,err)if chunk then return sock:send(chunk)else return 1 end end})end sinkt[\"default\"]=sinkt[\"keep-open\"]sink=choose(sinkt)sourcet[\"by-length\"]=function(sock,length)return base.set"
	"metatable({getfd=function()return sock:getfd()end,dirty=function()return sock:dirty()end},{__call=function()if length<=0 then return nil end local size=math.min(socket.BLOCKSIZE,length)local chunk,err=sock:receive(size)if err then return nil,err end length=length-string.len(chunk)return chunk end})end sourcet[\"until-closed\"]=function(sock)local done return base.setmetatable({getfd=function()return sock:getfd()end,dirty=function()return sock:dirty()end},{__call=function()if done then return nil end local"
	" chunk,err,partial=sock:receive(socket.BLOCKSIZE)if not err then return chunk elseif err==\"closed\"then sock:close()done=1 return partial else return nil,err end end})end sourcet[\"default\"]=sourcet[\"until-closed\"]source=choose(sourcet)",

	/* mime.lua */
	"local base=_G local ltn12=require(\"ltn12\")local mime=require(\"mime.core\")local io=require(\"io\")local string=require(\"string\")module(\"mime\")encodet={}decodet={}wrapt={}local function choose(table)return function(name,opt1,opt2)if base.type(name)~=\"string\"then name,opt1,opt2=\"default\",name,opt1 end local f=table[name or\"nil\"]if not f then base.error(\"unknown key (\"..base.tostring(name)..\")\",3)else return f(opt1,opt2)end end end encodet[\"base64\"]=function()return ltn12.filter.cycle(b64,"
	"\"\")end encodet[\"quoted-printable\"]=function(mode)return ltn12.filter.cycle(qp,\"\",(mode==\"binary\")and\"=0D=0A\"or\"\\r\\n\")end decodet[\"base64\"]=function()return ltn12.filter.cycle(unb64,\"\")end decodet[\"quoted-printable\"]=function()return ltn12.filter.cycle(unqp,\"\")end local function format(chunk)if chunk then if chunk==\"\"then return\"''\"else return string.len(chunk)end else return\"nil\"end end wrapt[\"text\"]=function(length)length=length or 76 return ltn12.filter.cycle(wrp,length,len"
	"gth)end wrapt[\"base64\"]=wrapt[\"text\"]wrapt[\"default\"]=wrapt[\"text\"]wrapt[\"quoted-printable\"]=function()return ltn12.filter.cycle(qpwrp,76,76)end encode=choose(encodet)decode=choose(decodet)wrap=choose(wrapt)function normalize(marker)return ltn12.filter.cycle(eol,0,marker)end function stuff()return ltn12.filter.cycle(dot,2)end",

	/* ltn12.lua */
	"local string=require(\"string\")local table=require(\"table\")local base=_G module(\"ltn12\")filter={}source={}sink={}pump={}BLOCKSIZE=2048 _VERSION=\"LTN12 1.0.1\"function filter.cycle(low,ctx,extra)base.assert(low)return function(chunk)local ret ret,ctx=low(ctx,chunk,extra)return ret end end function filter.chain(...)local n=table.getn(arg)local top,index=1,1 local retry=\"\"return function(chunk)retry=chunk and retry while true do if index==top then chunk=arg[index](chunk)if chunk==\"\"or top==n then r"
	"eturn chunk elseif chunk then index=index+1 else top=top+1 index=top end else chunk=arg[index](chunk or\"\")if chunk==\"\"then index=index-1 chunk=retry elseif chunk then if index==n then return chunk else index=index+1 end else base.error(\"filter returned inappropriate nil\")end end end end end local function empty()return nil end function source.empty()return empty end function source.error(err)return function()return nil,err end end function source.file(handle,io_err)if handle then return function()lo"
	"cal chunk=handle:read(BLOCKSIZE)if not chunk then handle:close()end return chunk end else return source.error(io_err or\"unable to open file\")end end function source.simplify(src)base.assert(src)return function()local chunk,err_or_new=src()src=err_or_new or src if not chunk then return nil,err_or_new else return chunk end end end function source.string(s)if s then local i=1 return function()local chunk=string.sub(s,i,i+BLOCKSIZE-1)i=i+BLOCKSIZE if chunk~=\"\"then return chunk else return nil end end else"
	" return source.empty()end end function source.rewind(src)base.assert(src)local t={}return function(chunk)if not chunk then chunk=table.remove(t)if not chunk then return src()else return chunk end else table.insert(t,chunk)end end end function source.chain(src,f)base.assert(src and f)local last_in,last_out=\"\",\"\"local state=\"feeding\"local err return function()if not last_out then base.error(\"source is empty!\",2)end while true do if state==\"feeding\"then last_in,err=src()if err then return nil,err e"
	"nd last_out=f(last_in)if not last_out then if last_in then base.error(\"filter returned inappropriate nil\")else return nil end elseif last_out~=\"\"then state=\"eating\"if last_in then last_in=\"\"end return last_out end else last_out=f(last_in)if last_out==\"\"then if last_in==\"\"then state=\"feeding\"else base.error(\"filter returned \\\"\\\"\")end elseif not last_out then if last_in then base.error(\"filter returned inappropriate nil\")else return nil end else return last_out end end end end end func"
	"tion source.cat(...)local src=table.remove(arg,1)return function()while src do local chunk,err=src()if chunk then return chunk end if err then return nil,err end src=table.remove(arg,1)end end end function sink.table(t)t=t or{}local f=function(chunk,err)if chunk then table.insert(t,chunk)end return 1 end return f,t end function sink.simplify(snk)base.assert(snk)return function(chunk,err)local ret,err_or_new=snk(chunk,err)if not ret then return nil,err_or_new end snk=err_or_new or snk return 1 end end func"
	"tion sink.file(handle,io_err)if handle then return function(chunk,err)if not chunk then handle:close()return 1 else return handle:write(chunk)end end else return sink.error(io_err or\"unable to open file\")end end local function null()return 1 end function sink.null()return null end function sink.error(err)return function()return nil,err end end function sink.chain(f,snk)base.assert(f and snk)return function(chunk,err)if chunk~=\"\"then local filtered=f(chunk)local done=chunk and\"\"while true do local re"
	"t,snkerr=snk(filtered,err)if not ret then return nil,snkerr end if filtered==done then return 1 end filtered=f(done)end else return 1 end end end function pump.step(src,snk)local chunk,src_err=src()local ret,snk_err=snk(chunk,src_err)if chunk and ret then return 1 else return nil,src_err or snk_err end end function pump.all(src,snk,step)base.assert(src and snk)step=step or pump.step while true do local ret,err=step(src,snk)if not ret then if err then return nil,err else return 1 end end end end",

	/* ftp.lua */
	"local base=_G local table=require(\"table\")local string=require(\"string\")local math=require(\"math\")local socket=require(\"socket\")local url=require(\"socket.url\")local tp=require(\"socket.tp\")local ltn12=require(\"ltn12\")module(\"socket.ftp\")TIMEOUT=60 PORT=21 USER=\"ftp\"PASSWORD=\"anonymous@anonymous.org\"local metat={__index={}}function open(server,port,create)local tp=socket.try(tp.connect(server,port or PORT,TIMEOUT,create))local f=base.setmetatable({tp=tp},metat)f.try=socket.newtry(functio"
	"n()f:close()end)return f end function metat.__index:portconnect()self.try(self.server:settimeout(TIMEOUT))self.data=self.try(self.server:accept())self.try(self.data:settimeout(TIMEOUT))end function metat.__index:pasvconnect()self.data=self.try(socket.tcp())self.try(self.data:settimeout(TIMEOUT))self.try(self.data:connect(self.pasvt.ip,self.pasvt.port))end function metat.__index:login(user,password)self.try(self.tp:command(\"user\",user or USER))local code,reply=self.try(self.tp:check{\"2..\",331})if code="
	"=331 then self.try(self.tp:command(\"pass\",password or PASSWORD))self.try(self.tp:check(\"2..\"))end return 1 end function metat.__index:pasv()self.try(self.tp:command(\"pasv\"))local code,reply=self.try(self.tp:check(\"2..\"))local pattern=\"(%d+)%D(%d+)%D(%d+)%D(%d+)%D(%d+)%D(%d+)\"local a,b,c,d,p1,p2=socket.skip(2,string.find(reply,pattern))self.try(a and b and c and d and p1 and p2,reply)self.pasvt={ip=string.format(\"%d.%d.%d.%d\",a,b,c,d),port=p1*256+p2}if self.server then self.server:close()self.s"
	"erver=nil end return self.pasvt.ip,self.pasvt.port end function metat.__index:port(ip,port)self.pasvt=nil if not ip then ip,port=self.try(self.tp:getcontrol():getsockname())self.server=self.try(socket.bind(ip,0))ip,port=self.try(self.server:getsockname())self.try(self.server:settimeout(TIMEOUT))end local pl=math.mod(port,256)local ph=(port-pl)/256 local arg=string.gsub(string.format(\"%s,%d,%d\",ip,ph,pl),\"%.\",\",\")self.try(self.tp:command(\"port\",arg))self.try(self.tp:check(\"2..\"))return 1 end func"
	"tion metat.__index:send(sendt)self.try(self.pasvt or self.server,\"need port or pasv first\")if self.pasvt then self:pasvconnect()end local argument=sendt.argument or url.unescape(string.gsub(sendt.path or\"\",\"^[/\\\\]\",\"\"))if argument==\"\"then argument=nil end local command=sendt.command or\"stor\"self.try(self.tp:command(command,argument))local code,reply=self.try(self.tp:check{\"2..\",\"1..\"})if not self.pasvt then self:portconnect()end local step=sendt.step or ltn12.pump.step local readt={self."
	"tp.c}local checkstep=function(src,snk)local readyt=socket.select(readt,nil,0)if readyt[tp]then code=self.try(self.tp:check(\"2..\"))end return step(src,snk)end local sink=socket.sink(\"close-when-done\",self.data)self.try(ltn12.pump.all(sendt.source,sink,checkstep))if string.find(code,\"1..\")then self.try(self.tp:check(\"2..\"))end self.data:close()local sent=socket.skip(1,self.data:getstats())self.data=nil return sent end function metat.__index:receive(recvt)self.try(self.pasvt or self.server,\"need por"
	"t or pasv first\")if self.pasvt then self:pasvconnect()end local argument=recvt.argument or url.unescape(string.gsub(recvt.path or\"\",\"^[/\\\\]\",\"\"))if argument==\"\"then argument=nil end local command=recvt.command or\"retr\"self.try(self.tp:command(command,argument))local code=self.try(self.tp:check{\"1..\",\"2..\"})if not self.pasvt then self:portconnect()end local source=socket.source(\"until-closed\",self.data)local step=recvt.step or ltn12.pump.step self.try(ltn12.pump.all(source,recvt.sink,ste"
	"p))if string.find(code,\"1..\")then self.try(self.tp:check(\"2..\"))end self.data:close()self.data=nil return 1 end function metat.__index:cwd(dir)self.try(self.tp:command(\"cwd\",dir))self.try(self.tp:check(250))return 1 end function metat.__index:type(type)self.try(self.tp:command(\"type\",type))self.try(self.tp:check(200))return 1 end function metat.__index:greet()local code=self.try(self.tp:check{\"1..\",\"2..\"})if string.find(code,\"1..\")then self.try(self.tp:check(\"2..\"))end return 1 end functio"
	"n metat.__index:quit()self.try(self.tp:command(\"quit\"))self.try(self.tp:check(\"2..\"))return 1 end function metat.__index:close()if self.data then self.data:close()end if self.server then self.server:close()end return self.tp:close()end local function override(t)if t.url then local u=url.parse(t.url)for i,v in base.pairs(t)do u[i]=v end return u else return t end end local function tput(putt)putt=override(putt)socket.try(putt.host,\"missing hostname\")local f=open(putt.host,putt.port,putt.create)f:gree"
	"t()f:login(putt.user,putt.password)if putt.type then f:type(putt.type)end f:pasv()local sent=f:send(putt)f:quit()f:close()return sent end local default={path=\"/\",scheme=\"ftp\"}local function parse(u)local t=socket.try(url.parse(u,default))socket.try(t.scheme==\"ftp\",\"wrong scheme '\"..t.scheme..\"'\")socket.try(t.host,\"missing hostname\")local pat=\"^type=(.)$\"if t.params then t.type=socket.skip(2,string.find(t.params,pat))socket.try(t.type==\"a\"or t.type==\"i\",\"invalid type '\"..t.type..\"'\")e"
	"nd return t end local function sput(u,body)local putt=parse(u)putt.source=ltn12.source.string(body)return tput(putt)end put=socket.protect(function(putt,body)if base.type(putt)==\"string\"then return sput(putt,body)else return tput(putt)end end)local function tget(gett)gett=override(gett)socket.try(gett.host,\"missing hostname\")local f=open(gett.host,gett.port,gett.create)f:greet()f:login(gett.user,gett.password)if gett.type then f:type(gett.type)end f:pasv()f:receive(gett)f:quit()return f:close()end loc"
	"al function sget(u)local gett=parse(u)local t={}gett.sink=ltn12.sink.table(t)tget(gett)return table.concat(t)end command=socket.protect(function(cmdt)cmdt=override(cmdt)socket.try(cmdt.host,\"missing hostname\")socket.try(cmdt.command,\"missing command\")local f=open(cmdt.host,cmdt.port,cmdt.create)f:greet()f:login(cmdt.user,cmdt.password)f.try(f.tp:command(cmdt.command,cmdt.argument))if cmdt.check then f.try(f.tp:check(cmdt.check))end f:quit()return f:close()end)get=socket.protect(function(gett)if base.t"
	"ype(gett)==\"string\"then return sget(gett)else return tget(gett)end end)",

	/* headers.lua */
	"module(\"socket.headers\")canonic={[\"accept\"]=\"Accept\",[\"accept-charset\"]=\"Accept-Charset\",[\"accept-encoding\"]=\"Accept-Encoding\",[\"accept-language\"]=\"Accept-Language\",[\"accept-ranges\"]=\"Accept-Ranges\",[\"action\"]=\"Action\",[\"alternate-recipient\"]=\"Alternate-Recipient\",[\"age\"]=\"Age\",[\"allow\"]=\"Allow\",[\"arrival-date\"]=\"Arrival-Date\",[\"authorization\"]=\"Authorization\",[\"bcc\"]=\"Bcc\",[\"cache-control\"]=\"Cache-Control\",[\"cc\"]=\"Cc\",[\"comments\"]=\"Comments\",["
	"\"connection\"]=\"Connection\",[\"content-description\"]=\"Content-Description\",[\"content-disposition\"]=\"Content-Disposition\",[\"content-encoding\"]=\"Content-Encoding\",[\"content-id\"]=\"Content-ID\",[\"content-language\"]=\"Content-Language\",[\"content-length\"]=\"Content-Length\",[\"content-location\"]=\"Content-Location\",[\"content-md5\"]=\"Content-MD5\",[\"content-range\"]=\"Content-Range\",[\"content-transfer-encoding\"]=\"Content-Transfer-Encoding\",[\"content-type\"]=\"Content-Type\",[\"da"
	"te\"]=\"Date\",[\"diagnostic-code\"]=\"Diagnostic-Code\",[\"dsn-gateway\"]=\"DSN-Gateway\",[\"etag\"]=\"ETag\",[\"expect\"]=\"Expect\",[\"expires\"]=\"Expires\",[\"final-log-id\"]=\"Final-Log-ID\",[\"final-recipient\"]=\"Final-Recipient\",[\"from\"]=\"From\",[\"host\"]=\"Host\",[\"if-match\"]=\"If-Match\",[\"if-modified-since\"]=\"If-Modified-Since\",[\"if-none-match\"]=\"If-None-Match\",[\"if-range\"]=\"If-Range\",[\"if-unmodified-since\"]=\"If-Unmodified-Since\",[\"in-reply-to\"]=\"In-Reply-To\",[\"keyw"
	"ords\"]=\"Keywords\",[\"last-attempt-date\"]=\"Last-Attempt-Date\",[\"last-modified\"]=\"Last-Modified\",[\"location\"]=\"Location\",[\"max-forwards\"]=\"Max-Forwards\",[\"message-id\"]=\"Message-ID\",[\"mime-version\"]=\"MIME-Version\",[\"original-envelope-id\"]=\"Original-Envelope-ID\",[\"original-recipient\"]=\"Original-Recipient\",[\"pragma\"]=\"Pragma\",[\"proxy-authenticate\"]=\"Proxy-Authenticate\",[\"proxy-authorization\"]=\"Proxy-Authorization\",[\"range\"]=\"Range\",[\"received\"]=\"Received\",["
	"\"received-from-mta\"]=\"Received-From-MTA\",[\"references\"]=\"References\",[\"referer\"]=\"Referer\",[\"remote-mta\"]=\"Remote-MTA\",[\"reply-to\"]=\"Reply-To\",[\"reporting-mta\"]=\"Reporting-MTA\",[\"resent-bcc\"]=\"Resent-Bcc\",[\"resent-cc\"]=\"Resent-Cc\",[\"resent-date\"]=\"Resent-Date\",[\"resent-from\"]=\"Resent-From\",[\"resent-message-id\"]=\"Resent-Message-ID\",[\"resent-reply-to\"]=\"Resent-Reply-To\",[\"resent-sender\"]=\"Resent-Sender\",[\"resent-to\"]=\"Resent-To\",[\"retry-after\"]=\"Ret"
	"ry-After\",[\"return-path\"]=\"Return-Path\",[\"sender\"]=\"Sender\",[\"server\"]=\"Server\",[\"smtp-remote-recipient\"]=\"SMTP-Remote-Recipient\",[\"status\"]=\"Status\",[\"subject\"]=\"Subject\",[\"te\"]=\"TE\",[\"to\"]=\"To\",[\"trailer\"]=\"Trailer\",[\"transfer-encoding\"]=\"Transfer-Encoding\",[\"upgrade\"]=\"Upgrade\",[\"user-agent\"]=\"User-Agent\",[\"vary\"]=\"Vary\",[\"via\"]=\"Via\",[\"warning\"]=\"Warning\",[\"will-retry-until\"]=\"Will-Retry-Until\",[\"www-authenticate\"]=\"WWW-Authenticate\""
	",[\"x-mailer\"]=\"X-Mailer\",}",

	/* http.lua */
	"local socket=require(\"socket\")local url=require(\"socket.url\")local ltn12=require(\"ltn12\")local mime=require(\"mime\")local string=require(\"string\")local headers=require(\"socket.headers\")local base=_G local table=require(\"table\")module(\"socket.http\")TIMEOUT=60 PORT=80 USERAGENT=socket._VERSION local function receiveheaders(sock,headers)local line,name,value,err headers=headers or{}line,err=sock:receive()if err then return nil,err end while line~=\"\"do name,value=socket.skip(2,string.find(lin"
	"e,\"^(.-):%s*(.*)\"))if not(name and value)then return nil,\"malformed reponse headers\"end name=string.lower(name)line,err=sock:receive()if err then return nil,err end while string.find(line,\"^%s\")do value=value..line line=sock:receive()if err then return nil,err end end if headers[name]then headers[name]=headers[name]..\", \"..value else headers[name]=value end end return headers end socket.sourcet[\"http-chunked\"]=function(sock,headers)return base.setmetatable({getfd=function()return sock:getfd()end"
	",dirty=function()return sock:dirty()end},{__call=function()local line,err=sock:receive()if err then return nil,err end local size=base.tonumber(string.gsub(line,\";.*\",\"\"),16)if not size then return nil,\"invalid chunk size\"end if size>0 then local chunk,err,part=sock:receive(size)if chunk then sock:receive()end return chunk,err else headers,err=receiveheaders(sock,headers)if not headers then return nil,err end end end})end socket.sinkt[\"http-chunked\"]=function(sock)return base.setmetatable({getfd=f"
	"unction()return sock:getfd()end,dirty=function()return sock:dirty()end},{__call=function(self,chunk,err)if not chunk then return sock:send(\"0\\r\\n\\r\\n\")end local size=string.format(\"%X\\r\\n\",string.len(chunk))return sock:send(size..chunk..\"\\r\\n\")end})end local metat={__index={}}function open(host,port,create)local c=socket.try((create or socket.tcp)())local h=base.setmetatable({c=c},metat)h.try=socket.newtry(function()h:close()end)h.try(c:settimeout(TIMEOUT))h.try(c:connect(host,port or PORT))"
	"return h end function metat.__index:sendrequestline(method,uri)local reqline=string.format(\"%s %s HTTP/1.1\\r\\n\",method or\"GET\",uri)return self.try(self.c:send(reqline))end function metat.__index:sendheaders(tosend)local canonic=headers.canonic local h=\"\\r\\n\"for f,v in base.pairs(tosend)do h=(canonic[f]or f)..\": \"..v..\"\\r\\n\"..h end self.try(self.c:send(h))return 1 end function metat.__index:sendbody(headers,source,step)source=source or ltn12.source.empty()step=step or ltn12.pump.step local "
	"mode=\"http-chunked\"if headers[\"content-length\"]then mode=\"keep-open\"end return self.try(ltn12.pump.all(source,socket.sink(mode,self.c),step))end function metat.__index:receivestatusline()local status=self.try(self.c:receive(5))if status~=\"HTTP/\"then return nil,status end status=self.try(self.c:receive(\"*l\",status))local code=socket.skip(2,string.find(status,\"HTTP/%d*%.%d* (%d%d%d)\"))return self.try(base.tonumber(code),status)end function metat.__index:receiveheaders()return self.try(receivehea"
	"ders(self.c))end function metat.__index:receivebody(headers,sink,step)sink=sink or ltn12.sink.null()step=step or ltn12.pump.step local length=base.tonumber(headers[\"content-length\"])local t=headers[\"transfer-encoding\"]local mode=\"default\"if t and t~=\"identity\"then mode=\"http-chunked\"elseif base.tonumber(headers[\"content-length\"])then mode=\"by-length\"end return self.try(ltn12.pump.all(socket.source(mode,self.c,length),sink,step))end function metat.__index:receive09body(status,sink,step)local "
	"source=ltn12.source.rewind(socket.source(\"until-closed\",self.c))source(status)return self.try(ltn12.pump.all(source,sink,step))end function metat.__index:close()return self.c:close()end local function adjusturi(reqt)local u=reqt if not reqt.proxy and not PROXY then u={path=socket.try(reqt.path,\"invalid path 'nil'\"),params=reqt.params,query=reqt.query,fragment=reqt.fragment}end return url.build(u)end local function adjustproxy(reqt)local proxy=reqt.proxy or PROXY if proxy then proxy=url.parse(proxy)ret"
	"urn proxy.host,proxy.port or 3128 else return reqt.host,reqt.port end end local function adjustheaders(reqt)local lower={[\"user-agent\"]=USERAGENT,[\"host\"]=reqt.host,[\"connection\"]=\"close, TE\",[\"te\"]=\"trailers\"}if reqt.user and reqt.password then lower[\"authorization\"]=\"Basic \"..(mime.b64(reqt.user..\":\"..reqt.password))end for i,v in base.pairs(reqt.headers or lower)do lower[string.lower(i)]=v end return lower end local default={host=\"\",port=PORT,path=\"/\",scheme=\"http\"}local functio"
	"n adjustrequest(reqt)local nreqt=reqt.url and url.parse(reqt.url,default)or{}for i,v in base.pairs(reqt)do nreqt[i]=v end if nreqt.port==\"\"then nreqt.port=80 end socket.try(nreqt.host and nreqt.host~=\"\",\"invalid host '\"..base.tostring(nreqt.host)..\"'\")nreqt.uri=reqt.uri or adjusturi(nreqt)nreqt.host,nreqt.port=adjustproxy(nreqt)nreqt.headers=adjustheaders(nreqt)return nreqt end local function shouldredirect(reqt,code,headers)return headers.location and string.gsub(headers.location,\"%s\",\"\")~=\""
	"\"and(reqt.redirect~=false)and(code==301 or code==302 or code==303 or code==307)and(not reqt.method or reqt.method==\"GET\"or reqt.method==\"HEAD\")and(not reqt.nredirects or reqt.nredirects<5)end local function shouldreceivebody(reqt,code)if reqt.method==\"HEAD\"then return nil end if code==204 or code==304 then return nil end if code>=100 and code<200 then return nil end return 1 end local trequest,tredirect function tredirect(reqt,location)local result,code,headers,status=trequest{url=url.absolute(reqt"
	".url,location),source=reqt.source,sink=reqt.sink,headers=reqt.headers,proxy=reqt.proxy,nredirects=(reqt.nredirects or 0)+1,create=reqt.create}headers=headers or{}headers.location=headers.location or location return result,code,headers,status end function trequest(reqt)local nreqt=adjustrequest(reqt)local h=open(nreqt.host,nreqt.port,nreqt.create)h:sendrequestline(nreqt.method,nreqt.uri)h:sendheaders(nreqt.headers)if nreqt.source then h:sendbody(nreqt.headers,nreqt.source,nreqt.step)end local code,status=h"
	":receivestatusline()if not code then h:receive09body(status,nreqt.sink,nreqt.step)return 1,200 end local headers while code==100 do headers=h:receiveheaders()code,status=h:receivestatusline()end headers=h:receiveheaders()if shouldredirect(nreqt,code,headers)and not nreqt.source then h:close()return tredirect(reqt,headers.location)end if shouldreceivebody(nreqt,code)then h:receivebody(headers,nreqt.sink,nreqt.step)end h:close()return 1,code,headers,status end local function srequest(u,b)local t={}local req"
	"t={url=u,sink=ltn12.sink.table(t)}if b then reqt.source=ltn12.source.string(b)reqt.headers={[\"content-length\"]=string.len(b),[\"content-type\"]=\"application/x-www-form-urlencoded\"}reqt.method=\"POST\"end local code,headers,status=socket.skip(1,trequest(reqt))return table.concat(t),code,headers,status end request=socket.protect(function(reqt,body)if base.type(reqt)==\"string\"then return srequest(reqt,body)else return trequest(reqt)end end)",

	/* smtp.lua */
	"local base=_G local coroutine=require(\"coroutine\")local string=require(\"string\")local math=require(\"math\")local os=require(\"os\")local socket=require(\"socket\")local tp=require(\"socket.tp\")local ltn12=require(\"ltn12\")local headers=require(\"socket.headers\")local mime=require(\"mime\")module(\"socket.smtp\")TIMEOUT=60 SERVER=\"localhost\"PORT=25 DOMAIN=os.getenv(\"SERVER_NAME\")or\"localhost\"ZONE=\"-0000\"local metat={__index={}}function metat.__index:greet(domain)self.try(self.tp:check(\"2.."
	"\"))self.try(self.tp:command(\"EHLO\",domain or DOMAIN))return socket.skip(1,self.try(self.tp:check(\"2..\")))end function metat.__index:mail(from)self.try(self.tp:command(\"MAIL\",\"FROM:\"..from))return self.try(self.tp:check(\"2..\"))end function metat.__index:rcpt(to)self.try(self.tp:command(\"RCPT\",\"TO:\"..to))return self.try(self.tp:check(\"2..\"))end function metat.__index:data(src,step)self.try(self.tp:command(\"DATA\"))self.try(self.tp:check(\"3..\"))self.try(self.tp:source(src,step))self.try(s"
	"elf.tp:send(\"\\r\\n.\\r\\n\"))return self.try(self.tp:check(\"2..\"))end function metat.__index:quit()self.try(self.tp:command(\"QUIT\"))return self.try(self.tp:check(\"2..\"))end function metat.__index:close()return self.tp:close()end function metat.__index:login(user,password)self.try(self.tp:command(\"AUTH\",\"LOGIN\"))self.try(self.tp:check(\"3..\"))self.try(self.tp:command(mime.b64(user)))self.try(self.tp:check(\"3..\"))self.try(self.tp:command(mime.b64(password)))return self.try(self.tp:check(\"2.."
	"\"))end function metat.__index:plain(user,password)local auth=\"PLAIN \"..mime.b64(\"\\000\"..user..\"\\000\"..password)self.try(self.tp:command(\"AUTH\",auth))return self.try(self.tp:check(\"2..\"))end function metat.__index:auth(user,password,ext)if not user or not password then return 1 end if string.find(ext,\"AUTH[^\\n]+LOGIN\")then return self:login(user,password)elseif string.find(ext,\"AUTH[^\\n]+PLAIN\")then return self:plain(user,password)else self.try(nil,\"authentication not supported\")end en"
	"d function metat.__index:send(mailt)self:mail(mailt.from)if base.type(mailt.rcpt)==\"table\"then for i,v in base.ipairs(mailt.rcpt)do self:rcpt(v)end else self:rcpt(mailt.rcpt)end self:data(ltn12.source.chain(mailt.source,mime.stuff()),mailt.step)end function open(server,port,create)local tp=socket.try(tp.connect(server or SERVER,port or PORT,TIMEOUT,create))local s=base.setmetatable({tp=tp},metat)s.try=socket.newtry(function()s:close()end)return s end local function lower_headers(headers)local lower={}fo"
	"r i,v in base.pairs(headers or lower)do lower[string.lower(i)]=v end return lower end local seqno=0 local function newboundary()seqno=seqno+1 return string.format(\"%s%05d==%05u\",os.date(\"%d%m%Y%H%M%S\"),math.random(0,99999),seqno)end local send_message local function send_headers(tosend)local canonic=headers.canonic local h=\"\\r\\n\"for f,v in base.pairs(tosend)do h=(canonic[f]or f)..\": \"..v..\"\\r\\n\"..h end coroutine.yield(h)end local function send_multipart(mesgt)local bd=newboundary()local head"
	"ers=lower_headers(mesgt.headers or{})headers[\"content-type\"]=headers[\"content-type\"]or\"multipart/mixed\"headers[\"content-type\"]=headers[\"content-type\"]..\"; boundary=\\\"\"..bd..\"\\\"\"send_headers(headers)if mesgt.body.preamble then coroutine.yield(mesgt.body.preamble)coroutine.yield(\"\\r\\n\")end for i,m in base.ipairs(mesgt.body)do coroutine.yield(\"\\r\\n--\"..bd..\"\\r\\n\")send_message(m)end coroutine.yield(\"\\r\\n--\"..bd..\"--\\r\\n\\r\\n\")if mesgt.body.epilogue then coroutine.yield(m"
	"esgt.body.epilogue)coroutine.yield(\"\\r\\n\")end end local function send_source(mesgt)local headers=lower_headers(mesgt.headers or{})headers[\"content-type\"]=headers[\"content-type\"]or\"text/plain; charset=\\\"iso-8859-1\\\"\"send_headers(headers)while true do local chunk,err=mesgt.body()if err then coroutine.yield(nil,err)elseif chunk then coroutine.yield(chunk)else break end end end local function send_string(mesgt)local headers=lower_headers(mesgt.headers or{})headers[\"content-type\"]=headers[\"con"
	"tent-type\"]or\"text/plain; charset=\\\"iso-8859-1\\\"\"send_headers(headers)coroutine.yield(mesgt.body)end function send_message(mesgt)if base.type(mesgt.body)==\"table\"then send_multipart(mesgt)elseif base.type(mesgt.body)==\"function\"then send_source(mesgt)else send_string(mesgt)end end local function adjust_headers(mesgt)local lower=lower_headers(mesgt.headers)lower[\"date\"]=lower[\"date\"]or os.date(\"!%a, %d %b %Y %H:%M:%S \")..(mesgt.zone or ZONE)lower[\"x-mailer\"]=lower[\"x-mailer\"]or socket."
	"_VERSION lower[\"mime-version\"]=\"1.0\"return lower end function message(mesgt)mesgt.headers=adjust_headers(mesgt)local co=coroutine.create(function()send_message(mesgt)end)return function()local ret,a,b=coroutine.resume(co)if ret then return a,b else return nil,a end end end send=socket.protect(function(mailt)local s=open(mailt.server,mailt.port,mailt.create)local ext=s:greet(mailt.domain)s:auth(mailt.user,mailt.password,ext)s:send(mailt)s:quit()return s:close()end)",

	/* tp.lua */
	"local base=_G local string=require(\"string\")local socket=require(\"socket\")local ltn12=require(\"ltn12\")module(\"socket.tp\")TIMEOUT=60 local function get_reply(c)local code,current,sep local line,err=c:receive()local reply=line if err then return nil,err end code,sep=socket.skip(2,string.find(line,\"^(%d%d%d)(.?)\"))if not code then return nil,\"invalid server reply\"end if sep==\"-\"then repeat line,err=c:receive()if err then return nil,err end current,sep=socket.skip(2,string.find(line,\"^(%d%d%d)("
	".?)\"))reply=reply..\"\\n\"..line until code==current and sep==\" \"end return code,reply end local metat={__index={}}function metat.__index:check(ok)local code,reply=get_reply(self.c)if not code then return nil,reply end if base.type(ok)~=\"function\"then if base.type(ok)==\"table\"then for i,v in base.ipairs(ok)do if string.find(code,v)then return base.tonumber(code),reply end end return nil,reply else if string.find(code,ok)then return base.tonumber(code),reply else return nil,reply end end else return"
	" ok(base.tonumber(code),reply)end end function metat.__index:command(cmd,arg)cmd=string.upper(cmd)if arg then return self.c:send(cmd..\" \"..arg..\"\\r\\n\")else return self.c:send(cmd..\"\\r\\n\")end end function metat.__index:sink(snk,pat)local chunk,err=c:receive(pat)return snk(chunk,err)end function metat.__index:send(data)return self.c:send(data)end function metat.__index:receive(pat)return self.c:receive(pat)end function metat.__index:getfd()return self.c:getfd()end function metat.__index:dirty()ret"
	"urn self.c:dirty()end function metat.__index:getcontrol()return self.c end function metat.__index:source(source,step)local sink=socket.sink(\"keep-open\",self.c)local ret,err=ltn12.pump.all(source,sink,step or ltn12.pump.step)return ret,err end function metat.__index:close()self.c:close()return 1 end function connect(host,port,timeout,create)local c,e=(create or socket.tcp)()if not c then return nil,e end c:settimeout(timeout or TIMEOUT)local r,e=c:connect(host,port)if not r then c:close()return nil,e end"
	" return base.setmetatable({c=c},metat)end",

	/* url.lua */
	"local string=require(\"string\")local base=_G local table=require(\"table\")module(\"socket.url\")_VERSION=\"URL 1.0.1\"function escape(s)return(string.gsub(s,\"([^A-Za-z0-9_])\",function(c)return string.format(\"%%%02x\",string.byte(c))end))end local function make_set(t)local s={}for i,v in base.ipairs(t)do s[t[i]]=1 end return s end local segment_set=make_set{\"-\",\"_\",\".\",\"!\",\"~\",\"*\",\"'\",\"(\",\")\",\":\",\"@\",\"&\",\"=\",\"+\",\"$\",\",\",}local function protect_segment(s)return string.gs"
	"ub(s,\"([^A-Za-z0-9_])\",function(c)if segment_set[c]then return c else return string.format(\"%%%02x\",string.byte(c))end end)end function unescape(s)return(string.gsub(s,\"%%(%x%x)\",function(hex)return string.char(base.tonumber(hex,16))end))end local function absolute_path(base_path,relative_path)if string.sub(relative_path,1,1)==\"/\"then return relative_path end local path=string.gsub(base_path,\"[^/]*$\",\"\")path=path..relative_path path=string.gsub(path,\"([^/]*%./)\",function(s)if s~=\"./\"then r"
	"eturn s else return\"\"end end)path=string.gsub(path,\"/%.$\",\"/\")local reduced while reduced~=path do reduced=path path=string.gsub(reduced,\"([^/]*/%.%./)\",function(s)if s~=\"../../\"then return\"\"else return s end end)end path=string.gsub(reduced,\"([^/]*/%.%.)$\",function(s)if s~=\"../..\"then return\"\"else return s end end)return path end function parse(url,default)local parsed={}for i,v in base.pairs(default or parsed)do parsed[i]=v end if not url or url==\"\"then return nil,\"invalid url\"end "
	"url=string.gsub(url,\"#(.*)$\",function(f)parsed.fragment=f return\"\"end)url=string.gsub(url,\"^([%w][%w%+%-%.]*)%:\",function(s)parsed.scheme=s;return\"\"end)url=string.gsub(url,\"^//([^/]*)\",function(n)parsed.authority=n return\"\"end)url=string.gsub(url,\"%?(.*)\",function(q)parsed.query=q return\"\"end)url=string.gsub(url,\"%;(.*)\",function(p)parsed.params=p return\"\"end)if url~=\"\"then parsed.path=url end local authority=parsed.authority if not authority then return parsed end authority=string.g"
	"sub(authority,\"^([^@]*)@\",function(u)parsed.userinfo=u;return\"\"end)authority=string.gsub(authority,\":([^:]*)$\",function(p)parsed.port=p;return\"\"end)if authority~=\"\"then parsed.host=authority end local userinfo=parsed.userinfo if not userinfo then return parsed end userinfo=string.gsub(userinfo,\":([^:]*)$\",function(p)parsed.password=p;return\"\"end)parsed.user=userinfo return parsed end function build(parsed)local ppath=parse_path(parsed.path or\"\")local url=build_path(ppath)if parsed.params t"
	"hen url=url..\";\"..parsed.params end if parsed.query then url=url..\"?\"..parsed.query end local authority=parsed.authority if parsed.host then authority=parsed.host if parsed.port then authority=authority..\":\"..parsed.port end local userinfo=parsed.userinfo if parsed.user then userinfo=parsed.user if parsed.password then userinfo=userinfo..\":\"..parsed.password end end if userinfo then authority=userinfo..\"@\"..authority end end if authority then url=\"//\"..authority..url end if parsed.scheme then "
	"url=parsed.scheme..\":\"..url end if parsed.fragment then url=url..\"#\"..parsed.fragment end return url end function absolute(base_url,relative_url)if base.type(base_url)==\"table\"then base_parsed=base_url base_url=build(base_parsed)else base_parsed=parse(base_url)end local relative_parsed=parse(relative_url)if not base_parsed then return relative_url elseif not relative_parsed then return base_url elseif relative_parsed.scheme then return relative_url else relative_parsed.scheme=base_parsed.scheme if n"
	"ot relative_parsed.authority then relative_parsed.authority=base_parsed.authority if not relative_parsed.path then relative_parsed.path=base_parsed.path if not relative_parsed.params then relative_parsed.params=base_parsed.params if not relative_parsed.query then relative_parsed.query=base_parsed.query end end else relative_parsed.path=absolute_path(base_parsed.path or\"\",relative_parsed.path)end end return build(relative_parsed)end end function parse_path(path)local parsed={}path=path or\"\"string.gsub("
	"path,\"([^/]+)\",function(s)table.insert(parsed,s)end)for i=1,table.getn(parsed)do parsed[i]=unescape(parsed[i])end if string.sub(path,1,1)==\"/\"then parsed.is_absolute=1 end if string.sub(path,-1,-1)==\"/\"then parsed.is_directory=1 end return parsed end function build_path(parsed,unsafe)local path=\"\"local n=table.getn(parsed)if unsafe then for i=1,n-1 do path=path..parsed[i]path=path..\"/\"end if n>0 then path=path..parsed[n]if parsed.is_directory then path=path..\"/\"end end else for i=1,n-1 do path"
	"=path..protect_segment(parsed[i])path=path..\"/\"end if n>0 then path=path..protect_segment(parsed[n])if parsed.is_directory then path=path..\"/\"end end end if parsed.is_absolute then path=\"/\"..path end return path end",
	
};
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
		lua_pushliteral(L,"LUA_ERRSYNTAX");
	else if(res == LUA_ERRMEM)
		lua_pushliteral(L,"LUA_ERRMEM");	
	else if(res == LUA_ERRGCMM)
		lua_pushliteral(L,"LUA_ERRGCMM");
	
	lua_pushfstring(L,"ERROR: %s when loading %s :\n\t%s\n",lua_tostring(L,-1),name,lua_tostring(L,-2));
	return lua_error(L);
}

static int luasocket_socket_loader(lua_State* L)
{
	return load_and_run_buffer(L,0,"embedded socket.lua");
}
static int luasocket_mime_loader(lua_State* L)
{
	return load_and_run_buffer(L,1,"embedded mime.lua");
}
static int luasocket_ltn12_loader(lua_State* L)
{
	return load_and_run_buffer(L,2,"embedded ltn12.lua");
}
static int luasocket_ftp_loader(lua_State* L)
{
	return load_and_run_buffer(L,3,"embedded ftp.lua");
}
static int luasocket_headers_loader(lua_State* L)
{
	return load_and_run_buffer(L,4,"embedded headers.lua");
}
static int luasocket_http_loader(lua_State* L)
{
	return load_and_run_buffer(L,5,"embedded http.lua");
}
static int luasocket_smtp_loader(lua_State* L)
{
	return load_and_run_buffer(L,6,"embedded smtp.lua");
}
static int luasocket_tp_loader(lua_State* L)
{
	return load_and_run_buffer(L,7,"embedded tp.lua");
}
static int luasocket_url_loader(lua_State* L)
{
	return load_and_run_buffer(L,8,"embedded url.lua");
}
static luaL_Reg embedded_modules[] =
{
	{"socket",luasocket_socket_loader}
	,{"mime",luasocket_mime_loader}
	,{"ltn12",luasocket_ltn12_loader}
	,{"socket.ftp",luasocket_ftp_loader}
	,{"socket.headers",luasocket_headers_loader}
	,{"socket.http",luasocket_http_loader}
	,{"socket.smtp",luasocket_smtp_loader}
	,{"socket.tp",luasocket_tp_loader}
	,{"socket.url",luasocket_url_loader}
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


