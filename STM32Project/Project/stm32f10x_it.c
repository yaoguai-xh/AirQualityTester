/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "w5500_conf.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

#define RLEN 50
extern uint8_t rxbuf[RLEN];	
extern uint8_t start,end;
extern uint8_t PMrxbuf[RLEN];	
extern uint8_t PMstart,PMend;
extern uint8_t distance(uint8_t start,uint8_t end);
extern uint32_t RTC_GetCounter(void);
extern void tm1637DisplayTime(int v,int f);
extern void calcPM25();
extern void calcHCHO();
extern void send_read_cmd(void);

int volatile second;
int volatile disptype;
int volatile dispflag;
u8 volatile socktime;
u8 volatile wifi_conn_ok;
void RTC_IRQHandler(void)
{
	static u32 time;
	u16 h,m;
	time=RTC_GetCounter();
	h=time/3600;
	m=(time%3600)/60;
	
	second++;
	send_read_cmd();

	if((disptype==4) && (second%2==1)){
			tm1637DisplayTime(h*100+m,1);
	}else if((disptype==4) && (second%2==0)){
			tm1637DisplayTime(h*100+m,0);
	}
	if(second % 5==0){
			socktime=1;
	}
	if(second % 2==0){
			dispflag=1;
			disptype++;
			if(disptype==5)
					disptype=0;
	}
  if(RTC_GetITStatus(RTC_IT_SEC) != RESET)				 //读取秒中断状态
  {
    RTC_ClearITPendingBit(RTC_IT_SEC);					 //清除秒中断标志

    /* 时钟更新标志置位 */
//    TimeDisplay = 1;	  
    RTC_WaitForLastTask();							     //等待上一次对RTC寄存器的写操作是否已经完成    
    if(RTC_GetCounter() == 0x00015180)				     //当前时间是23:59:59时 复位为0:0:0 	    
    {
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	  PWR->CR|=1<<8;                  					 //取消备份区写保护
	  RTC_EnterConfigMode();						     //允许配置 	  				
	  RTC_WaitForLastTask();                             //等待上一次对RTC寄存器的写操作是否已经完成 
      RTC_SetCounter(0x0);								 //写入复位值
      RTC_WaitForLastTask();							 //等待上一次对RTC寄存器的写操作是否已经完成    
    }
	else if(RTC_GetCounter() > 0x00015180)				 //当再次上电后计数值超过0x00015180， 复位为当前值取模0x00015180。	    
    {
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	  PWR->CR|=1<<8;                                     //取消备份区写保护
	  RTC_EnterConfigMode();			                 //允许配置 
	  RTC_WaitForLastTask();                             //等待上一次对RTC寄存器的写操作是否已经完成    
      RTC_SetCounter(RTC_GetCounter()%0x00015180);		 //写入复位值
      RTC_WaitForLastTask();							 //等待上一次对RTC寄存器的写操作是否已经完成    
    }
  }
}


void USART2_IRQHandler(void)      //??1 ??????
{
  unsigned int i;
char c; 

  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	   //??????????
  {	
		if((end+1)%RLEN !=start){
			rxbuf[end]=USART_ReceiveData(USART2);
			end=(end+1)%RLEN;
		}	
    else USART_ReceiveData(USART2);   //?????????????????
//			if(reccount >=9 ){
//				rec_flag=1;
//				reccount=0;
//			}
		calcHCHO();	
  }
  
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                   //???????STM32 USART ??????????BUG 
  { 
     USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					     //?????????, 
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);					     //?????????, 
  }	
  
}

void USART1_IRQHandler(void)      //??1 ??????
{
  unsigned int i;
char c; 

  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	   //??????????
  {	
		if((PMend+1)%RLEN !=PMstart){
			PMrxbuf[PMend]=USART_ReceiveData(USART1);
			PMend=(PMend+1)%RLEN;
		}	
    else USART_ReceiveData(USART1);   //?????????????????
//			if(reccount >=9 ){
//				rec_flag=1;
//				reccount=0;
//			}
		calcPM25();	
  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                   //???????STM32 USART ??????????BUG 
  { 
     USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					     //?????????, 
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);					     //?????????, 
  }	
  
}

//extern char wifibuft[500];
extern char wifibuf[500];
unsigned int volatile i=0;
extern char volatile wififlag;
void USART3_IRQHandler(void)      //??1 ??????
{ static char c,start=0;

  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)	   //??????????
  {	
    c=USART_ReceiveData(USART3);   //?????????????????
		if(c=='{')  start=1;
		if(start && wififlag==0){
				wifibuf[i++]=c;
				if(i==5 && wifibuf[1]=='[' &&wifibuf[2]=='o' &&wifibuf[3]=='k' &&wifibuf[4]==']' ){
						wifi_conn_ok=1;
						i=0; start=0;
						printf("wifi conn success");
				}else if(i==5 && wifibuf[1]=='[' &&wifibuf[2]=='d' &&wifibuf[3]=='i' &&wifibuf[4]==']'){
						wifi_conn_ok=0;
						i=0; start=0;
						printf("wifi conn brake");
				}
				if(wifibuf[i-1]=='}'){
						wifibuf[i]=0;
						//printf("%s\r\n",wifibuf);
						//strcpy(wifibuft,wifibuf);
						i=0;
						start=0;
						wififlag=1;
				}
				if(i==499)  i=0;
		}
//    wifibuf[i]=USART_ReceiveData(USART3);   //?????????????????
//		if(wifibuf[0]=='{'){
//				i++;
//				if(wifibuf[i-1]=='}'){
//						wifibuf[i]=0;
//						//printf("%s\r\n",wifibuf);
//						strcpy(wifibuft,wifibuf);
//						i=0;
//						wififlag=1;
//				}
//				if(i==499)  i=0;
//		}
  }
  
//  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)                   //???????STM32 USART ??????????BUG 
//  { 
//     USART_ITConfig(USART3, USART_IT_TXE, DISABLE);					     //?????????, 
//		USART_ITConfig(USART3, USART_IT_TC, DISABLE);					     //?????????, 
//  }	
  
}
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
u32 volatile nTime;
void SysTick_Handler(void)
{
	//TimingDelay_Decrement();
	if(nTime)
	nTime--;
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		timer2_isr();
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);  		 
	}		 	
}



/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
