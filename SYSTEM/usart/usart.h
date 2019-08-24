#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "stdio.h"	  
#include "eeprom.h"

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void uart_init(u32 pclk2,u32 bound); 
uint8_t My_Putc(uint8_t ch);
void TIM3_Int_Init(u16 arr,u16 psc);
void turn_com_stm(void);
void usart_job(void);

#endif	   
















