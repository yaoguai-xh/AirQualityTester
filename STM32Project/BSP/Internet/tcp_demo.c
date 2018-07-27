/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP 演示函数
* @attention  
************************************************************************************************
**/

#include <stdio.h>
#include <string.h>
#include "tcp_demo.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"

#define tick_second 1
uint8 buff[2048];				                              	         /*定义一个2KB的缓存*/

/**
*@brief		TCP Server回环演示函数。
*@param		无
*@return	无
*/
void loopback_tcps(SOCKET s, uint16 port)
{
	uint16 len=0;  
	switch(getSn_SR(s))											            	/*获取socket的状态*/
	{
		case SOCK_CLOSED:													                  /*socket处于关闭状态*/
			socket(s ,Sn_MR_TCP,port,Sn_MR_ND);	        /*打开socket*/
		  break;     
    
		case SOCK_INIT:														                  /*socket已初始化状态*/
			listen(s);												                /*socket建立监听*/
		  break;
		
		case SOCK_ESTABLISHED:												              /*socket处于连接建立状态*/
		
			if(getSn_IR(s) & Sn_IR_CON)
			{
				setSn_IR(s, Sn_IR_CON);								          /*清除接收中断标志位*/
			}
			len=getSn_RX_RSR(s);									            /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(s,buff,len);								              	/*接收来自Client的数据*/
				buff[len]=0x00; 											                  /*添加字符串结束符*/
				printf("%s\r\n",buff);
				send(s,buff,len);									              /*向Client发送数据*/
		  }
		  break;
		
		case SOCK_CLOSE_WAIT:												                /*socket处于等待关闭状态*/
			close(s);
		  break;
	}
}


/**
*@brief		TCP client回环演示函数。
*@param		无
*@return	无
*/

u8 sockflag;
u8 conndisp;
void loopback_tcpc(SOCKET s, uint16 port)
{

   uint16 len=0;	

	switch(getSn_SR(s))						   		  				         /*获取socket的状态*/
	{
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
			printf("Sock close,recreat\r\n");
			socket(s,Sn_MR_TCP,local_port++,Sn_MR_ND);
			sockflag=0;
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
			printf("Sock conn\r\n");
			connect(s,remote_ip,port);                /*socket连接服务器*/ 
			conndisp=1;
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
			sockflag=1;
			if(sockflag && conndisp){
					conndisp=0;
					printf("Sock conn success\r\n");
			}
			if(getSn_IR(s) & Sn_IR_CON)
			{
				setSn_IR(s, Sn_IR_CON); 							         /*清除接收中断标志位*/
			}
		
			len=getSn_RX_RSR(s); 								  	         /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(s,buff,len); 							   		         /*接收来自Server的数据*/
				buff[len]=0x00;  											                 /*添加字符串结束符*/
				printf("Sock REC-%s\r\n",buff);
				send(s,buff,len);								     	         /*向Server发送数据*/
			}		  
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
			printf("Sock colse wat\r\n");
			close(s);
		  break;
	}	

}





