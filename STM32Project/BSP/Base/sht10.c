/*************************************************************
                             \(^o^)/
  Copyright (C), 2013-2020, ZheJiang University of Technology
  File name  : SHT10.c 
  Author     : ziye334    
  Version    : V1.0 
  Data       : 2014/3/10      
  Description: Digital temperature and humidity sensor driver code
  
*************************************************************/
#include "SHT10.h"
#include <math.h>


/*************************************************************
  Function   :SHT10_Dly  
  Description:SHT10???????
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Dly(void)
{
	u16 i;
	for(i = 500; i > 0; i--);
}


/*************************************************************
  Function   :SHT10_Config  
  Description:??? SHT10??
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	//???SHT10????
	RCC_APB2PeriphClockCmd(SHT10_AHB2_CLK ,ENABLE);
		
	//PD0 DATA ????	
	GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
	//PD1 SCK ????
	GPIO_InitStructure.GPIO_Pin = SHT10_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT10_SCK_PORT, &GPIO_InitStructure);

	SHT10_ConReset();	//????
}


/*************************************************************
  Function   :SHT10_DATAOut
  Description:??DATA?????
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAOut(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PD0 DATA ????	
	GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
	GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   :SHT10_DATAIn  
  Description:??DATA?????
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_DATAIn(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PD0 DATA ????	
	GPIO_InitStructure.GPIO_Pin = SHT10_DATA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SHT10_DATA_PORT, &GPIO_InitStructure);
}


/*************************************************************
  Function   :SHT10_WriteByte  
  Description:?1??
  Input      : value:??????        
  return     : err: 0-??  1-??    
*************************************************************/
u8 SHT10_WriteByte(u8 value)
{
	u8 i, err = 0;
	
	SHT10_DATAOut();			  //??DATA??????

	for(i = 0x80; i > 0; i /= 2)  //?1???
	{
		if(i & value)
			SHT10_DATA_H();
		else
			SHT10_DATA_L();
		SHT10_Dly();
		SHT10_SCK_H();
		SHT10_Dly();
		SHT10_SCK_L();
		SHT10_Dly();
	}
	SHT10_DATAIn();				  //??DATA??????,??DATA?
	SHT10_SCK_H();
	err = SHT10_DATA_R();		  //??SHT10????
	SHT10_SCK_L();

	return err;
}

/*************************************************************
  Function   :SHT10_ReadByte  
  Description:?1????
  Input      : Ack: 0-???  1-??        
  return     : err: 0-?? 1-??    
*************************************************************/
u8 SHT10_ReadByte(u8 Ack)
{
	u8 i, val = 0;

	SHT10_DATAIn();				  //??DATA??????
	for(i = 0x80; i > 0; i /= 2)  //??1?????
	{
		SHT10_Dly();
		SHT10_SCK_H();
		SHT10_Dly();
		if(SHT10_DATA_R())
			val = (val | i);
		SHT10_SCK_L();
	}
	SHT10_DATAOut();			  //??DATA??????
	if(Ack)
		SHT10_DATA_L();			  //??,????????????(????)
	else
		SHT10_DATA_H();			  //???,??????
	SHT10_Dly();
	SHT10_SCK_H();
	SHT10_Dly();
	SHT10_SCK_L();
	SHT10_Dly();

	return val;					  //??????
}


/*************************************************************
  Function   :SHT10_TransStart  
  Description:??????,????:
                     _____         ________
               DATA:      |_______|
                         ___     ___
               SCK : ___|   |___|   |______	
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_TransStart(void)
{
	SHT10_DATAOut();			  //??DATA??????

	SHT10_DATA_H();
	SHT10_SCK_L();
	SHT10_Dly();
	SHT10_SCK_H();
	SHT10_Dly();
	SHT10_DATA_L();
	SHT10_Dly();
	SHT10_SCK_L();
	SHT10_Dly();
	SHT10_SCK_H();
	SHT10_Dly();
	SHT10_DATA_H();
	SHT10_Dly();
	SHT10_SCK_L();

}


/*************************************************************
  Function   :SHT10_ConReset  
  Description:????,????:
                     _____________________________________________________         ________
               DATA:                                                      |_______|
                        _    _    _    _    _    _    _    _    _        ___     ___
               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
  Input      : none        
  return     : none    
*************************************************************/
void SHT10_ConReset(void)
{
	u8 i;

	SHT10_DATAOut();

	SHT10_DATA_H();
	SHT10_SCK_L();

	for(i = 0; i < 9; i++)		  //??SCK??9c?
	{
		SHT10_SCK_H();
		SHT10_Dly();
		SHT10_SCK_L();
		SHT10_Dly();
	}
	SHT10_TransStart();			  //????
}



/*************************************************************
  Function   :SHT10_SoftReset  
  Description:???
  Input      : none        
  return     : err: 0-??  1-??    
*************************************************************/
u8 SHT10_SoftReset(void)
{
	u8 err = 0;

	SHT10_ConReset();			      //????
	err += SHT10_WriteByte(SOFTRESET);//?RESET????

	return err;
}


/*************************************************************
  Function   :SHT10_ReadStatusReg  
  Description:??????
  Input      : p_value-?????;p_checksun-???????       
  return     : err: 0-??  0-??    
*************************************************************/
u8 SHT10_ReadStatusReg(u8 *p_value, u8 *p_checksum)
{
	u8 err = 0;

	SHT10_TransStart();					//????
	err = SHT10_WriteByte(STATUS_REG_R);//?STATUS_REG_R?????????
	*p_value = SHT10_ReadByte(ACK);		//??????
	*p_checksum = SHT10_ReadByte(noACK);//???????
	
	return err;
}



/*************************************************************
  Function   :SHT10_WriteStatusReg  
  Description:??????
  Input      : p_value-???????       
  return     : err: 0-??  1-??    
*************************************************************/
u8 SHT10_WriteStatusReg(u8 *p_value)
{
	u8 err = 0;

	SHT10_TransStart();					 //????
	err += SHT10_WriteByte(STATUS_REG_W);//?STATUS_REG_W????????
	err += SHT10_WriteByte(*p_value);	 //?????

	return err;
}



/*************************************************************
  Function   :SHT10_Measure  
  Description:????????????
  Input      : p_value-????;p_checksum-??????        
  return     : err: 0-?? 1—??    
*************************************************************/
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode)
{
	u8 err = 0;
	u32 i;
	u8 value_H = 0;
	u8 value_L = 0;

	SHT10_TransStart();						 //????
	switch(mode)							 
	{
	case TEMP:								 //????
		err += SHT10_WriteByte(MEASURE_TEMP);//?MEASURE_TEMP??????
		break;
	case HUMI:
		err += SHT10_WriteByte(MEASURE_HUMI);//?MEASURE_HUMI??????
		break;
	default:
		break;
	}
	SHT10_DATAIn();
	for(i = 0; i < 72000000; i++)			     //??DATA?????
	{
		if(SHT10_DATA_R() == 0) break;	     //???DATA????,????
	}
	if(SHT10_DATA_R() == 1)   			     //???????
		err += 1;
	value_H = SHT10_ReadByte(ACK);
	value_L = SHT10_ReadByte(ACK);
	*p_checksum = SHT10_ReadByte(noACK);  	 //??????
	*p_value = (value_H << 8) | value_L;
	return err;
}


/*************************************************************
  Function   :SHT10_Calculate  
  Description:???????
  Input      : Temp-??????????;Humi-??????????
               p_humidity-??????????;p_temperature-?????????        
  return     : none    
*************************************************************/
void SHT10_Calculate(u16 t, u16 rh, float *p_temperature, float *p_humidity)
{
	const float d1 = -39.7;
	const float d2 = +0.01;
	const float C1 = -2.0468;
	const float	C2 = +0.0367;
	const float C3 = -0.0000015955;	
	const float T1 = +0.01;
	const float T2 = +0.00008;

	float RH_Lin;  										//RH???	
	float RH_Ture; 										//RH???
	float temp_C;

	temp_C = d1 + d2 * t;  							    //?????	
	RH_Lin = C1 + C2 * rh + C3 * rh * rh;			    //?????
	RH_Ture = (temp_C -25) * (T1 + T2 * rh) + RH_Lin;	//???????,????????

	if(RH_Ture > 100)									//???????
		RH_Ture	= 100;
	if(RH_Ture < 0.1)
		RH_Ture = 0.1;									//???????

	*p_humidity = RH_Ture;
	*p_temperature = temp_C;

}


/*************************************************************
  Function   :SHT10_CalcuDewPoint  
  Description:????
  Input      : h-?????;t-?????        
  return     : dew_point-??    
*************************************************************/
float SHT10_CalcuDewPoint(float t, float h)
{
	float logEx, dew_point;

	logEx = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
	dew_point = ((0.66077 - logEx) * 237.3) / (logEx - 8.16077);

	return dew_point; 
}


//2.8.3?main?????:
//int main(void)
//{  
//	u16 humi_val, temp_val;
//	u8 err = 0, checksum = 0;
//	float humi_val_real = 0.0; 
//	float temp_val_real = 0.0;
//	float dew_point = 0.0;
//	
//	BSP_Init();
//	printf("\nSHT10???????!!!\n");
//	SHT10_Config();
//	while(1)
//	{
//		err += SHT10_Measure(&temp_val, &checksum, TEMP);		  //???????
//		err += SHT10_Measure(&humi_val, &checksum, HUMI);		  //???????
//		if(err != 0)
//			SHT10_ConReset();
//		else
//		{
//			SHT10_Calculate(temp_val, humi_val, &temp_val_real, &humi_val_real); //?????????
//			dew_point = SHT10_CalcuDewPoint(temp_val_real, humi_val_real);		 //??????
//		} 
//		printf("???????:%2.1f?,???:%2.1f%%,?????%2.1f?\r\n", temp_val_real, humi_val_real, dew_point);
//		LED1_Toggle();
//		Delay_ms(1000);
//	}
//}

