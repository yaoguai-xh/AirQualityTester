/******************** (C) COPYRIGHT  源地工作室 ********************************
 * 文件名  ：i2c.c
 * 描述    ：初始化IIC及一些基本的操作 
 * 作者    ：zhuoyingxingyu
 * 淘宝    ：源地工作室http://vcc-gnd.taobao.com/
 * 论坛地址：极客园地-嵌入式开发论坛http://vcc-gnd.com/
 * 版本更新: 2015-11-11
 * 硬件连接: PB6-I2C1_SCL、PB7-I2C1_SDA
 * 调试方式：J-Link-OB
**********************************************************************************/

//头文件
#include "i2c1.h"
#include "usart.h"

 /**
  * @file   I2C_Configuration
  * @brief  EEPROM管脚配置
  * @param  无
  * @retval 无
  */
void I2C_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  /* Configure I2C2 pins: PB6->SCL and PB7->SDA */
  RCC_APB2PeriphClockCmd(EEPROM_I2C_SCL_GPIO_RCC|EEPROM_I2C_SDA_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);  
	GPIO_InitStructure.GPIO_Pin =  EEPROM_I2C_SCL_PIN ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(EEPROM_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  EEPROM_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(EEPROM_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}

 /**
  * @file   I2C_delay
  * @brief  延迟时间
  * @param  无
  * @retval 无
  */
static void I2C_delay(uint16_t  time_us)
{	
   uint16_t i,j;
  for( i=0;i<time_us;i++ )
  {
		for( j=0;j<10;j++ );//大约1us
  }
}
  
 /**
  * @file   I2C_Start
  * @brief  起始信号
  * @param  无
  * @retval 无
  */
static FunctionalState I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay(5);
	if(!SDA_read)return DISABLE;	/* SDA线为低电平则总线忙,退出 */
	SDA_L;
	I2C_delay(4);
	if(SDA_read) return DISABLE;	/* SDA线为高电平则总线出错,退出 */
	SDA_L;
	I2C_delay(4);
	return ENABLE;
}

 /**
  * @file   I2C_Stop
  * @brief  停止信号
  * @param  无
  * @retval 无
  */
static void I2C_Stop(void)
{
	SCL_L;
	I2C_delay(1);
	SDA_L;
	I2C_delay(1);
	SCL_H;
	I2C_delay(5);
	SDA_H;
	I2C_delay(5);
}

 /**
  * @file   I2C_Ack
  * @brief  应答信号
  * @param  无
  * @retval 无
  */
static void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay(1);
	SDA_L;
	I2C_delay(1);
	SCL_H;
	I2C_delay(5);
	SCL_L;
	I2C_delay(5);
}

 /**
  * @file   I2C_NoAck
  * @brief  无应答信号
  * @param  无
  * @retval 无
  */
static void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay(1);
	SDA_H;
	I2C_delay(1);
	SCL_H;
	I2C_delay(5);
	SCL_L;
	I2C_delay(5);
}

 /**
  * @file   I2C_WaitAck
  * @brief  等待Ack
  * @param  无
  * @retval 返回为:=1有ACK,=0无ACK
  */
static FunctionalState I2C_WaitAck(void) 	
{
	SCL_L;
	I2C_delay(1);
	SDA_H;			
	I2C_delay(1);
	SCL_H;
	I2C_delay(5);
	if(SDA_read)
	{
      SCL_L;
      return DISABLE;
	}
	SCL_L;
	return ENABLE;
}

 /**
  * @file   I2C_SendByte
  * @brief  数据从高位到低位
  * @param  - SendByte: 发送的数据
  * @retval 无
  */
static void I2C_SendByte(uint8_t SendByte) 
{
    uint8_t i=8;
    while(i--)
    {
			SCL_L;
			I2C_delay(5);
			if(SendByte&0x80)
			SDA_H;  
			else 
			SDA_L;   
			SendByte<<=1;
			I2C_delay(5);
			SCL_H;
			I2C_delay(10);
    }
    SCL_L;
}


 /**
  * @file   I2C_ReceiveByte
  * @brief  数据从高位到低位
  * @param  无
  * @retval I2C总线返回的数据
  */
static uint8_t I2C_ReceiveByte(void)  
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
			ReceiveByte<<=1;      
			SCL_L;
			I2C_delay(5);
			SCL_H;
			I2C_delay(5);	
			if(SDA_read)
			{
				ReceiveByte|=0x01;
			}
    }
    SCL_L;
    return ReceiveByte;
}
   
 /**
  * @file   I2C_WriteByte
  * @brief  写一字节数据
  * @param  
	*          - SendByte: 待写入数据
	*          - WriteAddress: 待写入地址
  * @retval 返回为:=1成功写入,=0失败
  */
FunctionalState I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress)
{
    if(!I2C_Start())
		{
			return DISABLE;
		}
    I2C_SendByte(EE_DEV_ADDR|I2C_WR); /*设置高起始地址+器件地址 */
    if(!I2C_WaitAck())
		{
			I2C_Stop(); 
			return DISABLE;
		}
    I2C_SendByte((uint8_t)(WriteAddress & 0x00FF));   /* 设置低起始地址 */      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
    return ENABLE;
}									 
/*
*********************************************************************************************************
*	函 数 名: ee_WriteBytes
*	功能说明: 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
*	形    参：_usAddress : 起始地址
*			      _usSize : 数据长度，单位为字节
*			      _pWriteBuf : 存放读到的数据的缓冲区指针
*	返 回 值: 0 表示失败，1表示成功
*********************************************************************************************************
*/
FunctionalState I2C_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t Addr;
	uint8_t pages;
	uint8_t bytes;
	Addr = _usAddress;
	pages=_usSize/8;//需要些eeprom的页数
	bytes=_usSize%8;//剩余不够一页的字节数
	for(m=0;m<pages;m++)
	{
		//printf("m=%d\r\n",m);
		
		if(!I2C_Start())
		{
			return DISABLE;
		}
		I2C_SendByte(EE_DEV_ADDR|I2C_WR); /*设置高起始地址+器件地址 */
		if(!I2C_WaitAck())
		{
			I2C_Stop(); 
			return DISABLE;
		}
		I2C_SendByte((uint8_t)(Addr & 0x00FF));   /* 设置低起始地址 */      
		I2C_WaitAck();
		for(i=0;i<8;i++)
		{
			I2C_SendByte(*_pWriteBuf++);
			//printf("_pWriteBuf2=%d\r\n",*_pWriteBuf++);
			I2C_WaitAck(); 
		}
		Addr=Addr+8;
		I2C_Stop(); 
		I2C_delay(10000);//等待数据写完
	}
	if(bytes!=0)
	{
		if(!I2C_Start())
		{
			return DISABLE;
		}
		I2C_SendByte(EE_DEV_ADDR|I2C_WR); /*设置高起始地址+器件地址 */
		if(!I2C_WaitAck())
		{
			I2C_Stop(); 
			return DISABLE;
		}
		I2C_SendByte((uint8_t)(Addr & 0x00FF));   /* 设置低起始地址 */      
		I2C_WaitAck();
		for(i=0;i<bytes;i++)
		{
			I2C_SendByte(*_pWriteBuf++);
			I2C_WaitAck();	
		}
		I2C_Stop();
		I2C_delay(10000);//等待数据写完
	}
		I2C_Stop();
   return ENABLE;
}
 /**
  * @file   I2C_ReadByte
  * @brief  读取一串数据
  * @param  
	*					- pBuffer: 存放读出数据
	*     	  - length: 待读出长度
	*         - ReadAddress: 待读出地址
	*         - DeviceAddress: 器件类型(24c16或SD2403)
  * @retval 返回为:=1成功读入,=0失败
  */
FunctionalState I2C_ReadByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress)//,  uint8_t DeviceAddress)
{		
    if(!I2C_Start())return DISABLE;
    I2C_SendByte(EE_DEV_ADDR|I2C_WR); /* 设置高起始地址+器件地址 */ 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte((uint8_t)(ReadAddress & 0x00FF));   /* 设置低起始地址 */      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(EE_DEV_ADDR|I2C_RD);
    I2C_WaitAck();
    while(length)
    {
      *pBuffer = I2C_ReceiveByte();
      if(length == 1)I2C_NoAck();
      else I2C_Ack(); 
      pBuffer++;
      length--;
    }
    I2C_Stop();
    return ENABLE;
}

FunctionalState I2C_Erase()
{
	uint16_t i;
	uint8_t buf[EE_SIZE];
	/* 填充缓冲区 */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
	if(I2C_WriteBytes(buf, 0, EE_SIZE)==DISABLE)
	{
			printf("擦除eeprom出错！\r\n");
			return  DISABLE;
	}
	else
	{
	    printf("擦除eeprom成功！\r\n");
		  return  ENABLE;
	}

}

/**
  * @file   I2C_Test
  * @brief  I2C(AT24C08)读写测试。
  * @param  无
  * @retval 无
  */
void I2C_Test1(void)
{
		uint16_t Addr;
		uint8_t WriteBuffer[256],ReadBuffer[256];
	 //I2C_Erase();
		for(Addr=0; Addr<256; Addr++)
	  {
				WriteBuffer[Addr]=Addr;	 /* 填充WriteBuffer */
				printf("0x%02x ",WriteBuffer[Addr]);
				if(Addr%16 == 15)    
				printf("\n\r");
    }
		/* 开始向EEPROM写数据 */
		printf("\r\n EEPROM 24C08 Write Test \r\n");
		I2C_WriteBytes(WriteBuffer, 0, 256);
		printf("\r\n EEPROM Write Test OK \r\n");

		/* EEPROM读数据 */
		printf("\r\n EEPROM 24C08 Read Test \r\n");
		I2C_ReadByte(ReadBuffer, sizeof(WriteBuffer),0);//, EE_DEV_ADDR);
    for(Addr=0; Addr<256; Addr++)
		{
			printf("0x%02x ",ReadBuffer[Addr]);
			if(Addr%16 == 15)    
			printf("\n\r");
		}
		if(  memcmp(WriteBuffer,ReadBuffer,sizeof(WriteBuffer)) == 0 ) /* 匹配数据 */
		printf("\r\n EEPROM 24C08 Read Test OK\r\n");
		else
		printf("\r\n EEPROM 24C08 Read Test False\r\n");
}



/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
