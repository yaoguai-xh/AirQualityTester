function sendserver(port,server)
            collectgarbage("collect")
            cu = net.createConnection(net.TCP)
            cu:connect(port,server)
            serveconn=1
            --connect end
            tmr.alarm(3, 40000, 0, function()
                    serveconn=0
               end)
            --keep online
            tmr.alarm(2, 20000, 1, function()
                --print("local:"..s1)
                    cu:send( "\n" )
                    print("{[ok]")
               end)
            --keep online end
            
            --receive message from bigiot
            cu:on("receive", function(cu, c) 
                print(c)
                --uart.write(0, c)
            end)
            --receive end
            --receive message from bigiot
            cu:on("disconnection", function(cu) 
                --uart.write(0, "disconnect")
                cu:close()
                print("{[di]")
                tmr.stop(2)
                tmr.stop(3)
                serveconn=0
                servedisconn=servedisconn+1
            end)
            --receive end
            
            --receive message from bigiot
            cu:on("connection", function(cu) 
                --uart.write(0, "connection")
                tmr.stop(3)
                print("{[ok]")
            end)
            --receive end
            
            --receive message from bigiot
            cu:on("reconnection", function(cu) 
                uart.write(0, "reconnection")
            end)
            --receive end
            
            --receive message from uart
            uart.on("data", "\n",
              function(data)
                cu:send( data )
            end, 0)
end

function connserver()
--wifi.sta.config("jjw","2113201jjw")
wifi.sta.config(config.apssid,config.appassword)
--wifi.sta.connect()
wifi.sta.autoconnect(1)
conn=0
serveconn=0
servedisconn=0
tmr.alarm(1,5000,1,function()
    if(wifi.sta.getip()==nil) and (conn<30) then
        print("IP un,Watiing...")
        conn=conn+1
    else
        
        if(conn<30) then
            if(serveconn==0) then
                print(wifi.sta.getip())
                sendserver(config.port,config.server)
            end
            conn=0
            if(servedisconn>10) then   
                print("servedisonn REset Node jjw")
                node.restart()
            end
        else
            print("REset Node jjw")
            node.restart()
            print("sta.disconn")
            wifi.sta.disconnect()
            tmr.delay(1000000)
            --wifi.sta.connect()
            wifi.sta.autoconnect(1)
            conn=0
        end
    end
end)

end
--receive end
