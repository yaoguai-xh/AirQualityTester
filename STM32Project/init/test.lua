befor=tmr.now()
result={}
for i=0,127 do
    result[i]=adc.read(0)
end
after=tmr.now()

for i=0,127 do
    print(result[i]..",")
end

print(after-befor)
