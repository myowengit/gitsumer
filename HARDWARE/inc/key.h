/*
*说明:按键排列顺序,PF0 ~ PF3 对应于行
*				   PF4 ~ PF7 对应于列				
*/

#ifndef __KEY_H
#define __KEY_H	 
#include <stm32f4xx.h>
#include "sys.h" 
#include "oled.h"
#include "my_printf.h"
#include "pid.h"
#include "eeprom.h"
#include "motor.h"

typedef struct 
{
    uint16_t value;
    uint16_t status;
	uint16_t key_used_value;
}key;

extern key key_t;
extern uint8_t set_angle_falg;		//设置指定角度标志

/********************外部单次使用****************************/
void KEY_Init(void);
void Motor_IO_Init(void);			//初始化电机控制转向IO
void KeyScan(void);
void Key_Action(void);
/*************************************************/

/******************内部函数使用************************/
void set_mand_Variable(void);

/********************************************************/

/************************外部多次使用**********************************/
/*将存储在缓冲区的数据转换成数值,在串口改变设定值时也会使用,在读取eeprom赋值时也需使用*/
double turn_to_num(uint8_t *p_buf);
void supplement_fun(uint8_t len,uint8_t *p_buf);
void clean_array(uint8_t *p_buf);


#endif




