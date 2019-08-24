#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_printf.h"

uint8_t if_point_flag = 0;		//记录'.'个数的变量
uint8_t clean_acc_flag = 0;		//是否清除对应静态原始精度值的标志
uint8_t clean_wid_flag = 0;		//是否清除对应宽度变量值的标志
uint8_t wearing_flag = 0;		//输入限制性输出字符有误标志
uint8_t if_do_acc_flag = 0;     //是否有精度限制输入
	
/*执行数组翻转操作*/
void str_up_down(uint8_t *p_buf)
{
    uint8_t str_buf[8] = {0};
    char counter = 0;
    
    while(*p_buf != '\0')
    {
        str_buf[counter] = *p_buf;
        counter++;
        p_buf++;
    }
    p_buf -= counter;
    while(counter != 0)
    {
        *p_buf++ = str_buf[counter-1];
        counter--;
    }
}

/*执行数字部分,数组化*/
uint8_t integer_str(long long num,uint8_t *p_buf)
{
    uint8_t counter = 0;
    while(num / 1 != 0)
    {
        *p_buf++ = num % 10 + '0';
        num /= 10;
        counter++;
    }
    return counter;
}
/*输出转化完成的数字*/
void My_Putnum(uint8_t *p_buff)
{
    while(*p_buff != '\0')
        My_Putc(*p_buff++);
}

/*数字进制转换,num要转换的十进制数字,system 转换到那种进制的形式,缓存转换数据的首地址*/
uint8_t system_change(int num,uint8_t system,uint8_t *p_buff)
{
    uint8_t counter = 0;
    uint8_t mand = 0;
    
    //判断数字正负
    if(num < 0)
    {
        My_Putc('-');           //小于0输出一个'-'号
        num = -num;
    }
    while(num / 1 != 0)         //没有到最高位,数字转化
    {
        mand = num % system + '0';  //转化为字符
        p_buff[counter] = mand;
        num /= system;
        if(p_buff[counter] > '9')
            p_buff[counter] = p_buff[counter] - '9' + 'a';
        counter++;
    }
    return counter;               //返回实际数字转化后的位数
}
/*检测类型符号,非定义类型返回0*/
uint8_t type_chack(uint8_t ch)
{
	switch(ch)
	{
		case 'd':return 1;
		case 'o':return 2;
		case 'u':return 3;
		case 'x':return 4;
		case 'X':return 5;
		case 'f':return 6;
		case 'c':return 7;
		case 's':return 8;
		default :return 0;
	}
}
/*判断是否有宽度标识符,并返回宽度值*/
uint8_t num_chack(uint8_t ch)
{
	static uint8_t num_buf = 0;
    uint8_t buffer = 0;				//数据二级缓存

    if((ch >= '0') && (ch <= '9'))      //如果检测到数字
    {									//执行数字操作,标志值置一
        num_buf = num_buf*10 + (ch - '0');
    }
    if(clean_wid_flag == 1)   //即将异常结束
    {
        num_buf = 0;          //计数清零
        return 0;			  //返回0
    }
    if(clean_wid_flag == 2)	  //即将正常结束
    {
		buffer = num_buf;num_buf = 0;
		return buffer;
	}
    return 0;
}

/*判断是否有精度标识符,返回精度对应值*/
uint8_t accuracy_chack(uint8_t *ch,uint8_t *num_flag)
{
	static uint8_t num_buf = 0;             //记录精度值
    uint8_t buffer = 0;

	if((*ch >= '0') && (*ch <= '9'))     //记录精度数值
        num_buf = num_buf*10 + *ch-'0'; 
    if(clean_acc_flag == 1)     //精度记录即将异常结束
    {      
    	num_buf = 0;
        return 0; 
    }
	if(clean_acc_flag == 2)		//精度记录即将正常结束
	{
		buffer = num_buf;num_buf = 0;
		return buffer;
	}
    return 0;
}

/*精度要求对应的操作 acc精度要求值,num2实际存在位数值*/
void acc_obj(uint8_t acc,uint8_t num2)
{
    for(num2 = acc - num2;num2 != 0;num2--)
        My_Putc('0');                //数字前面补0
}
/*宽度要求执行的对应的操作,wide宽度值,num2实际存在位数值*/
void wide_obj(uint8_t wide,uint8_t num2)
{   
    wide = wide - num2;
    for(;wide != 0;wide--)    //输出空格
        My_Putc(' ');
}

/*判断宽度精度以及数字关系,进行相应输出,该函数只能判断整数*/
void  chack_acc_wide(uint8_t wide_num,uint8_t accuracy_num,uint8_t counter)
{
    if(if_do_acc_flag == 1)                          //如果有精度控制
    {
        if(accuracy_num >= wide_num)             //精度要求大于宽度
        {
            if(accuracy_num >= counter)         //精度要求位大于数字实际位数,前面补0
            {
                acc_obj(accuracy_num,counter);  //执行精度操作
                return;                         //返回
            }
            else return;                       //精度要求位数小于数字实际位数,直接输出数字 
        }
        else                                   //精度要求小于宽度
        {
            if(wide_num >= counter)            //判断数字位数是否小于等于宽度
            {
                if(accuracy_num > counter)  //判断精度是否大于等于数字值
                {
                     wide_obj(wide_num,accuracy_num);		//补空格
                    acc_obj(accuracy_num,counter);	//前面补0
                    return;						
                }
                else return;     //精度小于直接输出 
            }
            else return;            //宽度小于实际位数,返回
        }
    }
    else
    {
        if(wide_num > counter)  wide_obj(wide_num,counter);
        else return;
    }
}

/*执行字符串的判断与输出*/
void str_send_job(uint8_t wide_num,uint8_t accuracy_num,uint8_t counter,uint8_t *p_buf)
{
    if((accuracy_num != 0) || (wide_num != 0))
    {
        if(accuracy_num >= wide_num)
        {
            if(accuracy_num >= counter)
                while(*p_buf != '\0')
                    My_Putc(*p_buf++);
            else
                while(accuracy_num-- != 0)
                    My_Putc(*p_buf++);
        }
        else
        {
            if(accuracy_num >= counter)
            {
                wide_obj(wide_num, counter);
                while(*p_buf != '\0')
                    My_Putc(*p_buf++);
            }
            else
                if(wide_num > counter)
                    wide_obj(wide_num, accuracy_num);		//宽度大于字符串长度在字符串前面输出空格
                while(accuracy_num-- != 0)
                    My_Putc(*p_buf++);
        }
    }
    else while(*p_buf != '\0') My_Putc(*p_buf++);
}

/*判断浮点数是否需要输出空格,和后补0*/
uint8_t if_black(uint8_t wide_num,uint8_t accuracy_num,uint8_t integer_counter,uint8_t decimal_counter)
{
	uint8_t num2 = 0;		//实际输出位数计数
	num2 = integer_counter+1+accuracy_num;
	
    if(accuracy_num == 0)       //没有指定精度
        accuracy_num = 6;
	if(wide_num > num2)
		wide_obj(wide_num, num2);
	if(accuracy_num > decimal_counter)
		return accuracy_num-decimal_counter;
	else
		return 0;
    	
}

//d转化
void d_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    int temp = 0;
    uint8_t num_buff[20] = {0};    //数字转换缓冲区
    uint8_t counter = 0;
    
    temp = (int)va_arg(*arg,int);      //获取不定区域的数字
    counter = system_change(temp,10,num_buff);    //执行进制转换
    chack_acc_wide(wide_num,accuracy_num,counter);    //输出限制性字符要求的内容
    str_up_down(num_buff);
    My_Putnum(num_buff);                          //输出数字(十进制)
}
//o转化
void o_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    uint16_t temp = 0;
    uint8_t num_buff[20] = {0};    //数字转换缓冲区
    uint8_t counter = 0;
    
    temp = (uint16_t)va_arg(*arg,int);      //获取不定区域的数字
    counter = system_change(temp,8,num_buff);       //执行进制转换
    chack_acc_wide(wide_num,accuracy_num,counter);    //输出限制性字符要求的内容
    str_up_down(num_buff);    
    My_Putnum(num_buff);                          //输出数字(八进制)
}
//u转化
void u_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    int temp = 0;
    uint8_t num_buff[20] = {0};    //数字转换缓冲区
    uint8_t counter = 0;
    
    temp = (int)va_arg(*arg,int);      //获取不定区域的数字
    if(temp < 0)  temp = -temp;
    counter = system_change(temp,10,num_buff);    //执行进制转换
    chack_acc_wide(wide_num,accuracy_num,counter);    //输出限制性字符要求的内容
    str_up_down(num_buff);
    My_Putnum(num_buff);                          //输出数字(无符号十进制)
}
//x转化
void x_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    uint16_t temp = 0;
    uint8_t num_buff[20] = {0};    //数字转换缓冲区
    uint8_t counter = 0;
    
    temp = (uint16_t)va_arg(*arg,int);      //获取不定区域的数字
    counter = system_change(temp,16,num_buff);    //执行进制转换
    chack_acc_wide(wide_num,accuracy_num,counter);    //输出限制性字符要求的内容
    str_up_down(num_buff);
    My_Putnum(num_buff);                          //输出数字(十六进制字母为小写)
}
//X转化
void X_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    uint16_t temp = 0;
    uint8_t num_buff[20] = {0};    //数字转换缓冲区
    uint8_t counter = 0;
    
    temp = (uint16_t)va_arg(*arg,int);      //获取不定区域的数字
    counter = system_change(temp,16,num_buff);    //执行进制转换
    chack_acc_wide(wide_num,accuracy_num,counter);    //输出限制性字符要求的内容
    for(counter += 1;counter != 0;counter--)          //将小写转换为大写
    {
        if((num_buff[counter-1] >= 'a') && (num_buff[counter-1] <= 'z'))
            num_buff[counter-1] = num_buff[counter-1] - 32;
    }
    str_up_down(num_buff);
    My_Putnum(num_buff);                          //输出数字(十六进制字母为大写)
}
//f转化
void f_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    double temp = 0.0;
    uint8_t integer_buff[8] = {0};          //整数部分缓存区
    uint8_t decimal_buff[8] = {0};          //小数部分缓存区
    long long integer = 0;                  //整数部分
    long long decimal = 0;                  //小数部分
    uint8_t integer_counter = 0;             //整数部分位数计数
    uint8_t decimal_counter = 0;            //小数部分位数计数
    uint8_t mand_flag = 0;					//后补0个数计数
    uint8_t i = 0;							//输出小数个数计数
		
    temp = (double)va_arg(*arg,double);
    integer = temp;								//获取整数部分
    decimal = (temp - integer) * 10000000;      //获取小数部分
    integer_counter = integer_str(integer,integer_buff);    //整数部分数组化
    decimal_counter = integer_str(decimal,decimal_buff);    //小数部分数组化

    str_up_down(integer_buff);					//执行数字正常排序
    str_up_down(decimal_buff);					//执行数字正常排序
	//执行宽度判断,判断过程需要输出空格执行完返回,需要输出后补0返回非零,不需要返回0
	mand_flag = if_black(wide_num, accuracy_num, integer_counter, decimal_counter);
	for(i = 0;integer_buff[i] != '\0';) My_Putc(integer_buff[i++]);				//输出整数部分
	My_Putc('.');							//输出小数点
    if(accuracy_num == 0)
        accuracy_num = 6;
	if(mand_flag != 0)						//即宽度大于小数实际位数,后面补0
		while(mand_flag-- != 0)
			My_Putc('0');
	else
		for(i = 0;accuracy_num-- != 0;)
			My_Putc(decimal_buff[i++]);		//输出指定精度的小数位个数
	
}
//c转化
void c_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    uint8_t temp = 0;
    
    temp = (uint8_t)va_arg(*arg,int);      //获取不定区域的字符
    wide_obj(wide_num,1);
    My_Putc(temp);                //输出字符
}
//s转化
void s_job(uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    uint8_t *p_buff = NULL;
    uint8_t counter = 0;
    uint8_t str_buf[50] = {0};          //未指定部分字符串的缓存区
    
    p_buff = (uint8_t *)va_arg(*arg,int);
    while(*p_buff != '\0')              //获取未指定部分字符串的值
        str_buf[counter++] = *p_buff++;
    str_send_job(wide_num, accuracy_num, counter-1, str_buf);            //执行限制性字符操作,同时输出字符串
}

/*执行对应标志的转换*/
void carry_obj(uint8_t flag,uint8_t wide_num,uint8_t accuracy_num,va_list *arg)
{
    switch(flag)
    {
        case 1:d_job(wide_num,accuracy_num,arg);break;
        case 2:o_job(wide_num,accuracy_num,arg);break;
        case 3:u_job(wide_num,accuracy_num,arg);break;
        case 4:x_job(wide_num,accuracy_num,arg);break;
        case 5:X_job(wide_num,accuracy_num,arg);break;             
        case 6:f_job(wide_num,accuracy_num,arg);break;
        case 7:c_job(wide_num,accuracy_num,arg);break;
        case 8:s_job(wide_num,accuracy_num,arg);break;
    }
}


/*将非格式化输出部分直接输出,同时计算格式化部分的宽度,精度的数值并执行对应操作,ch标志字符串,arg传递的参数*/
void wide_acc_get(uint8_t *ch,va_list *arg)
{
    uint8_t wide_num = 0;
    uint8_t accuracy_num = 0;
    uint8_t sign_num = 0;								//检测到格式输入错误标志
    uint8_t type_num = 0;                              
    
    while(*ch != '\0')
    {
        while((*ch != '%') && (*ch != '\0'))  My_Putc(*ch++);                                 //没有检测到标准输出的标志时直接输出对应字符
        if(*ch == '%')                                      //检测到标准输出标志
        {
            ch++;
        	for(;(*ch != '\0') && (type_chack(*ch) == 0) && (sign_num != 1);ch++)		//没有到最后一个字符且没有检测到类型标志符,且标志位没有置位,执行宽度精度值的获取
        	{
				if((*ch < '0' || *ch > '9') && (*ch != '.'))				//在限制性输出中出现非类型标识符,且不是'.',即输入错误
					wearing_flag = 1;	//错误标志位置一
				if(wearing_flag != 1)				//没有检测到错误标志
				{
					if(((*(ch+1) < '0' || *(ch+1) > '9') || *(ch + 1) == '\0') && (type_chack(*(ch+1) == 0) && (*(ch+1) != '.')))	//宽度检测即将异常结束
						clean_wid_flag = 1;										//异常结束
					if(*(ch+1) == '.') clean_wid_flag = 2;			//宽度记录即将正常结束
					if(if_point_flag == 1)					//记录到第一个小数点后
						if(((*(ch+1) < '0' || *(ch+1) > '9') || *(ch + 1) == '\0') && type_chack(*(ch+1)) == 0)		//精度获取即将异常结束
							clean_acc_flag = 1;						
					if(type_chack(*(ch+1)) != 0)						//本次扫描转换标准输出即将正常结束						
					{
						clean_wid_flag = 2;
						clean_acc_flag = 2;
					}					
					if(*ch == '.') {if_point_flag++;if_do_acc_flag = 1;}								//检测到小数点,小数点计数值增
					if(if_point_flag <= 1)				//并且没有出现第二个小数点
					{
						if(if_point_flag == 0) wide_num = num_chack(*ch);
						if(if_point_flag == 1) accuracy_num = accuracy_chack(ch, &sign_num);
					}
					else 				//出现了非数字,或者第二个小数点
					{
						clean_wid_flag = 0;wide_num= 0; accuracy_num = 0;if_do_acc_flag = 0;				  //对应记录值清零,执行下一个'%'的检测与扫描				
						type_num = 0;if_point_flag = 0;ch++;clean_acc_flag = 0;if_point_flag = 0;break;
					}
				}
				else 
				{
					clean_wid_flag = 0;wide_num= 0; accuracy_num = 0;if_do_acc_flag = 0;				  //对应记录值清零,执行下一个'%'的检测与扫描				
					type_num = 0;if_point_flag = 0;ch++;clean_acc_flag = 0;if_point_flag = 0;break;
				} 
            }
            type_num = type_chack(*ch);
            if(type_num != 0)
            {
                carry_obj(type_num,wide_num,accuracy_num,arg);	//一次'%'有关的参数获取完毕,执行对应字符串的转换以及输出工作
                clean_wid_flag = 0;wide_num= 0; accuracy_num = 0;if_do_acc_flag = 0;                 //对应记录值清零,执行下一个'%'的检测与扫描	            
                type_num = 0;if_point_flag = 0;ch++;clean_acc_flag = 0;if_point_flag = 0;
            }
        }
    }
}

void d_oled_job(va_list *arg)
{
    int temp = 0;
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t len = 0;
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    temp = va_arg(*arg,int);
    len = (uint8_t)va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    
    LCD_Show_Num(x,y,temp,len,color);
}

void u_oled_job(va_list *arg)
{
    int temp = 0;
    uint8_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t len = 0;
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    temp = va_arg(*arg,int);
    len = (uint8_t)va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    
    if(temp < 0) temp = -temp;

    LCD_Show_Num(x,y,temp,len,color);
}
void o_oled_job(va_list *arg)
{
    int temp = 0;
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t buff[15] = {0};
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    temp = va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    
    system_change(temp,8,buff);
    str_up_down(buff);
    LCD_Show_Str(x,y,buff,color);
}
void x_oled_job(va_list *arg)
{
    int temp = 0;
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t buff[15] = {0};
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    temp = va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    
    system_change(temp,8,buff);
    str_up_down(buff);
    LCD_Show_Str(x,y,buff,color);
}
void X_oled_job(va_list *arg)
{
    int temp = 0;
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t count = 0;
    uint8_t buff[15] = {0};
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    temp = va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    
    system_change(temp,8,buff);
    str_up_down(buff);
    while(buff[count] != '\0')
        if(buff[count] >= 'a' && buff[count] < 'z')
            {buff[count] = buff[count] - 32;count++;}
    LCD_Show_Str(x,y,buff,color);
}
void f_oled_job(va_list *arg)
{
    float temp = 0;
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t len = 0;
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    temp = va_arg(*arg,double);
    len = (uint8_t)va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    
    LCD_Show_FNum(x,y,temp,len,color);
}
void c_oled_job(va_list *arg)
{
    uint8_t temp = 0;
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t mode = 0;
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    temp = (uint8_t)va_arg(*arg,int);
    mode = (uint8_t)va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    
    LCD_Show_Char(x,y,temp,mode,color);
}
void s_oled_job(va_list *arg)
{
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t buff[30] = {0};
    uint8_t *p_buff = 0;
    uint8_t count = 0;
    
    x = (uint16_t)va_arg(*arg,int);
    y = (uint16_t)va_arg(*arg,int);
    p_buff = (uint8_t *)va_arg(*arg,int);
    color = (uint16_t) va_arg(*arg,int);
    while(*p_buff != '\0')
        buff[count++] = *p_buff++;
       
    LCD_Show_Str(x,y,buff,color);
}
void action_job(uint8_t type,va_list *arg)
{
    switch(type)
    {
        case 1:d_oled_job(arg);break;
        case 2:o_oled_job(arg);break;
        case 3:u_oled_job(arg);break;
        case 4:x_oled_job(arg);break;
        case 5:X_oled_job(arg);break;
        case 6:f_oled_job(arg);break;
        case 7:c_oled_job(arg);break;
        case 8:s_oled_job(arg);break;
    }
}
/*在oled上面显示内容*/
void oled_job(uint8_t *ch,va_list *arg)
{
    uint8_t *p_buff = 0;
    uint16_t x = 0,y = 0;            //显示在oled上面的横,纵坐标
    uint16_t color = 0;
    uint8_t type = 0;
    uint8_t buff[30] = {0};
    uint8_t count = 0;
    
    if(*ch == '%')
    {
        ch++;
        type = type_chack(*ch);
        action_job(type,arg);
    }
    else
    {
        x = va_arg(*arg,int);
        y = va_arg(*arg,int);
        p_buff = (uint8_t *)va_arg(*arg,int);
        color = (uint16_t)va_arg(*arg,int);
        while(*p_buff != '\0')
            buff[count++] = *p_buff++;
        LCD_Show_Str(x,y,buff,color);
    }
        
}
/*
*函数参数:uint8_t mode 
*						0 打印在串口
*						1 打印在LCD显示屏
*/
uint16_t my_printf(uint8_t mode,uint8_t *formate,...)
{    
    va_list arg;
    
    va_start(arg,formate);
    if(mode == 0)
        wide_acc_get(formate,&arg);
    else 
        oled_job(formate,&arg);
    
	va_end(arg);
	return 0;
}

















