#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "motor.h"

uint8_t buff[8] = {0};		//存储获得的参数变量的字符串形式
uint8_t rx_counter = 0;				//usart_rx_buf计数参数值


/*
*函数功能:串口接收转换功能
*函数参数:无
*/
void usart_job(void)
{
	if(USART_RX_STA & 0x8000)
	{
		 turn_com_stm();
	}
}
/****************************串口获取的数据的转换准备工作********************************************/
/*
*函数功能:将串口发送的一连串数据进行解析分出对应于不同变量的不同参数
*函数返回值:从接收数组里面获取的
*/
static uint8_t analysis_buf(uint8_t *r_buf,uint8_t *p_buf)
{
	uint8_t counter = 0;					//截取个数计数
	
	while((*p_buf >= '0' && *p_buf <= '9') || (*p_buf == '.'))		//没有检测到标志结束的字母
	{
		if(counter < 7)
			*r_buf++ = *p_buf++;						//最多从串口读取7个数据
        else
            p_buf++;
		counter++;        //计数值增加
	}
	return counter;
}

/*将串口获得的数据转换到速度设定变量*/
static uint8_t speed_get(void)
{	
	uint8_t counter = 0,j;
	
	counter = analysis_buf(buff, &USART_RX_BUF[rx_counter]);		//对接收到的数据进行截取处理
	if(buff[6] != '\0') buff[6] = '\0';				//速度设定宽度为6	
	set_speed = turn_to_num(buff);					//将截取到的数据处理成对应数值赋值给对应的参数
    supplement_fun(6,buff);
	for(j = 0;j < 6;j++) my_printf(1, "%c",80+j*9,156,buff[j],0,RED);
    eeprom_write(10,buff,6);
	clean_array(buff);								//进行对接收数组的清空操作
	return counter;
}
/*将串口获得的数据转换到角度设定变量*/
static uint8_t angle_get(void)
{
	uint8_t counter = 0,j;
	
	counter = analysis_buf(buff, &USART_RX_BUF[rx_counter]);		//对接收到的数据进行截取处理
	if(buff[6] != '\0') buff[6] = '\0';				//角度设定宽度为6	
	set_angle = turn_to_num(buff);					//将截取到的数据处理成对应数值赋值给对应的参数
    supplement_fun(6,buff);
	for(j = 0;j < 6;j++) my_printf(1, "%c",170+j*9,196,buff[j],0,RED);
    eeprom_write(20,buff,6);
	clean_array(buff);								//进行对接收数组的清空操作
	return counter;	
}

/*将串口获得的数据转换到Kp设定变量*/
static uint8_t Kp_get(void)
{
	uint8_t counter = 0,j;
	
	counter = analysis_buf(buff, &USART_RX_BUF[rx_counter]);		//对接收到的数据进行截取处理
	pid.Kp = turn_to_num(buff);					//将截取到的数据处理成对应数值赋值给对应的参数
    supplement_fun(7,buff);
	for(j = 0;j < 7;j++) my_printf(1, "%c",30+j*9,172,buff[j],0,RED);
    eeprom_write(30,buff,7);
	clean_array(buff);								//进行对接收数组的清空操作
	return counter;	
}

/*将串口获得的数据转换到Ki设定变量*/
static uint8_t Ki_get(void)
{
	uint8_t counter = 0,j;
	
	counter = analysis_buf(buff, &USART_RX_BUF[rx_counter]);		//对接收到的数据进行截取处理
	pid.Ki = turn_to_num(buff);					//将截取到的数据处理成对应数值赋值给对应的参数
    supplement_fun(7,buff);
	for(j = 0;j < 7;j++) my_printf(1, "%c",30+j*9,192,buff[j],0,RED);
    eeprom_write(40,buff,7);
	clean_array(buff);								//进行对接收数组的清空操作
	return counter;
}
/*将串口获得的数据转换到Kd设定变量*/
static uint8_t Kd_get(void)
{
	uint8_t counter = 0,j;
	
	counter = analysis_buf(buff, &USART_RX_BUF[rx_counter]);		//对接收到的数据进行截取处理
	pid.Kd = turn_to_num(buff);					//将截取到的数据处理成对应数值赋值给对应的参数
    supplement_fun(7,buff);
	for(j = 0;j < 7;j++) my_printf(1, "%c",30+j*9,212,buff[j],0,RED);
    eeprom_write(50,buff,7);
	clean_array(buff);								//进行对接收数组的清空操作
	return counter;
}
/********************************************结束**********************************************************/
/*将串口获取的参数值赋予对应的变量*/
void turn_com_stm(void)
{
	while(USART_RX_BUF[rx_counter] != '\0')		//判断是否接收到ASART_RX_BUF的最后一位
	{
		switch(USART_RX_BUF[rx_counter])
		{
			case 'S':rx_counter += 1;rx_counter += speed_get();break;				//执行设定速度获取
			case 'A':rx_counter += 1;rx_counter+= angle_get();break;				//知行设定角度获取
			case 'P':rx_counter += 1;rx_counter += Kp_get();break;					//执行设定Kp获取
			case 'I':rx_counter += 1;rx_counter += Ki_get();break;					//执行设定Ki获取
			case 'D':rx_counter += 1;rx_counter += Kd_get();break;					//执行设定Kd获取
            default :rx_counter++;
		}
	}
	rx_counter = 0;	    //计数值清空
	clean_array(USART_RX_BUF);			//串口接收缓冲区清空
	grid_counter = 0;
    USART_RX_STA = 0;                   //标志清零
}

uint8_t My_Putc(uint8_t ch)
{
    while((USART1->SR & 0x40) == 0);
    USART1->DR = ch;
    return ch;
}
//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
        KeyScan();
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器	  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//抢占1，子优先级3，组2									 
}

    
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   

u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目

u16 USART_RX_STA=0;       //接收状态标记	  
  
void USART1_IRQHandler(void)
{
	u8 res;	
	if(USART1->SR&(1<<5))//接收到数据
	{	 
		res=USART1->DR; 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}
	} 
} 
#endif	

//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB2ENR|=1<<4;  	//使能串口1时钟 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//波特率设置
 	USART1->BRR=mantissa; 	//波特率设置	 
	USART1->CR1&=~(1<<15); 	//设置OVER8=0 
	USART1->CR1|=1<<3;  	//串口发送使能 
#if EN_USART1_RX		  	//如果使能了接收
	//使能接收中断 
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,2,USART1_IRQn,2);//组2， 
#endif
	USART1->CR1|=1<<13;  	//串口使能
}


