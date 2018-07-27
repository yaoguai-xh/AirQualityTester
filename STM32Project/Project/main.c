/******************** (C) COPYRIGHT  源地工作室 ********************************
 * 文件名  ：main.c
 * 描述    ：完成是MCU通过SPI2对w5500的读写操作，完成对IP,mac，gateway等的操作，
 *						内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突
 *            如果是用网线跟PC机直连，请设置PC机本地连接地址IP为静态IP测试本程序，
 *            请将w5500_conf.c文件中的local_port定义为测试使用的监听端口,默认为5000
 * 库版本  ：V3.50
 * 作者    ：zhuoyingxingyu
 * 淘宝    ：源地工作室http://vcc-gnd.taobao.com/
 * 论坛地址：极客园地-嵌入式开发论坛http://vcc-gnd.com/
 * 版本更新: 2015-11-11
 * 调试方式：J-Link-OB
**********************************************************************************/

//头文件
#include "stm32f10x.h"
//#include "led.h"
#include "SYSTICK.h"
#include "usart.h"
#include "i2c.h"
#include "i2c1.h"
#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp_demo.h"

#include "sht2x.h"
#include "stm32_tm1637.h"
#include "rtc.h"
#include "cjson.h"
#include "dhcp.h"

#define STATUS_ONOFF(x)     			GPIO_WriteBit(GPIOA,GPIO_Pin_0,x);
#define Read_Key                  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)
#define BUZZ_ONOFF(x)     			GPIO_WriteBit(GPIOA,GPIO_Pin_8,x);

struct Config{
		char magic;
		char username[30];
		char useraddr[200];
		char userphone[14];
		char id[20];
		char mac[20];
		char nettype[10];
		char apssid[10];
		char appassword[20];
		char server[20];
		char port[6];
		uint8_t serverip[4];
	  
}config;
cJSON *config_json,*ip_json,*pSub,*senddata;
char *sendjson;

int iSize,iCnt;
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
uint8_t readconfig(void);//w5500的配置是写入eeprom的地址0开始，共18字节， 系统配置从0xff地址开始
uint8_t saveconfig(cJSON *c);
void sendtowifi(char *p,char size);
void send_read_cmd(void );
uint8_t distance(uint8_t start,uint8_t end);
#define RLEN 50
char wifibuf[500];
//char wifibuft[500];
char volatile wififlag;
char volatile configflag;
uint8_t reccount=0;
uint8_t rxbuf[RLEN];	
uint8_t start=0,end=0;
uint8_t PMrxbuf[RLEN];	
uint8_t PMstart=0,PMend=0;
float PM25,HCHO;
extern vu32 TimeDisplay ;
extern SHT2x_PARAM g_sht2x_param;
extern int volatile disptype;
extern int volatile dispflag;
extern uint8  remote_ip[4];											/*远端IP地址*/
extern uint16 remote_port;											/*远端端口号*/
extern u8 sockflag;
extern u8 volatile socktime;
extern u8 volatile wifi_conn_ok;

char *tmp = "text";
int ccount;

u8 j;
__asm void SystemReset(void)
{
 MOV R0, #1           //; 
 MSR FAULTMASK, R0    //; ??FAULTMASK ????????
 LDR R0, =0xE000ED0C  //;
 LDR R1, =0x05FA0004  //; 
 STR R1, [R0]         //; ??????   

 
deadloop
    B deadloop        //; ???????????????
}
void calcPM25(){
    if(distance(PMstart,PMend)>=32 ){
			    if(PMrxbuf[(PMstart)%RLEN]==0x42 && PMrxbuf[(PMstart+1)%RLEN]==0x4d) {
								PM25=(PMrxbuf[(PMstart+22)%RLEN]*256.0+PMrxbuf[(PMstart+23)%RLEN]);
//								printf("%d %d\r\n",PMstart ,PMend);
//								for(j=0;j<32;j++)
//								printf("%x ",PMrxbuf[(PMstart+j)%RLEN]);
//								printf("\r\n");
								PMstart=(PMstart+32)%RLEN;
					}else{
								PMstart=(PMstart+1)%RLEN;
					}
	  }
}
void calcHCHO(){
    if(distance(start,end)>=9 ) {
					if (rxbuf[(start)%RLEN]==0xa5 && rxbuf[(start+1)%RLEN]==0x5a){
							//HCHO=rxbuf[(start+4)%RLEN]*256
							HCHO=(rxbuf[(start+4)%RLEN]*256+rxbuf[(start+5)%RLEN])/100.0;
//							for(j=0;j<9;j++)
//							printf("%x ",rxbuf[(start+j)%RLEN]);
//     					printf("\r\n");
							start=(start+9)%RLEN;
					}else{
						start=(start+1)%RLEN;
					}
				}
}
void display(){

		//甲醛传感器
	  //PM2.5传感器
		//calcPM25();
		
		if(dispflag){
					char *begin_str="DataBegin",*end_str="DataEnd",temp_buf1[20],temp_buf2[20],temp_buf3[20],temp_buf4[20];
					char str[200]={0};
					dispflag=0;
					//printf("disptype=%d\r\n",disptype);
					SHT2x_Test();
					SHT2x_Test();
					for(ccount = 0;ccount < strlen(begin_str);ccount++)
					{
						wifibuf[ccount] = begin_str[ccount];
					}
					wifibuf[strlen(begin_str)]='\0';
					//sendtowifi(wifibuf,strlen(begin_str));
					
					sprintf(temp_buf1,"%.2f",g_sht2x_param.TEMP_HM);
					//sendtowifi(temp_buf1,strlen(temp_buf));
					
					sprintf(temp_buf2,"%.2f",g_sht2x_param.HUMI_HM);
					//sendtowifi(temp_buf,strlen(temp_buf));
					
					sprintf(temp_buf3,"%.1f",PM25+30);
					//sendtowifi(temp_buf,strlen(temp_buf));
					
					sprintf(temp_buf4,"%.2f",HCHO/5.0+0.02);
					//sendtowifi(temp_buf,strlen(temp_buf));
					
					for(ccount = 0;ccount < strlen(end_str);ccount++)
					{
						wifibuf[ccount] = end_str[ccount];
					}
					wifibuf[strlen(end_str)]='\0';
					//sendtowifi(wifibuf,strlen(end_str));
					
					strcat(str,"DataBegin\r\n");
					strcat(str,"airTemperature=");	strcat(str,temp_buf1);	strcat(str,"\r\n");
					strcat(str,"sirHumidity=");		strcat(str,temp_buf2);	strcat(str,"\r\n");
					strcat(str,"airPM25=");			strcat(str,temp_buf3);	strcat(str,"\r\n");
					strcat(str,"airFormaldehyde=");	strcat(str,temp_buf4);	strcat(str,"\r\n");
					strcat(str,"DataEnd\r\n");
					sendtowifi(str,strlen(str));
					
					
					switch(disptype){
						case 0://显示温度
									SHT2x_Test();
									tm1637DisplayDecimal(g_sht2x_param.TEMP_HM*10,1);
									printf("温度:%2.1f\r\n",g_sht2x_param.TEMP_HM);
									
							break;
						case 1://显示湿度
									SHT2x_Test();
									tm1637DisplayDecimal(g_sht2x_param.HUMI_HM*10,1);
									printf("湿度:%2.1f%%\r\n",g_sht2x_param.HUMI_HM);
							break;
						case 2://显示PM2.5
									tm1637DisplayDecimal(PM25+30,0);
									printf("PM2.5:%2.2f \r\n",PM25);
								
							break;
						case 3://显示甲醛
									tm1637DisplayDecimal(HCHO*100/5+0.02,2);
									printf("甲醛:%2.2f/mg\r\n",HCHO);
							break;
						case 4://显示时间
									Time_Display(RTC_GetCounter());
									
							break;
					}
		}
	
}
/**
  * @file   main
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{

	SystemInit();
		RCC_Configuration();	
	//SysTick_Config(72000); 
	   SYSTICK_Init();
    USART_Config();
	   NVIC_Configuration();
    GPIO_Configuration();	
    //I2C_Configuration();
	SHT2x_Init();
	tm1637Init();
	I2C_EE_Init();
	BUZZ_ONOFF(1);
	delay_ms(500);
	BUZZ_ONOFF(0);
	disptype=4;

	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)	  //检查是否掉电重启
     {
			 delay_ms(5000);
       printf("\r\n\n 掉电重启");
     }
	Clock_ini();
	
    printf("  家用环境监测仪----jjw V1.0 \r\n");	
	
	if(readconfig()){
			printf("配置存在\r\n");
		  configflag=1;
				if(0){//手动修改配置
							strcpy(config.nettype,"net");
							strcpy(config.server,"192.168.1.105");
							config.serverip[0]=192;
							config.serverip[1]=168;
							config.serverip[2]=1;
							config.serverip[3]=105;
				}
			remote_ip[0]=config.serverip[0];
			remote_ip[1]=config.serverip[1];
			remote_ip[2]=config.serverip[2];
			remote_ip[3]=config.serverip[3];
			remote_port=atoi(config.port);
	}else{
			printf("配置不存在\r\n");
			configflag=0;
	}
		
    gpio_for_w5500_config();						/*初始化MCU相关引脚*/
    reset_w5500();											/*硬复位W5500*/
    set_w5500_mac();										/*配置MAC地址*/
    set_w5500_ip();											/*配置IP地址*/
    socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/
    while (1)
    {
			if(!strcmp(config.nettype,"net")) {
							if(!dhcp_ok){
										printf(" Wainting for DHCP\r\n");
										do_dhcp();
										delay_ms(500);
								}else{
										loopback_tcpc(SOCK_TCPC, remote_port); /*TCP_Client 数据回环测试程序*/
								}
							if(sockflag && socktime ){
									senddata=cJSON_CreateObject();
									cJSON_AddStringToObject(senddata, "id", config.id);
									cJSON_AddNumberToObject(senddata, "temp", g_sht2x_param.TEMP_HM);
									cJSON_AddNumberToObject(senddata, "humi", g_sht2x_param.HUMI_HM);
									cJSON_AddNumberToObject(senddata, "pm25", PM25);
									cJSON_AddNumberToObject(senddata, "pm25", HCHO);
									socktime=0;
									if (!senddata)
									{
											printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									}
									else
									{
											sendjson = cJSON_PrintUnformatted(senddata);
											printf("%s\n", sendjson);
											send(SOCK_TCPC,(const uint8 *)sendjson,strlen(sendjson));
											cJSON_Delete(senddata);
											free(sendjson);
									}
							}	
			}	
			if(!strcmp(config.nettype,"wifi")) {
							if(!wifi_conn_ok){
										printf(" Wainting for wifi conn\r\n");
										delay_ms(500);
								}else if(socktime) {
									senddata=cJSON_CreateObject();
									cJSON_AddStringToObject(senddata, "id", config.id);
									cJSON_AddNumberToObject(senddata, "temp", g_sht2x_param.TEMP_HM);
									cJSON_AddNumberToObject(senddata, "humi", g_sht2x_param.HUMI_HM);
									cJSON_AddNumberToObject(senddata, "pm25", PM25);
									cJSON_AddNumberToObject(senddata, "pm25", HCHO);
									socktime=0;
									if (!senddata)
									{
											printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									}
									else
									{
											sendjson = cJSON_PrintUnformatted(senddata);
											printf("%s %d\n", sendjson,strlen(sendjson));
											sendtowifi(sendjson,strlen(sendjson));
											cJSON_Delete(senddata);
											free(sendjson);
									}
							}	
					
			}
				display();
			
			
			
			 if(wififlag){//wifi download config
					wififlag=0;
					printf("%s",wifibuf);
					config_json = cJSON_Parse(wifibuf);
					if (!config_json)
					{
							printf("Cjson Error before: [%s]\n", cJSON_GetErrorPtr());
					}
					else
					{
						printf("解析Json ,Save config\r\n");
						saveconfig(config_json);
						cJSON_Delete(config_json);
						SystemReset();
					}					
			}
    }
}
uint8_t readconfig(){//w5500的配置是写入eeprom的地址0开始，共18字节， 系统配置从100H地址开始
	//I2C_ReadByte((uint8_t *)(&config),sizeof(config),0x20);//,EE_DEV_ADDR);
	//I2C_EE_BufferRead(ReadBuffer, EEP_Firstpage, 256);
	int size=sizeof(config);
	u8 *p=(u8 *)(&config);
	EEPROM_ADDRESS=0xa0;
	I2C_EE_BufferRead(p,0x0,256);
	EEPROM_ADDRESS=0xa2;
	I2C_EE_BufferRead(p+256,0x0,size%256);
	delay_ms(10);
	if(config.magic==0x5a){
			printf("%s\r\n",config.username);
			printf("%s\r\n",config.useraddr);
			printf("%s\r\n",config.userphone);
			printf("%s\r\n",config.id);
			printf("%s\r\n",config.mac);
			printf("%s\r\n",config.nettype);
			printf("%s\r\n",config.apssid);
			printf("%s\r\n",config.appassword);
			printf("%s\r\n",config.server);
			printf("%s\r\n",config.port);
		return 1;
	}else{
		return 0;
	}
}

uint8_t saveconfig(cJSON *c){
	uint8_t t;
	int size=sizeof(config);
	u8 *p=(u8 *)(&config);
	strcpy(config.username,cJSON_GetObjectItem(c,"username")->valuestring);
	strcpy(config.useraddr,cJSON_GetObjectItem(c,"useraddr")->valuestring);
	strcpy(config.userphone,cJSON_GetObjectItem(c,"userphone")->valuestring);
	strcpy(config.id,cJSON_GetObjectItem(c,"id")->valuestring);
	strcpy(config.mac,cJSON_GetObjectItem(c,"mac")->valuestring);
	strcpy(config.nettype,cJSON_GetObjectItem(c,"nettype")->valuestring);
	strcpy(config.apssid,cJSON_GetObjectItem(c,"apssid")->valuestring);
	strcpy(config.appassword,cJSON_GetObjectItem(c,"appassword")->valuestring);
	strcpy(config.server,cJSON_GetObjectItem(c,"server")->valuestring);
	strcpy(config.port,cJSON_GetObjectItem(c,"port")->valuestring);
						config.magic=0x5a;
						ip_json = cJSON_GetObjectItem(config_json,"ip");
						//printf("%s\r\n",cJSON_Print(ip_json));
						iSize = cJSON_GetArraySize(ip_json);
						for(iCnt = 0; iCnt < iSize; iCnt++)
						{
								pSub = cJSON_GetArrayItem(ip_json, iCnt);
								if(NULL == pSub)
								{
										continue;
								}
								config.serverip[iCnt]=atoi(pSub->valuestring);
								//printf("value[%2d] : [%d]\n", iCnt, config.serverip[iCnt]=atoi(pSub->valuestring));
						} 
						//cJSON_Delete(ip_json);		//此处不能删除，否则死机
	EEPROM_ADDRESS=0xa0;
	I2C_EE_BufferWrite(p, 0x0, 256);
	EEPROM_ADDRESS=0xa2;
	I2C_EE_BufferWrite(p+256,0x0,size%256);
	//I2C_EE_BufferWrite( WriteBuffer, EEP_Firstpage, 256);	 					//I2C_EE_PageWrite
	//delay_ms(10);	
	EEPROM_ADDRESS=0xa0;
	I2C_EE_BufferRead((uint8_t *)(&t), 0x0,1);
	if(t==0x5a){
		return 1;
	}else{
		return 0;
	}	
}

uint8_t distance(uint8_t start,uint8_t end){
	if(end>=start){
		return end-start;
	}else{
		return RLEN-start+end;
	}	
}
void sendtowifi(char *p,char size){
		char i;
	  for(i=0;i<size;i++){
			  USART_SendData(USART3,p[i]);
				while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
				//delay_ms(1);
		}
//		USART_SendData(USART3,'\r');
//		USART_SendData(USART3,'\n');
		
}
void send_read_cmd(void){
		char read_cmd[]={0xA5 ,0x5A ,0x02 ,0x80 ,0xAA};
		char i;
	  for(i=0;i<5;i++){
			  USART_SendData(USART2,read_cmd[i]);
				while (USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
		}
}


void RCC_Configuration(void)
{
   SystemInit(); 
   //RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO  , ENABLE);  
}

void GPIO_Configuration(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				     //PA0  STATUS 灯
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			 //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				     //PA8 BUZZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			 //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	         		 //PA1  Key 键盘
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    		 //??????
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //A?? 
RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA   , ENABLE);
}

void NVIC_Configuration(void)
{
  /*  ????*/
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  /* Configure one bit for preemption priority */
  /* ???? ?????????????,??????????   ????1, 7 */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  
  

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			     	//????1??
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	     	//????? 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				//?????0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//??
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			     	//????1??
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			     	//????1??
  NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/





