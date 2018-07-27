function readconfig()
file.open("config.txt","r+")
local config={}
repeat
	local str=file.readline()
	--print(str)
	if (str~=nil) then
		for k, v in string.gmatch(str, "(%w+)=(.*)\n") do
		config[k]=v
		--print(k, v)
		print(string.len(v))
		print(string.byte(v,4))
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
file.write("username="..c.username.."\n")
file.write("useraddr="..c.useraddr.."\n")
file.close()

end

print(node.chipid())
print(node.flashid())
print(wifi.ap.getmac())
print(wifi.sta.getmac())
--uart.setup(0,115200,8,0,1,0)
c=readconfig()
ok, s = pcall(cjson.encode, c)
if ok then
    print(s)
  else
    print("failed to encode!")
 end
d=cjson.decode(s)
for k,v in pairs(d) do
	print(k,v)
end

