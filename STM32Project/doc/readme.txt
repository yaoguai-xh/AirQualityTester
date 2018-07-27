使用编译环境：MDK4.72A  IAR6.30
使用硬件环境：STM32F103RBT6
使用函数库版本：STM32F10x_StdPeriph_Driver V3.50
工程概要：  完成是MCU通过SPI2对w5500的读写操作，完成对IP,mac，gateway等的操作，
			内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突
            如果是用网线跟PC机直连，请设置PC机本地连接地址IP为静态IP
调试说明：此例程适用的调试下载器是J-link-ob，也可以用过串口下载调试程序,（请将串口线插在板子J1口
          上,并打开超级终端或串口助手，配置波特率115200，8位，一个停止位，无校验位。
淘宝购买：  http://vcc-gnd.taobao.com
源地工作室：http://www.vcc-gnd.com