/*数字8个像素点*/
/*汉字十六个像素点*/

#ifndef __GLOB_H__
#define __GLOB_H__

/*Kp,Ki,Kd  数字开始位置30      速度设定开始位置 80          角度设定开始位置   170*/
#include "sys.h"
#include "delay.h" 
#include "oled.h"
#include <stdint.h>
#include "usart.h"
#include "motor.h"
#include "my_printf.h"
#include "key.h"
#include "pid.h"
#include "eeprom.h"

void eerpom_fist_use(void);
void Every_Init(void);

#endif	/*__GLOB_H__*/


