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
--print(str)
local i=1
config={}
repeat
local j=string.find(str,"&",i)
if (j==nil) then
    local s=string.sub(str,i)
    --print(s)
    if (s~=nil) then
        for k, v in string.gmatch(s, "(%w+)=(.*)") do
        config[k]=v
--      print(k, v)
        end
    end
    break
end
s=string.sub(str,i,j-1)
--print(s)
if (s~=nil) then
    for k, v in string.gmatch(s, "(%w+)=(.*)") do
    config[k]=v
--  print(k, v)
    end
end

i=j+1

until (s==nil)

for k,v in pairs(config)
do
--  print (k,v)
end
return config
end

