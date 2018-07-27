/******************** (C) COPYRIGHT  源地工作室 ********************************
 * 文件名  ：usart.c
 * 描述    ：将printf函数重定向到USART1。这样就可以用printf函数将单片机的数据打
 *           印到PC上的超级终端或串口调试助手。     
 * 作者    ：zhuoyingxingyu
 * 淘宝    ：源地工作室http://vcc-gnd.taobao.com/
 * 论坛地址：极客园地-嵌入式开发论坛http://vcc-gnd.com/
 * 版本更新: 2015-11-11
 * 硬件连接: TX->PA9;RX->PA10
 * 调试方式：J-Link-OB
**********************************************************************************/	

//头文件
#include "usart.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


 /**
  * @file   USART_Config
  * @brief  USART1 GPIO 配置,工作模式配置。9600-8-N-1
  * @param  无
  * @retval 无
  */
void USART_Config(void)
{	
    GPIO_InitTypeDef GPIO_InitStructure;	
    USART_InitTypeDef USART_InitStructure;  //定义串口初始化结构体
    RCC_APB2PeriphClockCmd( USART1_RCC | USART1_GPIO_RCC,ENABLE);

    /*USART1_TX ->PA9*/			
    GPIO_InitStructure.GPIO_Pin = USART1_TX;	       //选中串口默认输出管脚         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //定义输出最大速率 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义管脚9的模式  
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);           //调用函数，把结构体参数输入进行初始化		   
    /*USART1_RX ->PA10*/
    GPIO_InitStructure.GPIO_Pin = USART1_RX;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);
    /*串口通讯参数设置*/
    USART_InitStructure.USART_BaudRate = BaudRate; //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;		//校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//使能接收和发送引脚

    USART_Init(USART1, &USART_InitStructure);
		USART_ClearFlag(USART1,USART_FLAG_TC);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);		
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);		
    USART_Cmd(USART1, ENABLE);
		
    /*USART2_TX ->PA2*/			
    RCC_APB1PeriphClockCmd( USART2_RCC ,ENABLE);
		GPIO_InitStructure.GPIO_Pin = USART2_TX;	       //选中串口默认输出管脚         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //定义输出最大速率 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义管脚9的模式  
    GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);           //调用函数，把结构体参数输入进行初始化		   
    /*USART2_RX ->PA3*/
    GPIO_InitStructure.GPIO_Pin = USART2_RX;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);
    /*串口通讯参数设置*/
    USART_InitStructure.USART_BaudRate = BaudRate; //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;		//校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//使能接收和发送引脚

    USART_Init(USART2, &USART_InitStructure);
		USART_ClearFlag(USART2,USART_FLAG_TC);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);		
    USART_Cmd(USART2, ENABLE);

    /*USART3_TX ->PB10*/			
    RCC_APB1PeriphClockCmd( USART3_RCC ,ENABLE);
    RCC_APB2PeriphClockCmd( USART3_GPIO_RCC,ENABLE);
		GPIO_InitStructure.GPIO_Pin = USART3_TX;	       //选中串口默认输出管脚         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //定义输出最大速率 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义管脚9的模式  
    GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);           //调用函数，把结构体参数输入进行初始化		   
    /*USART3_RX ->PB11*/
    GPIO_InitStructure.GPIO_Pin = USART3_RX;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);
    /*串口通讯参数设置*/
    USART_InitStructure.USART_BaudRate = 9600; //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;		//校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//使能接收和发送引脚

    USART_Init(USART3, &USART_InitStructure);
		USART_ClearFlag(USART3,USART_FLAG_TC);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);		
    USART_Cmd(USART3, ENABLE);
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  return ch;
}
