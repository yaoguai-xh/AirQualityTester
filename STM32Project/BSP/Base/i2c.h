#ifndef __IIC_H
#define __IIC_H			 
#include "stm32f10x.h"
#include <string.h>

/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xA0   /* E2 = 0 */
//#define EEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */
//#define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
//#define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */


#define I2C_Speed              100000
#define I2C1_SLAVE_ADDRESS7    0xA0
#define I2C_PageSize           8			/* AT24C02每页有8个字节 */

#define EEPROM_I2C      					 	       I2C1
#define EEPROM_I2C_RCC      					 	   RCC_APB1Periph_I2C1 

#define EEPROM_I2C_SCL_PIN      					 	GPIO_Pin_6   
#define EEPROM_I2C_SCL_GPIO_PORT   	       	GPIOB  
#define EEPROM_I2C_SCL_GPIO_RCC      			  RCC_APB2Periph_GPIOB  
 
#define EEPROM_I2C_SDA_PIN      					 	GPIO_Pin_7   
#define EEPROM_I2C_SDA_GPIO_PORT   	       	GPIOB  
#define EEPROM_I2C_SDA_GPIO_RCC      			  RCC_APB2Periph_GPIOB  


void I2C_Test(void);
void I2C_EE_Init(void);
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_EE_WaitEepromStandbyState(void);
extern u16 EEPROM_ADDRESS;
#endif /* __I2C_EE_H */
