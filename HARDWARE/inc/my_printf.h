#ifndef __MY_PRINTF_H__
#define __MY_PRINTF_H__

#include "usart.h"
#include "oled.h"

uint16_t my_printf(uint8_t mode,uint8_t *formate,...);
/****************************对my_printf说明*********************************************/
/*
*第一个参数mode对应于输出位置不同 0 面对上位机串口输出 可以指定精度宽度对于小数默认输出小数点后6位
								  1 面对OLED显示屏输出

*格式化参数类似于C库中的printf函数的使用方法

*注意:
	1)当对OLED显示屏输出时每次只能输出一个单元,并且有参数要求
	2)对于%o %x %X 参数依次为x坐标 y坐标 要输出数据 输出颜色
	  对于%d %u %f   参数依次为x坐标 y坐标  要输出数据 输出长度 输出颜色
	  对于%c       参数依次为x坐标 y坐标 要输出数据 是否使用叠加模式(1叠加 0 不叠加) 输出颜色
	  对于%s     参数依次为x坐标,y坐标  输出数据首地址 输出颜色
	  
*/


#endif /*__MY_PRINTF_H__*/



