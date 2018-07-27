#include "rtc.h"
#include "stdio.h"

vu32 TimeDisplay = 0;
ErrorStatus HSEStartUpStatus;

void Clock_ini(void){
	  NVIC_InitTypeDef NVIC_InitStructure;	

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  
 
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;					  //配置外部中断源（秒中断） 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

//     RTC_Configuration();							      //RTC初始化	 
//     printf("\r\n RTC configured....");
//     Time_Adjust(22,52,10);										  //设置RTC 时钟参数
//     BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);    	      //RTC设置后，将已配置标志写入备份数据寄存器 

	
	
  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)		     //判断保存在备份寄存器的RTC标志是否已经被配置过
  {
     printf("\r\n\n RTC not yet configured....");
     RTC_Configuration();							      //RTC初始化	 
     printf("\r\n RTC configured....");
     Time_Adjust(21,59,10);										  //设置RTC 时钟参数
     BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);    	      //RTC设置后，将已配置标志写入备份数据寄存器 
  }
  else
  {	     
     if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)	  //检查是否掉电重启
     {
       printf("\r\n\n Power On Reset occurred....");
     }												     
     else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) //检查是否reset复位
     {
       printf("\r\n\n External Reset occurred....");
     }
     printf("\r\n No need to configure RTC....");  
     RTC_WaitForSynchro();								   //等待RTC寄存器被同步 
     RTC_ITConfig(RTC_IT_SEC, ENABLE);					   //使能秒中断
     RTC_WaitForLastTask();								   //等待写入完成
  }
  RCC_ClearFlag();										   //清除复位标志
}

void RTC_Configuration(void)
{ 
  /* 使能 PWR 和 BKP 的时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  
  /* 允许访问BKP区域 */
  PWR_BackupAccessCmd(ENABLE);

  /* 复位BKP */
  BKP_DeInit();

#ifdef RTCClockSource_LSI
  /* 使能内部RTC时钟 */ 
  RCC_LSICmd(ENABLE);
  /* 等待RTC内部时钟就绪 */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  /* 选择RTC内部时钟为RTC时钟 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);  
#elif defined	RTCClockSource_LSE  
  /* 使能RTC外部时钟 */
  RCC_LSEConfig(RCC_LSE_ON);
  /* 等待RTC外部时钟就绪 */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {	    
  }

  /* 选择RTC外部时钟为RTC时钟 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  
#endif
  /* 使能RTC时钟 */
  RCC_RTCCLKCmd(ENABLE);


#ifdef RTCClockOutput_Enable  
  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                               functionality must be disabled */
                               
  /* 使能在TAMPER脚输出RTC时钟 */
  BKP_RTCCalibrationClockOutputCmd(ENABLE);
#endif 

  /* 等待RTC寄存器同步 */
  RTC_WaitForSynchro();

  /* 等待写RTC寄存器完成 */
  RTC_WaitForLastTask();
  
  /* 使能RTC秒中断 */  
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* 等待写RTC寄存器完成 */
  RTC_WaitForLastTask();
  
  /* 设置RTC预分频 */
#ifdef RTCClockSource_LSI
  RTC_SetPrescaler(31999);            /* RTC period = RTCCLK/RTC_PR = (32.000 KHz)/(31999+1) */
#elif defined	RTCClockSource_LSE
  RTC_SetPrescaler(32767);            /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
#endif
  
  /* 等待写RTC寄存器完成 */
  RTC_WaitForLastTask();
}

/****************************************************************************
* 名    称：u32 Time_Regulate(void)
* 功    能：时间校正函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
u32 Time_Regulate(int h,int m,int s)
{
//  u32 Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;

//  printf("\r\n==============Time Settings=====================================");
//  printf("\r\n  Please Set Hours");
//  
//  while(Tmp_HH == 0xFF)				      
//  {
//    Tmp_HH = 12;//USART_Scanf(23);	         
//  }
//  printf(":  %d", Tmp_HH); 
//  printf("\r\n  Please Set Minutes");
//  while(Tmp_MM == 0xFF)
//  {
//    Tmp_MM = 12;//USART_Scanf(59);
//  }
//  printf(":  %d", Tmp_MM); 
//  printf("\r\n  Please Set Seconds");
//  while(Tmp_SS == 0xFF)
//  {
//    Tmp_SS = 12;//USART_Scanf(59);
//  }
//  printf(":  %d", Tmp_SS); 

  /* 返回保存在RTC计数寄存器里的值 */
  //return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
	return((h*3600 + m*60 + s));
}

/****************************************************************************
* 名    称：void Time_Adjust(void)
* 功    能：时间调整函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void Time_Adjust(int h,int m,int s)
{
  /* 等待写RTC寄存器完成 */
  RTC_WaitForLastTask(); 
  /* 改变当前时间 */
  RTC_SetCounter(h*3600 + m*60 + s);
  /* 等待写RTC寄存器完成 */
  RTC_WaitForLastTask();   
}

/****************************************************************************
* 名    称：void Time_Display(u32 TimeVar)
* 功    能：显示当前时间
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
void Time_Display(u32 TimeVar)
{ 
  u32 THH = 0, TMM = 0, TSS = 0;

  /* 计算小时 */
  THH = TimeVar/3600;
  /* 计算分钟 */
  TMM = (TimeVar % 3600)/60;
  /* 计算秒 */
  TSS = (TimeVar % 3600)% 60;
  printf("Time: %0.2d:%0.2d:%0.2d\r\n",THH, TMM, TSS);
}

/****************************************************************************
* 名    称：void Time_Show(void)
* 功    能：循环显示当前时间
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/
//void Time_Show(void)
//{
//  printf("\n\r");	   
//  while(1)
//  {
//    /* 秒更新发生 */
//    if(TimeDisplay == 1)
//    {    
//      /* 显示当前时间 */
//      Time_Display(RTC_GetCounter());
//      TimeDisplay = 0;
//    }
//  }
//}

