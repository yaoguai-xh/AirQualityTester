dofile("fun.lua")
dofile("config.lua")
dofile("sendserver.lua")
--dofile("trans.lua")
--dofile("")
--System init
--uart.setup(0,115200,8,0,1,0)
files=file.list()
if(files["config.txt"]) then
    sendconfig()
    flag=1
else
	flag=0
	username=""
	useraddr=""
	print("No config")
end

wifi.setmode(wifi.STATIONAP)

--print(wifi.ap.getip())
tmr.delay(5000000)

status = xpcall( webconfig, myerrorhandler )
print( status)
if (config.nettype=="wifi") then
    print("Wifi Type")
    tmr.alarm(1,5000,1,function()
        if (flag==1) then
            tmr.stop(1)
            status = xpcall( connserver, myerrorhandler )
            print( status)
        else
            
        end
    end)
end
print("server working")
