#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"



#define LED1_GPIO_RCC           RCC_APB2Periph_GPIOA
#define LED1_GPIO_PORT          GPIOA
#define LED1_GPIO_PIN      			GPIO_Pin_0
#define LED1_ONOFF(x)     			GPIO_WriteBit(GPIOA,GPIO_Pin_0,x);

#define LED2_GPIO_RCC           RCC_APB2Periph_GPIOA
#define LED2_GPIO_PORT          GPIOA
#define LED2_GPIO_PIN      			GPIO_Pin_8
#define LED2_ONOFF(x)     			GPIO_WriteBit(GPIOA,GPIO_Pin_8,x);

#define LED3_GPIO_RCC           RCC_APB2Periph_GPIOA
#define LED3_GPIO_PORT          GPIOA
#define LED3_GPIO_PIN      			GPIO_Pin_1
#define LED3_ONOFF(x)      			GPIO_WriteBit(GPIOA,GPIO_Pin_1,x);


void LED_GPIO_Config(void);	
void LEDXToggle(uint8_t ledx);
#endif
