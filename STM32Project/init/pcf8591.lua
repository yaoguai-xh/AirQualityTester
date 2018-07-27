    id=0
    sda=4
    scl=3

    -- initialize i2c, set pin1 as sda, set pin0 as scl
    i2c.setup(id,sda,scl,i2c.SLOW)

    -- user defined function: read from reg_addr content of dev_addr
    function read_reg(dev_addr, reg_addr)
      i2c.start(id)
      i2c.address(id, dev_addr ,i2c.TRANSMITTER)
      i2c.write(id,reg_addr)
      i2c.stop(id)
      i2c.start(id)
      i2c.address(id, dev_addr,i2c.RECEIVER)
      c=i2c.read(id,2)
      i2c.stop(id)
      return c
    end

    function write_reg(dev_addr, reg_addr, reg_val)
      i2c.start(id)
      i2c.address(id, dev_addr, i2c.TRANSMITTER)
      i2c.write(id, reg_addr)
      i2c.write(id, reg_val)
      i2c.stop(id)
    end

    -- address of adc PCF8591
    device=0x48 --72 decimal

    for i=0,100 do
    befor=tmr.now()
      result = read_reg(device, 0x00) 
after=tmr.now()
print(after-befor)
   print("Result: " .. string.byte(result, 1) .. " " .. string.byte(result, 2))
      tmr.delay(250000)
      tmr.wdclr()
    end
