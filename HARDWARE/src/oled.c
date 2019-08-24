#include "oled.h"
#include "oledconf.h"

u16 BACK_COLOR;		//背景色

/*
*函数功能:LCD串行写入
*函数参数:byte 要写入的串行数据
*返回值:无
*/
static void LCD_WR_Bus(u8 byte)
{
    u8 i = 0;
    for(i = 0;i < 8;i ++)
    {
        OLED_SCLK_Clr();
        if(byte & 0x80)
        {
            OLED_SDIN_Set();
        }
        else
        {
            OLED_SDIN_Clr();
        }
        OLED_SCLK_Set();
        byte <<= 1;
    }
}

/*
*函数功能:lcd写入8bit位数据
*函数参数:dat 要写入的数据
*函数返回值:无
*/
static void LCD_WR_DAT8(u8 dat)
{
    OLED_DC_Set();
    LCD_WR_Bus(dat);
}

/*
*函数功能:lcd写入16bit位数据
*函数参数:dat 要写入的数据
*函数返回值:无
*/
static void LCD_WR_DAT(u16 dat)
{
    OLED_DC_Set();
    LCD_WR_Bus(dat >> 8);
    LCD_WR_Bus(dat);
}

/*
*函数功能:LCD写指令
*函数参数:reg 指令
*函数返回值:无
*/
static void LCD_WR_REG(u8 reg)
{
    OLED_DC_Clr();
    LCD_WR_Bus(reg);
}

/*
*函数功能:对应的LCD屏幕引脚初始化
*函数参数:无
*函数返回值:无
*/
static void OLED_GPIO_Init(void)
{
    RCC->AHB1ENR |= 1 << 3;
    RCC->AHB1ENR |= 1 << 6;
    RCC->AHB1ENR |= 1 << 5;
    
    GPIO_Set(GPIOD,PIN4 | PIN5 | PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
    PDout(4) = 1;
    PDout(5) = 1;
    PDout(15) = 1;
    
    GPIO_Set(GPIOG,PIN12,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
    PGout(12) = 1;
    GPIO_Set(GPIOF,PIN10 | PIN8,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
    //PFout(8) = 1;
    PFout(10) = 1;
}

/*
*函数功能:LCD初始化
*函数参数:无
*函数返回值:无
*/
void LCD_Init(void)
{
    OLED_GPIO_Init();
 
    OLED_RES_Clr();
	delay_ms(200);
	OLED_RES_Set();
	delay_ms(200);
	OLED_BLK_Set();
	delay_ms(200);
    
    /*开始初始序列*/
    LCD_WR_REG(0x36);
    if(USE_HORIZONTAL == 0)LCD_WR_DAT8(0x00);
    else if(USE_HORIZONTAL == 1)LCD_WR_DAT8(0xC0);
    else if(USE_HORIZONTAL == 2)LCD_WR_DAT8(0x70);
    else LCD_WR_DAT8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DAT8(0x05);
    
    LCD_WR_REG(0xB2);     //门廊设置
    LCD_WR_DAT8(0x0C);
    LCD_WR_DAT8(0x0C);
    LCD_WR_DAT8(0x00);
    LCD_WR_DAT8(0x33);
    LCD_WR_DAT8(0x33);
    
    LCD_WR_REG(0xB7);     //选通控制
    LCD_WR_DAT8(0x35);
    
    LCD_WR_REG(0xBB);     //VCOM背景
    LCD_WR_DAT8(0x19);
        
    LCD_WR_REG(0xC0);     //LMC控制
    LCD_WR_DAT8(0x2C);
    
    LCD_WR_REG(0xC2);     //VDA VRH启用
    LCD_WR_DAT8(0x01);
    
    LCD_WR_REG(0xC3);     //VRH设置
    LCD_WR_DAT8(0x12);
   
    LCD_WR_REG(0xC4);
    LCD_WR_DAT8(0x20);
  
    LCD_WR_REG(0xC6);     //正常模式下帧率控制
    LCD_WR_DAT8(0x0F);
    
    LCD_WR_REG(0xD0);
    LCD_WR_DAT8(0xA4);
    LCD_WR_DAT8(0xA1);
    
    LCD_WR_REG(0xE0);     //伽马电压
    LCD_WR_DAT8(0xD0);
    LCD_WR_DAT8(0x04);
    LCD_WR_DAT8(0x0D);
    LCD_WR_DAT8(0x11);
    LCD_WR_DAT8(0x13);
    LCD_WR_DAT8(0x2B);
    LCD_WR_DAT8(0x3F);
    LCD_WR_DAT8(0x54);
    LCD_WR_DAT8(0x4C);
    LCD_WR_DAT8(0x18);
    LCD_WR_DAT8(0x0D);
    LCD_WR_DAT8(0x0B);   
    LCD_WR_DAT8(0x1F);
    LCD_WR_DAT8(0x23);
    
    LCD_WR_REG(0xE1);
    LCD_WR_DAT8(0xD0);
    LCD_WR_DAT8(0x04);
    LCD_WR_DAT8(0x0C);
    LCD_WR_DAT8(0x11);
    LCD_WR_DAT8(0x13);
    LCD_WR_DAT8(0x2C);
    LCD_WR_DAT8(0x3F);
    LCD_WR_DAT8(0x44);
    LCD_WR_DAT8(0x51);
    LCD_WR_DAT8(0x2F);
    LCD_WR_DAT8(0x1F);
    LCD_WR_DAT8(0x1F);
    LCD_WR_DAT8(0x20);
    LCD_WR_DAT8(0x23);
    
    LCD_WR_REG(0x21);
    LCD_WR_REG(0x11);
    
//    delay_ms(120);
    LCD_WR_REG(0x29);
}

/*
*函数功能:设置光标地址
*函数参数:u16 xs	 ,u16 ys 设置行的起始和结束地址
*		  u16 xe,u16 ye	 设置列的起始和结束地址
*函数返回值:无
*/
void LCD_Set_Cur(u16 xs,u16 ys,u16 xe,u16 ye)
{
    if(USE_HORIZONTAL == 0)
    {
        LCD_WR_REG(0x2A);     //列地址集
        LCD_WR_DAT(xs);
        LCD_WR_DAT(xe);
        LCD_WR_REG(0x2B);
        LCD_WR_DAT(ys);
        LCD_WR_DAT(ye);
        LCD_WR_REG(0x2C);
    }
    else if(USE_HORIZONTAL == 1)
    {
        LCD_WR_REG(0x2A);     //列地址集
        LCD_WR_DAT(xs);
        LCD_WR_DAT(xe);
        LCD_WR_REG(0x2B);
        LCD_WR_DAT(ys + 80);
        LCD_WR_DAT(ye + 80);
        LCD_WR_REG(0x2C);
    }
    else if(USE_HORIZONTAL == 2)
    {
        LCD_WR_REG(0x2A);     //列地址集
        LCD_WR_DAT(xs);
        LCD_WR_DAT(xe);
        LCD_WR_REG(0x2B);
        LCD_WR_DAT(ys);
        LCD_WR_DAT(ye);
        LCD_WR_REG(0x2C);
    }
    else
    {
        LCD_WR_REG(0x2A);     //列地址集
        LCD_WR_DAT(xs + 80);
        LCD_WR_DAT(xe + 80);
        LCD_WR_REG(0x2B);
        LCD_WR_DAT(ys);
        LCD_WR_DAT(ye);
        LCD_WR_REG(0x2C);
    }
}

/*
*函数功能:在LCD指定位置画一个小点
*函数参数:u16 x,u16 y		光标位置
*		  u16 color		点的颜色
*函数返回值:无
*/
void LCD_Draw_Ponit(u16 x,u16 y,u16 color)
{
    LCD_Set_Cur(x,y,x,y);   //设置光标地址
    LCD_WR_DAT(color);
}

/*
*函数功能:在LCD指定位置画一个大点
*函数参数:u16 x,u16 y	光标位置
*		  u16 color	  点的颜色
*函数返回值:无
*/
void LCD_Draw_big_Ponit(u16 x,u16 y,u16 color)
{
    u16 i,j;
    LCD_Set_Cur(x-1,y-1,x+1,y+1);
    for(i = y-1;i < y+1;i ++)
    {
        for(j = x-1;j < x+1;j ++) LCD_WR_DAT(color);
    } 
}

/*
*函数功能:LCD清屏
*函数参数:u16 color  清屏后屏幕的颜色
*函数返回值:无
*/
void LCD_Clear(u16 color)
{
    u16 i,j;
    LCD_Set_Cur(0,0,LCD_W-1,LCD_H-1);
    for(i = 0;i < LCD_W;i ++)
    {
        for(j = 0;j < LCD_H;j ++)
        {
            LCD_WR_DAT(color);
        }
    }
}

/*
*函数功能:在LCD上面画一条直线
*函数参数:u16 x1,u16 y1	起始行地址
*		  u16 x2,u16 y2	结束行地址
*		  u16 color		线的颜色
函数返回值:无
*/
void LCD_Draw_Line(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t;
	int xerr = 0,yerr = 0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x = x2 - x1;
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if(delta_x > 0) incx = 1;
	else if(delta_x == 0) incx = 0;
	else {incx = -1;delta_x = -delta_x;}
	if(delta_y > 0) incy = 1;
	else if(delta_y == 0) incy = 0;
	else {incy = -1;delta_y = -delta_y;}

	if(delta_x > delta_y) distance = delta_x;
	else distance = delta_y;

	for(t = 0;t < distance+1;t++)
	{
		LCD_Draw_Ponit(uRow, uCol, color);
		xerr += delta_x;
		yerr += delta_y;
		if(xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if(yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/*
*函数功能:LCD显示汉字
*函数参数:u16 x,u16 y	汉字显示的位置
*		  u8 index 	  汉字的序号
*		  u8 size	  汉字的大小
*		  u16 color	  汉字的颜色
*函数返回值:无
*/
void LCD_Show_Chinese(u16 x,u16 y,u8 index,u8 size,u16 color)
{
	u8 i,j;
	u8 *temp,size1;
	if(size == 16) {temp = Hzk16;}
	LCD_Set_Cur(x, y, x+size-1, y+size-1);
	size1 = size*size/8;
	temp += index*size1;
	for(j = 0;j < size1;j++)
	{
		for(i = 0;i < 8;i++)
		{
			if((*temp & (1 << i)) != 0)
			{
				LCD_WR_DAT(color);
			}
			else
			{
				LCD_WR_DAT(BACK_COLOR);
			}
		}
		temp++;
	}
}

/*
*函数功能:在指定区域填充指定颜色
*函数参数:u16 xsta,u16 ysta			填充起始地址(对角线)
*		  u16 xend,u16 yend		填充结束地址(对角线)
*		  u16 color				填充的颜色
*函数返回值:无
*/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
	u16 i,j;
	LCD_Set_Cur(xsta, ysta, xend, yend);
	for(i = ysta;i <= yend;i++)
	{
		for(j = xsta;j <= xend;j++)
			LCD_WR_DAT(color);
	}
}

/*
*函数功能:在LCD上面画一个矩形
*函数参数:u16 x1,u16 y1,u16 x2,u16 y2		矩形的位置
*		  u16 color		矩形边框的颜色
*函数返回值:无
*/
void LCD_Draw_Rectangle(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	LCD_Draw_Line(x1, y1, x2, y1, color);
	LCD_Draw_Line(x1, y1, x1, y2, color);
	LCD_Draw_Line(x1, y2, x2, y2, color);
	LCD_Draw_Line(x2, y1, x2, y2, color);
}

/*
*函数功能:LCD上面显示字符
*函数参数:u16 x,u16 y		显示位置
*		  u8 num		要显示的字符
*显示模式:u8 mode			0不重叠显示
*						1重叠显示
*		  u16 color		字符的颜色
*函数返回值:无
*/
void LCD_Show_Char(u16 x,u16 y,u8 num,u8 mode,u16 color)
{
	u8 temp;
	u8 pos,t;
	u16 x0 = x;
	if(x > LCD_W-16 || y > LCD_H-16)return;
	num = num - ' ';
	LCD_Set_Cur(x, y, x + 8-1, y + 16-1);
	if(!mode)
	{
		for(pos = 0;pos < 16;pos++)
		{
			temp = asc2_1608[(u16)num*16 + pos];
			for(t = 0;t < 8;t++)
			{
				if(temp & 0x01) LCD_WR_DAT(color);
				else LCD_WR_DAT(BACK_COLOR);
				temp >>= 1;
				x++;
			}
			x = x0;
			y++;
		}
	}
	else
	{
        temp = asc2_1608[(uint16_t)num*16 + pos];
		for(pos = 0;pos < 16;pos++)
		{
			if(temp & 0x01) LCD_Draw_Ponit(x+t, y+pos, color);
			temp >>= 1;
		}
	}
}

/*
*函数功能:LCD显示字符串
*函数参数:u16 x,u16 y		字符串显示起始地址
*		  const u8 *p	要显示字符串的首地址
*		  u16 color		显示的颜色
*函数返回值:无
*/
void LCD_Show_Str(u16 x,u16 y,const u8 *p,u16 color)
{
	while (*p != '\0')
	{
		if(x > LCD_W-16) {x = 0;y += 16;}
		if(y > LCD_H-16) {y = x = 0;LCD_Clear(RED);}
		LCD_Show_Char(x, y, *p, 0, color);
		x += 8;
		p++;
	}
}

/*
*函数功能:将小数整数化
*函数参数:u8 m   底数
*		  u8 n指数
*函数返回值:去掉小数点后的数
*/
u32 mypow(u8 m,u8 n)
{
	u32 result = 1;
	while (n--) result *= m;
	return result;
}

/*
*函数功能:在LCD上面显示数字
*函数参数:u16 x,u16 y			要显示数字的位置
*	      u16 num			要显示的数字
*		  u8 len			要显示的长度
*		  u16 color			显示数字的颜色
*函数返回值:无
*/
void LCD_Show_Num(u16 x,u16 y,u16 num,u8 len,u16 color)
{
	u8 t,temp;
	u8 enshow = 0;
	for(t = 0;t < len;t++)
	{
		temp = (num / mypow(10, len-t-1))%10;
		if(enshow == 0 && t < (len - 1))
		{
			if(temp == 0)
			{
				LCD_Show_Char(x + 8*t, y, ' ', 0, color);
				continue;
			}
			else
			{
				enshow = 1;	
			}
		}
		LCD_Show_Char(x + 8*t, y, temp + 48, 0, color);
	}
}

/*
*函数功能:LCD显示小数
*函数参数:u16 x,u16 y		数字显示的位置
*		  float num		要显示的数组
*		  u8 len		要显示的数字的个数
*		  u16 color		显示数字的颜色
*函数返回值:无
*/
void LCD_Show_FNum(u16 x,u16 y,float num,u8 len,u16 color)
{
	u8 t,temp;
	u16 num1;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_Show_Char(x+8*(len-2),y,'.',0,color);
			t++;
			len+=1;
		}
	 	LCD_Show_Char(x+8*t,y,temp+48,0,color);
	}
}
/*
*函数功能:显示光标
*函数参数:u16 x,u16 y  光标显示位置
*函数返回值:无
*/
void LCD_Show_Cur(u16 x,u16 y)
{
    LCD_Draw_Line(x, y, x, y+16, BLACK);
}

/*
*函数功能:关闭光标
*函数参数:u16 x,u16 y  光标显示位置
*函数返回值:无
*/
void LCD_Close_Cur(u16 x,u16 y)
{
    LCD_Draw_Line(x, y, x, y+16, BACK_COLOR);
}

/*在OLED上面显示刻度*/
void LCD_Show_Scale(uint16_t x,uint16_t y)
{
    LCD_Draw_Line(x,y,x+3,y,BLUE);
}

/*在OLED上面显示速度波形*/
void LCD_Show_Speed(void)
{
	static uint16_t counter = 29;		//起点,不确定是否会出现闪烁现象,暂时保留
    int num = 0;

	if(counter <= 239)
	{
        num = (int)pid.ActualSpeed;
        if(num < 0) num = -num;  
   		LCD_Draw_Ponit(counter,135 - num,BLUE);
		counter++;
	}
	else
	{
		LCD_Fill(29, 1, 239, 134, BACK_COLOR);
		counter = 29;					//一次显示到头返回重新显示
	}
}
/*OLED显示实际角度波形*/
void LCD_Show_angle(void)
{
	int angle_buff = 0;
	static uint16_t angle_counter = 29;

	angle_buff = ActuallAngle;
    
    if(angle_buff < 0)
        angle_buff = -angle_buff;

	angle_buff /= 3;

	if(angle_counter < 239)
	{
		LCD_Draw_Ponit(angle_counter, 135 - angle_buff, BLUE);
        angle_counter++;
	}
	else 
	{
		LCD_Fill(29, 1, 239, 134, BACK_COLOR);
		angle_counter = 29;
	}
}
/*更换刻度标识数前先清除之前内容*/
void clean_before(void)
{
	LCD_Fill(1, 6, 16, 139, BACK_COLOR);
}
/*显示角度对应的内容*/
void angle_thing(void)
{
	my_printf(1,"%d",16,124,0,1,GRAY);
	my_printf(1,"%d",8,107,60,2,GRAY);
	my_printf(1,"%d",1,87,120,3,GRAY);
	my_printf(1,"%d",1,67,180,3,GRAY);
	my_printf(1,"%d",1,47,240,3,GRAY);
	my_printf(1,"%d",1,27,300,3,GRAY);
	my_printf(1,"%d",1,7,360,3,GRAY);	 /*显示角度刻度值*/
}
/*显示速度对应坐标的内容*/
void speed_thing(void)
{
	my_printf(1,"%d",16,124,0,1,GRAY);
	my_printf(1,"%d",8,107,20,2,GRAY);
	my_printf(1,"%d",8,87,40,2,GRAY);
	my_printf(1,"%d",8,67,60,2,GRAY);
	my_printf(1,"%d",8,47,80,2,GRAY);
	my_printf(1,"%d",1,27,100,3,GRAY);
	my_printf(1,"%d",1,7,120,3,GRAY);	 /*显示速度刻度值*/
}

/*基础界面显示*/
void interface_job(void)
{
	LCD_Show_Chinese(10,140,0,16,BLUE);
	LCD_Show_Chinese(26,140,1,16,BLUE);
	LCD_Show_Chinese(42,140,2,16,BLUE);
	LCD_Show_Chinese(58,140,3,16,BLUE); 		//显示"实际速度"

	LCD_Show_Chinese(10,156,4,16,BLUE);
	LCD_Show_Chinese(26,156,5,16,BLUE);
	LCD_Show_Chinese(42,156,6,16,BLUE);
	LCD_Show_Chinese(58,156,3,16,BLUE); 		//显示"设定速度"

	my_printf(1,"%s",10,172,"Kp",RED);
	my_printf(1,"%s",10,192,"Ki",RED);
	my_printf(1,"%s",10,212,"Kd",RED);			//显示"Kp,Ki,Kd"
 
	LCD_Show_Chinese(100,180,0,16,BLUE);
	LCD_Show_Chinese(116,180,1,16,BLUE);
	LCD_Show_Chinese(132,180,7,16,BLUE);
	LCD_Show_Chinese(148,180,3,16,BLUE);		//显示"实际角度"
	  
	LCD_Show_Chinese(100,196,4,16,BLUE);
	LCD_Show_Chinese(116,196,5,16,BLUE);
	LCD_Show_Chinese(132,196,7,16,BLUE);
	LCD_Show_Chinese(148,196,3,16,BLUE);		//显示"设定角度"
  
	LCD_Draw_Line(26,135,239,135,BLACK);
	LCD_Draw_Line(26,1,26,135,BLACK);		   //画坐标轴

	LCD_Show_Scale(27,115);
	LCD_Show_Scale(27,95);
	LCD_Show_Scale(27,75);
	LCD_Show_Scale(27,55);
	LCD_Show_Scale(27,35);
	LCD_Show_Scale(27,15);				/*显示刻度*/
}



