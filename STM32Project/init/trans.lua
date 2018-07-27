dofile("fun.lua")
str="abd%E4%B8%ADse123%E5%B9%A3sd123"
 i,j,k,m,n = string.find(str,"%%(%w%w)%%(%w%w)%%(%w%w)%w*")
   print(i,j,k,m,n)
s=string.gsub(str,"(%%%w%w%%%w%w%%%w%w)","||")
print(s)
s=string.gsub(str,"(%%%w%w%%%w%w%%%w%w)",utf82unicode)
print(s)
--utf82unicode

str="123.23.34.45"
config={}
ip={}
config["server"]=str
for i,j,m,n in string.gmatch(str, "(%d+).(%d+).(%d+).(%d+)") do
        ip[1]=i
		ip[2]=j
		ip[3]=m
		ip[4]=n
        --print(string.len(v))
        print(i,j,m,n)
		config["ip"]=ip
end

for k,v in pairs(ip) do
	print(k,v)
end

for k,v in pairs(config) do
	print(k,v)
end
--s=cjson.encode(config)
ok,s=pcall(cjson.encode, config)
print(s)

