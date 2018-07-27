function calc(temp)
if (temp==nil) then
	return 0
end
local i=0
if(temp<=57) then
	i=temp-48
else
	i=temp-55
end
return i
end

function utf82unicode(s)
--s="%E4%B8%AD"
--4e2d
--0100 111000 101101
--1110 0100 10 111000 10 1011101
if (s==nil) then
	return nil
end

local a=string.byte(s,2)
local b=string.byte(s,3)
local c=string.byte(s,5)
local d=string.byte(s,6)
local e=string.byte(s,8)
local f=string.byte(s,9)
--print(a,b,c,d,e,f)
a=calc(a)
b=calc(b)
c=calc(c)
d=calc(d)
e=calc(e)
f=calc(f)
--print(a,b,c,d,e,f)
a=a*16+b
c=c*16+d
e=e*16+f
--print(a,c,e)
c=c%64
e=e%64
--print(b,c,e)
a=b*4096+c*64+e
--print(a)
b=a%16
e=a/(16*16*16)
e=math.floor(e)
a=a-e*(16*16*16)
d=a/(16*16)
d=math.floor(d)
a=a-d*(16*16)
c=a/16
c=math.floor(c)
--print(b,c,d,e)
local code={"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"}
s=code[e+1]..code[d+1]..code[c+1]..code[b+1]
--print(s)
return "&#x"..s
end
function ana(str)
--str="a=abc&b=AB%12%2e%34&c=%23%25%34AB&d=182.35.1.57"
--str=string.gsub(str,"&","\n")
print(str)
local i=1
config={}
repeat
local j=string.find(str,"&",i)
if (j==nil) then
	local s=string.sub(str,i)
	print(s)
	if (s~=nil) then
		for k, v in string.gmatch(s, "(%w+)=(.*)") do
		config[k]=v
--		print(k, v)
		end
	end
	break
end
s=string.sub(str,i,j-1)
--print(s)
if (s~=nil) then
	for k, v in string.gmatch(s, "(%w+)=(.*)") do
	config[k]=v
--	print(k, v)
	end
end

i=j+1

until (s==nil)

for k,v in pairs(config)
do
--	print (k,v)
end
return config
end

function readconfig()
file.open("config.txt","r+")
local config={}
repeat
	local str=file.readline()
	
	if (str~=nil) then
		for k, v in string.gmatch(str, "(%w+)=(.*)\n") do
		config[k]=v
		--print(string.len(v))
		--print(k, v)
		end
	end
until (str==nil)
file.close()
print("config:")
for k, v in pairs(config) do
    print(k, v)
end
return config
end

function saveconfig(c)
file.open("config.txt","w+")
file.seek("set",0)
for k,v in pairs(c) do
	file.write(k.."="..v.."\n")
end
file.close()

end

--System init
files=file.list()
if(files["config.txt"]) then
	config=readconfig()
	flag=1
else
	flag=0
	username=""
	useraddr=""
	print("No config")
end
cfgname={"&#x7528&#x6237&#x59D3&#x540D","&#x7528&#x6237&#x5730&#x5740","&#x7528&#x6237&#x7535&#x8BDD",
		"&#x7528&#x6237ID","MAC","&#x7F51&#x7EDC&#x7C7B&#x578B","&#x65E0&#x7EBF&#x7F51&#x7EDCSSID",
		"&#x65E0&#x7EBF&#x7F51&#x7EDCPassword","&#x670D&#x52A1&#x5668&#x5730&#x5740",
		"&#x670D&#x52A1&#x5668&#x7AEF&#x53E3"}
cfg={}
cfg.ssid="myssid"
cfg.pwd="2113201jjw"
wifi.setmode(wifi.STATIONAP)
wifi.ap.config(cfg)
wifi.sta.config("jjw","2113201jjw")
print(wifi.sta.getip())
--print(wifi.ap.getip())
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive", function(client,request)
        local buf = "";
        print("rec-"..request)
        local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
        if(method == nil)then
            _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
        end
        local _GET = {}
        if (vars ~= nil)then
	        print("vars"..vars)
	        args= ana(vars)
			for v,k in pairs(args) do
				print(v,k)
			end				
	        --s=utf82unicode(args.username)
	        --t=utf82unicode(args.useraddr)
--            for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
--                _GET[k] = v
--                print(k.."-"..v)
--            end
			if(args ~=nil) then
	        	print("save from html form Config")
	        	--print(string.len(args.username))
	        	saveconfig(args)
				config=readconfig()
				--print(string.len(config.username))
				flag=1
	        end

        end
        --print(_GET.text)
        buf=buf.."<meta  http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">"
        buf = buf.."<h1> &#x5BB6&#x7528&#x73AF&#x5883&#x76D1&#x6D4B&#x4EEA&#x7CFB&#x7EDF&#x914D&#x7F6E</h1>";
        if(flag==1) then
        	buf=buf..cfgname[1]..":"..config.username.."<br>"
        	buf=buf..cfgname[2]..":"..config.useraddr.."<br>"
        	buf=buf..cfgname[3]..":"..config.userphone.."<br>"
        	buf=buf..cfgname[4]..":"..config.id.."<br>"
        	buf=buf..cfgname[5]..":"..config.mac.."<br>"
        	buf=buf..cfgname[6]..":"..config.nettype.."<br>"
        	buf=buf..cfgname[7]..":"..config.apssid.."<br>"
        	buf=buf..cfgname[8]..":"..config.appassword.."<br>"
        	buf=buf..cfgname[9]..":"..config.server.."<br>"
        	buf=buf..cfgname[10]..":"..config.port.."<br>"
        	
        end
--        buf = buf.."<p>%E4%B8%AD%E5%9B%BDGPIO0 <a href=\"?pin=ON1\"><button>ON</button></a>&nbsp;<a href=\"?pin=OFF1\"><button>OFF</button></a></p>";
        --s="&#xd6d0"
        --t="&#xe4b8ad"
--        buf = buf.."<p>GPIO2 <a href=\"?pin=ON2\"><button>ON</button></a>&nbsp;<a href=\"?pin=OFF2\"><button>OFF</button></a></p>"..s..t.."&#x4E2D";
        buf = buf..[[
        <form method="get">
		&#x7528&#x6237&#x59D3&#x540D:<input type="text" name="username" value=""/><br>
		&#x7528&#x6237&#x5730&#x5740:<input type="text" name="useraddr" value=""/><br>
		&#x7528&#x6237&#x7535&#x8BDD:<input type="text" name="userphone" value=""/><br>
		&#x7528&#x6237ID:<input type="text" name="id" value=""/><br>
		MAC:<input type="text" name="mac" value=""/><br>
		&#x7F51&#x7EDC&#x7C7B&#x578B:<input type="text" name="nettype" value=""/><br>
		&#x65E0&#x7EBF&#x7F51&#x7EDCSSID:<input type="text" name="apssid" value=""/><br>
		&#x65E0&#x7EBF&#x7F51&#x7EDCPassword:<input type="text" name="appassword" value=""/><br>
		&#x670D&#x52A1&#x5668&#x5730&#x5740:<input type="text" name="server" value=""/><br>
		&#x670D&#x52A1&#x5668&#x7AEF&#x53E3:<input type="text" name="port" value=""/><br>
		<input type="submit" value="&#x4FDD&#x5B58"/>
		</form>
        ]]
        client:send(buf);
        client:close();
        collectgarbage();
    end)
end)
print("server working")
