#ifndef __OLED_H__
#define __OLED_H__
#include "sys.h"
#include <stdlib.h>
#include "delay.h"
#include "pid.h"
#include "my_printf.h"

#define USE_HORIZONTAL 0            //设置横屏或者竖屏显示,0,1为竖屏,2,3为横屏

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 240
#define LCD_H 240

#else
#define LCD_W 240
#define LCD_H 240
#endif /*USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1*/

#define OLED_SCLK_Clr()     PGout(12) = 0;//时钟引脚拉低
#define OLED_SCLK_Set()     PGout(12) = 1;//时钟引脚拉高

#define OLED_SDIN_Clr()     PDout(5) = 0; //数据输入引脚拉低,即数据输入0
#define OLED_SDIN_Set()     PDout(5) = 1;  //数据输入引脚拉高,即数据输入1

#define OLED_RES_Clr()      PDout(4) = 0;  //复位引脚拉低
#define OLED_RES_Set()      PDout(4) = 1;  //引脚置位

#define OLED_DC_Clr()       PDout(15) = 0;
#define OLED_DC_Set()       PDout(15) = 1;

#define OLED_BLK_Clr()      PDout(6) = 0;
#define OLED_BLK_Set()      PDout(6) = 1;

#define OLED_CMD    0   //写命令
#define OLED_DAT    1   //写数据

extern  u16 BACK_COLOR; //设置背景色

void LCD_Init(void);
void LCD_Clear(u16 color);
void LCD_Draw_Ponit(u16 x,u16 y,u16 color);
void LCD_Set_Cur(u16 xs,u16 ys,u16 xe,u16 ye);
void LCD_Draw_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_Draw_big_Ponit(u16 x,u16 y,u16 color);
void LCD_Show_Chinese(u16 x,u16 y,u8 index,u8 size,u16 color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_Draw_Rectangle(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void LCD_Show_Char(u16 x,u16 y,u8 num,u8 mode,u16 color);
void LCD_Show_Str(u16 x,u16 y,const u8 *p,u16 color);
void LCD_Show_Num(u16 x,u16 y,u16 num,u8 len,u16 color);
void LCD_Show_FNum(u16 x,u16 y,float num,u8 len,u16 color);
void LCD_Show_Cur(u16 x,u16 y);
void LCD_Close_Cur(u16 x,u16 y);
void LCD_Show_Scale(uint16_t x,uint16_t y);
void LCD_Show_Speed(void);
void speed_thing(void);
void interface_job(void);
void angle_thing(void);
void clean_before(void);
void LCD_Show_angle(void);


extern u16 BACK_COLOR;  //背景色

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


#endif /* __OLED_H__ */

