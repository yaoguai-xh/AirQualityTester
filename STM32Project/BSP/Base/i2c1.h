#ifndef __I2C_H
#define __I2C_H			 
#include "stm32f10x.h"

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <string.h>
/* Private define ------------------------------------------------------------*/

#define EEPROM_I2C_SCL_PIN      					 	GPIO_Pin_6   
#define EEPROM_I2C_SCL_GPIO_PORT   	       	GPIOB  
#define EEPROM_I2C_SCL_GPIO_RCC      			  RCC_APB2Periph_GPIOB  
 
#define EEPROM_I2C_SDA_PIN      					 	GPIO_Pin_7   
#define EEPROM_I2C_SDA_GPIO_PORT   	       	GPIOB  
#define EEPROM_I2C_SDA_GPIO_RCC      			  RCC_APB2Periph_GPIOB  

#define SCL_H         	 GPIO_SetBits(EEPROM_I2C_SCL_GPIO_PORT , EEPROM_I2C_SCL_PIN)   /*GPIOB->BSRR = GPIO_Pin_6*/
#define SCL_L            GPIO_ResetBits(EEPROM_I2C_SCL_GPIO_PORT , EEPROM_I2C_SCL_PIN) /*GPIOB->BRR  = GPIO_Pin_6 */
   
#define SDA_H         	 GPIO_SetBits(EEPROM_I2C_SDA_GPIO_PORT , EEPROM_I2C_SDA_PIN)   /*GPIOB->BSRR = GPIO_Pin_7*/
#define SDA_L         	 GPIO_ResetBits(EEPROM_I2C_SDA_GPIO_PORT , EEPROM_I2C_SDA_PIN) /*GPIOB->BRR  = GPIO_Pin_7*/

#define SCL_read       	GPIO_ReadInputDataBit(EEPROM_I2C_SCL_GPIO_PORT , EEPROM_I2C_SCL_PIN)/* GPIOB->IDR  & GPIO_Pin_6   */
#define SDA_read       	GPIO_ReadInputDataBit(EEPROM_I2C_SDA_GPIO_PORT , EEPROM_I2C_SDA_PIN)/*GPIOB->IDR  & GPIO_Pin_7	  */

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */
#define EE_PageSize  8  /* 24xx02的页面大小 */
#define EE_DEV_ADDR		0xA0   /* 24xx02的设备地址 */
#define EE_SIZE				256			  /* 24xx02总容量 */
  


/* Private function prototypes -----------------------------------------------*/
void I2C_Test1(void);
void I2C_Configuration(void);
FunctionalState I2C_WriteByte(uint8_t SendByte, uint16_t WriteAddress);
FunctionalState I2C_ReadByte(uint8_t* pBuffer,   uint16_t length,   uint16_t ReadAddress);//,  uint8_t DeviceAddress);
FunctionalState I2C_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
#endif 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
