#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

#define BaudRate	 9600

#define USART1_RCC         	 RCC_APB2Periph_USART1

#define USART1_GPIO_RCC    		RCC_APB2Periph_GPIOA
#define USART1_TX		        	GPIO_Pin_9	// out
#define USART1_RX		        	GPIO_Pin_10	// in 
#define USART1_GPIO_PORT    		GPIOA   


void USART_Config(void);

#define USART2_RCC         	 RCC_APB1Periph_USART2
#define USART2_TX		        	GPIO_Pin_2	// out
#define USART2_RX		        	GPIO_Pin_3	// in 
#define USART2_GPIO_PORT    		GPIOA   

#define USART3_GPIO_RCC    		RCC_APB2Periph_GPIOB
#define USART3_RCC         	 RCC_APB1Periph_USART3
#define USART3_TX		        	GPIO_Pin_10	// out
#define USART3_RX		        	GPIO_Pin_11	// in 
#define USART3_GPIO_PORT    		GPIOB   

#endif
