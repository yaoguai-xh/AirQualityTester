cfgname={"&#x7528&#x6237&#x59D3&#x540D","&#x7528&#x6237&#x5730&#x5740","&#x7528&#x6237&#x7535&#x8BDD",
        "&#x7528&#x6237ID","MAC","&#x7F51&#x7EDC&#x7C7B&#x578B","&#x65E0&#x7EBF&#x7F51&#x7EDCSSID",
        "&#x65E0&#x7EBF&#x7F51&#x7EDCPassword","&#x670D&#x52A1&#x5668&#x5730&#x5740",
        "&#x670D&#x52A1&#x5668&#x7AEF&#x53E3","&#x7CFB&#x7EDF&#x65F6&#x95F4"}
function sendconfig()
    config=readconfig()
    if(config~=nil) then
         ip={}
         for i,j,m,n in string.gmatch(config.server, "(%d+).(%d+).(%d+).(%d+)") do
             ip[1]=i
             ip[2]=j
             ip[3]=m
             ip[4]=n
         end
         config["ip"]=ip
        ok, s = pcall(cjson.encode, config)
        if ok then
            print("5aa5"..string.len(s)..s)
        else
            print("failed to encode!")
        end
     end

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

function myerrorhandler( err )
   print( "ERROR:", err )
end


function webconfig()
cfg={}
cfg.ssid="System Config"
cfg.pwd="147258369"
wifi.ap.config(cfg)
--ok, s = pcall(cjson.encode, config)
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive", function(client,request)
        local buf = "";
        --print("rec-"..request)
        local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
        if(method == nil)then
            _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
        end
        if (path~=nil) then
            print(path)
            if (path=="/clear") then
                flag=0
            end
            if (path=="/save") then
                sendconfig()
            end
            if (path=="/reset") then
                node.restart()
            end
        end
        local _GET = {}
        if (vars ~= nil)then
            --print("vars--"..vars)
            args= ana(vars)
            for v,k in pairs(args) do
                --print(v,k)
            end             
            --s=utf82unicode(args.username)
            --t=utf82unicode(args.useraddr)
--            for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
--                _GET[k] = v
--                print(k.."-"..v)
--            end
            if(args ~=nil) then
                --print("save from html form Config")
                --print(string.len(args.username))
                config.username=string.gsub(config.username,"(%%%w%w%%%w%w%%%w%w)",utf82unicode)
                config.useraddr=string.gsub(config.useraddr,"(%%%w%w%%%w%w%%%w%w)",utf82unicode)
                saveconfig(args)
                config=args
                --print(string.len(config.username))
                ip={}
                for i,j,m,n in string.gmatch(config.server, "(%d+).(%d+).(%d+).(%d+)") do
                        ip[1]=i
                        ip[2]=j
                        ip[3]=m
                        ip[4]=n
                        --print(string.len(v))
                        --print(i,j,m,n)
                        config["ip"]=ip
                end
                ok,s=pcall(cjson.encode, config)
                print("5aa5"..string.len(s)..s)
                flag=1
            end

        end
        --print(_GET.text)
        if(flag==1) then
            buf=buf.."<meta  http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">"
            buf = buf.."<h1> &#x5BB6&#x7528&#x73AF&#x5883&#x76D1&#x6D4B&#x4EEA&#x7CFB&#x7EDF&#x914D&#x7F6E</h1>"
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
--            buf=buf..cfgname[11]..":"..config.port.."<br>"
            buf=buf.."<a href=\"/clear\"><button>CLEAR</button></a>"
            buf=buf.."<a href=\"/save\"><button>SAVE</button></a>"
            buf=buf.."<a href=\"/reset\"><button>RESET</button></a>"
          --buf=buf..config.username.."<br>"
--            buf=buf..config.useraddr.."<br>"
--            buf=buf..config.userphone.."<br>"
--            buf=buf..config.id.."<br>"
--            buf=buf..config.mac.."<br>"
--            buf=buf..config.nettype.."<br>"
--            buf=buf..config.apssid.."<br>"
--            buf=buf..config.appassword.."<br>"
--            buf=buf..config.server.."<br>"
--            buf=buf..config.port.."<br>"
--            buf = buf..[[
--            <form method="get">
--            &#x7528&#x6237&#x59D3&#x540D:<input type="text" name="username" value="]]..config.username..[["/><br>
--            &#x7528&#x6237&#x5730&#x5740:<input type="text" name="useraddr" value="]]..config.useraddr..[["/><br>
--            &#x7528&#x6237&#x7535&#x8BDD:<input type="text" name="userphone" value="]]..config.userphone..[["/><br>
--            &#x7528&#x6237ID:<input type="text" name="id" value="]]..config.id..[["/><br>
--            MAC:<input type="text" name="mac" value="]]..config.mac..[["/><br>
--            &#x7F51&#x7EDC&#x7C7B&#x578B:<input type="text" name="nettype" value="]]..config.nettype..[["/><br>
--            &#x65E0&#x7EBF&#x7F51&#x7EDCSSID:<input type="text" name="apssid" value="]]..config.apssid..[["/><br>
--            &#x65E0&#x7EBF&#x7F51&#x7EDCPassword:<input type="text" name="appassword" value="]]..config.appassword..[["/><br>
--            &#x670D&#x52A1&#x5668&#x5730&#x5740:<input type="text" name="server" value="]]..config.server..[["/><br>
--            &#x670D&#x52A1&#x5668&#x7AEF&#x53E3:<input type="text" name="port" value="]]..config.port..[["/><br>
--            &#x7CFB&#x7EDF&#x65F6&#x95F4:<input type="text" name="hour" value="]]..config.hour..[["/>:<input type="text" name="minute" value="]]..config.minute..[["/><br>
--            <input type="submit" value="&#x4FDD&#x5B58" onclick="msg()"/>
--            </form>
--            ]]
        else
            buf=buf..[[
            <script language="javascript">
            <!--
            function msg(){
                alert("save OK")
            }
            //-->
            </script>
            ]]
            buf=buf.."<meta  http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">"
            buf = buf.."<h1> &#x5BB6&#x7528&#x73AF&#x5883&#x76D1&#x6D4B&#x4EEA&#x7CFB&#x7EDF&#x914D&#x7F6E</h1>"
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
        &#x7CFB&#x7EDF&#x65F6&#x95F4:<input type="text" name="hour" />:<input type="text" name="minute" /><br>
        <input type="submit" value="&#x4FDD&#x5B58"  onclick="msg()"/>
        </form>
        ]]
            
        end
        print(string.len(buf))
        client:send(buf)
        client:close()
        collectgarbage("collect")
    end)
end)

end

