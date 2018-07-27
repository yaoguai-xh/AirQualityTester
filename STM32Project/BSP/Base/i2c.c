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
#include "i2c.h"
#include "usart.h"

#define  EEP_Firstpage      0x00
extern u32 volatile nTime;
u16 EEPROM_ADDRESS;
 /**
  * @file   I2C_GPIO_Config
  * @brief  I2C1 I/O配置
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 

    /* 使能与 I2C1 有关的时钟 */
    RCC_APB2PeriphClockCmd(EEPROM_I2C_SCL_GPIO_RCC|EEPROM_I2C_SDA_GPIO_RCC,ENABLE);
    RCC_APB1PeriphClockCmd(EEPROM_I2C_RCC,ENABLE);  

    /* PB6-I2C1_SCL、PB7-I2C1_SDA*/
    GPIO_InitStructure.GPIO_Pin =  EEPROM_I2C_SCL_PIN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
    GPIO_Init(EEPROM_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
	  
	  GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
    GPIO_Init(EEPROM_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}

 /**
  * @file   I2C_Mode_Config
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
static void I2C_Mode_Config(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  /* 使能 I2C1 */
  I2C_Cmd(EEPROM_I2C, ENABLE);
  /* I2C1 初始化 */
  I2C_Init(EEPROM_I2C, &I2C_InitStructure);
	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(EEPROM_I2C, ENABLE);    
}

 /**
  * @file   I2C_EE_Init
  * @brief  I2C 外设(EEPROM)初始化
  * @param  无
  * @retval 无
  */
void I2C_EE_Init(void)
{

	I2C_GPIO_Config(); 

	I2C_Mode_Config();
	EEPROM_ADDRESS=0xa0;
//	/* 根据头文件i2c_ee.h中的定义来选择EEPROM要写入的地址 */
//	#ifdef EEPROM_Block0_ADDRESS
//	/* 选择 EEPROM Block0 来写入 */
//	EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
//	#endif

//	#ifdef EEPROM_Block1_ADDRESS  
//	/* 选择 EEPROM Block1 来写入 */
//	EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
//	#endif

//	#ifdef EEPROM_Block2_ADDRESS  
//	/* 选择 EEPROM Block2 来写入 */
//	EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
//	#endif

//	#ifdef EEPROM_Block3_ADDRESS  
//	/* 选择 EEPROM Block3 来写入 */
//	EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
//	#endif
}

 /**
  * @file   I2C_EE_BufferWrite
  * @brief  将缓冲区中的数据写到I2C EEPROM中
  * @param  
  *					-pBuffer 缓冲区指针
  * 				-WriteAddr 接收数据的EEPROM的地址
  * 				-NumByteToWrite 要写入EEPROM的字节数
  * @retval 无
  */
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}

 /**
  * @file   I2C_EE_ByteWrite
  * @brief  写一个字节到I2C EEPROM中
  * @param  
  *					-pBuffer 缓冲区指针
  * 				-WriteAddr 接收数据的EEPROM的地址
  * @retval 无
  */
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr)
{
  /* Send STRAT condition */
  I2C_GenerateSTART(EEPROM_I2C, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT));  

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(EEPROM_I2C, WriteAddr);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData(EEPROM_I2C, *pBuffer); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STOP condition */
  I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
}

 /**
  * @file   I2C_EE_PageWrite
  * @brief  在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数不能超过EEPROM页的大小。AT24C02每页有8个字节。
  * @param  
  *					-pBuffer 缓冲区指针
  * 				-WriteAddr 接收数据的EEPROM的地址
  *         -NumByteToWrite 要写入EEPROM的字节数
  * @retval 无
  */
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY) ); // Added by Najoua 27/08/2008
    
  /* Send START condition */
  I2C_GenerateSTART(EEPROM_I2C, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)  ); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
	 

  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)  );  

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(EEPROM_I2C, WriteAddr);  

  /* Test on EV8 and clear it */
		 

  while(! I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)  );

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(EEPROM_I2C, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
			 

    while (!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)  );
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
}

 /**
  * @file   I2C_EE_BufferRead
  * @brief  从EEPROM里面读取一块数据。 
  * @param  
  *					-pBuffer 缓冲区指针
  * 				-WriteAddr 接收数据的EEPROM的地址
  *         -NumByteToWrite 要从EEPROM读取的字节数
  * @retval 无
  */
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  //*((u8 *)0x4001080c) |=0x80; 
	 
    while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY)  ); // Added by Najoua 27/08/2008
  /* Send START condition */
  I2C_GenerateSTART(EEPROM_I2C, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  /* Test on EV5 and clear it */
		 
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)  );

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
		 
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)  );
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(EEPROM_I2C, ReadAddr);  

  /* Test on EV8 and clear it */
		 
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)  );
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(EEPROM_I2C, ENABLE);
  
  /* Test on EV5 and clear it */
		 
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT)  );
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
		 
  while(!I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) );
  
  /* While there is data to be read */
	 
  while(NumByteToRead  )  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(EEPROM_I2C, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(EEPROM_I2C);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(EEPROM_I2C, ENABLE);
}

 /**
  * @file   I2C_EE_WaitEepromStandbyState
  * @brief  Wait for EEPROM Standby state 
  * @param  无
  * @retval 无
  */
void I2C_EE_WaitEepromStandbyState(void)      
{
  vu16 SR1_Tmp = 0;
		 
  do
  {
    /* Send START condition */
    I2C_GenerateSTART(EEPROM_I2C, ENABLE);
    /* Read I2C1 SR1 register */
    SR1_Tmp = I2C_ReadRegister(EEPROM_I2C, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(EEPROM_I2C, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  }while(!(I2C_ReadRegister(EEPROM_I2C, I2C_Register_SR1) & 0x0002) );
  
  /* Clear AF flag */
  I2C_ClearFlag(EEPROM_I2C, I2C_FLAG_AF);
    /* STOP condition */    
    I2C_GenerateSTOP(EEPROM_I2C, ENABLE); // Added by Najoua 27/08/2008
}

/**
  * @file   I2C_Test
  * @brief  I2C(AT24C08)读写测试。
  * @param  无
  * @retval 无
  */
void I2C_Test(void)
{
	uint8_t WriteBuffer[256],ReadBuffer[256];
	uint16_t i;
	for ( i=0; i<256; i++) 
	{ 	WriteBuffer[i]=1;	 }
	I2C_EE_BufferWrite( WriteBuffer, EEP_Firstpage, 256);	 
EEPROM_ADDRESS=0xa2;
	for ( i=0; i<256; i++) 
	{ 	WriteBuffer[i]=2;	 }
	I2C_EE_BufferWrite( WriteBuffer, EEP_Firstpage, 256);	 
EEPROM_ADDRESS=0xa4;
	for ( i=0; i<256; i++) 
	{ 	WriteBuffer[i]=3;	 }
	I2C_EE_BufferWrite( WriteBuffer, EEP_Firstpage, 256);	 
EEPROM_ADDRESS=0xa6;
	for ( i=0; i<256; i++) 
	{ 	WriteBuffer[i]=4;	 }
	I2C_EE_BufferWrite( WriteBuffer, EEP_Firstpage, 256);	 
EEPROM_ADDRESS=0xa0;
	/* 填充WriteBuffer */  
//	for ( i=0; i<256; i++) 
//	{   
//		WriteBuffer[i]=i;	 
////		printf("0x%02x ", WriteBuffer[i]);
////		if(i%16 == 15)    
////		printf("\n\r");   
//	}
	/* 开始向EEPROM写数据 */
//	I2C_EE_BufferWrite( WriteBuffer, EEP_Firstpage, 256);	 
//	I2C_EE_BufferWrite( WriteBuffer+1, 0x100, 256);	 
	printf("\r\n EEPROM Write Test OK \r\n");
	/* EEPROM读数据 */
	printf("\r\n EEPROM 24C02 Read Test 0 \r\n");
	
	I2C_EE_BufferRead(ReadBuffer, EEP_Firstpage, 256);//将EEPROM读出数据顺序保持到I2c_Buf_Read中  
	for (i=0; i<256; i++)
	{	
		printf("0x%02X ", ReadBuffer[i]);//将I2c_Buf_Read中的数据通过串口打印
		if(i%16 == 15)    
		printf("\n\r");
	}
	printf("\r\n EEPROM 24C02 Read Test 2\r\n");
EEPROM_ADDRESS=0xa2;

	I2C_EE_BufferRead(ReadBuffer, EEP_Firstpage, 256);//将EEPROM读出数据顺序保持到I2c_Buf_Read中  
	for (i=0; i<256; i++)
	{	
		printf("0x%02X ", ReadBuffer[i]);//将I2c_Buf_Read中的数据通过串口打印
		if(i%16 == 15)    
		printf("\n\r");
	}
	printf("\r\n EEPROM 24C02 Read Test 4\r\n");
EEPROM_ADDRESS=0xa4;

	I2C_EE_BufferRead(ReadBuffer, EEP_Firstpage, 256);//将EEPROM读出数据顺序保持到I2c_Buf_Read中  
	for (i=0; i<256; i++)
	{	
		printf("0x%02X ", ReadBuffer[i]);//将I2c_Buf_Read中的数据通过串口打印
		if(i%16 == 15)    
		printf("\n\r");
	}
	printf("\r\n EEPROM 24C02 Read Test 6\r\n");
EEPROM_ADDRESS=0xa6;

	I2C_EE_BufferRead(ReadBuffer, EEP_Firstpage, 256);//将EEPROM读出数据顺序保持到I2c_Buf_Read中  
	for (i=0; i<256; i++)
	{	
		printf("0x%02X ", ReadBuffer[i]);//将I2c_Buf_Read中的数据通过串口打印
		if(i%16 == 15)    
		printf("\n\r");
	}
	


	if(  memcmp(WriteBuffer,ReadBuffer,sizeof(WriteBuffer)) == 0 ) /* 匹配数据 */
  printf("\r\n EEPROM 24C08 Read Test OK\r\n");
  else
  printf("\r\n EEPROM 24C08 Read Test False\r\n");
}

/******************** ********************************* ********************/
