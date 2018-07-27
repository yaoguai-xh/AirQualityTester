/*************************************************************
                             \(^o^)/
  Copyright (C), 2013-2020, ZheJiang University of Technology
  File name  : SHT10.h 
  Author     : ziye334    
  Version    : V1.0 
  Data       : 2014/3/10      
  Description: Digital temperature and humidity sensor driver code
  
*************************************************************/
#ifndef __SHT10_H__
#define __SHT10_H__
#include "stm32f10x.h"

enum {TEMP, HUMI};

/* GPIO????? */
#define SHT10_AHB2_CLK	RCC_APB2Periph_GPIOB
#define SHT10_DATA_PIN	GPIO_Pin_6
#define SHT10_SCK_PIN	GPIO_Pin_7
#define SHT10_DATA_PORT	GPIOB
#define SHT10_SCK_PORT	GPIOB

#define SHT10_DATA_H()	GPIO_SetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)			 //??DATA???
#define SHT10_DATA_L()	GPIO_ResetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)			 //??DATA???
#define SHT10_DATA_R()	GPIO_ReadInputDataBit(SHT10_DATA_PORT, SHT10_DATA_PIN)	 //?DATA???

#define SHT10_SCK_H()	GPIO_SetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)				 //??SCK???
#define SHT10_SCK_L()	GPIO_ResetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)			 //??SCK???

/* ???????? */
#define	noACK	0
#define ACK		1
								//addr  command	 r/w
#define STATUS_REG_W	0x06	//000	 0011	  0	  ??????
#define STATUS_REG_R	0x07	//000	 0011	  1	  ??????
#define MEASURE_TEMP 	0x03	//000	 0001	  1	  ????
#define MEASURE_HUMI 	0x05	//000	 0010	  1	  ????
#define SOFTRESET       0x1E	//000	 1111	  0	  ??


void SHT10_Config(void);
void SHT10_ConReset(void);
u8 SHT10_SoftReset(void);
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode);
void SHT10_Calculate(u16 t, u16 rh,float *p_temperature, float *p_humidity);
float SHT10_CalcuDewPoint(float t, float h);


#endif
