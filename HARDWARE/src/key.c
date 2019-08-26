#include "key.h"

key key_t = {0,0,0};

uint8_t right_count = 1;     //向右移动位数
uint8_t down_count = 0;      //向下移动步数
uint8_t cur_flag = 0;        //是否调参状态标志
uint8_t num_buff[8] = {0};  //数据获取缓冲区
uint8_t change_num_flag = 0;  //数据是否改变标志,用于是否从新对参数进行修改的标志
uint8_t auxiliary_lock = 1;		//起到辅助清除显示完成卡扣功能
uint8_t clean_num_flag = 0;		//清除之前显示标志
uint8_t set_angle_falg = 0;		//设置指定角度标志

/*
*函数名称:void KEY_Init(void)
*函数功能:与按键有关IO的初始化
*函数参数:无
*/
void KEY_Init(void)
{
    RCC->AHB1ENR|= 1<< 5;		//使能PF时钟
    
    GPIO_Set(GPIOF,PIN0 | PIN1 | PIN2 | PIN3,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//配置IO
    GPIO_Set(GPIOF,PIN4 | PIN5 | PIN6 | PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);	//配置IO
} 
/*
*函数名称:static void _line_write(uint16_t position)
*函数功能:写对应行为0
*函数参数:uint16_t position		 扫描过程中的计数值
*/
static void _line_write(uint16_t position)
{
	PFout(0) = 1;
	PFout(1) = 1;
	PFout(2) = 1;
	PFout(3) = 1;
	PFout(position / 4) = 0;
}
/*
*函数名称:static uint8_t _list_read(uint16_t position)
*函数功能:读取对应列,检测按键是否按下
*函数参数:uint16_t position 	 扫描过程中的计数值
*/
static uint8_t _list_read(uint16_t position)
{
	position = position % 4;

	switch (position)
    {
        case 0:return PFin(4);
        case 1:return PFin(5);
        case 2:return PFin(6);
        case 3:return PFin(7);
	}
	return 1;
}
/*
*函数名称:static void Key_Scan_Once(uint16_t position)
*函数功能:
*函数参数:
*/
static void Key_Scan_Once(uint16_t position)
{
	_line_write(position);
	if(_list_read(position) == 0)
		key_t.status = 1 << position;
}
/*
*函数名称:static void Read_Key_Value(void)
*函数功能:读取按键扫描结果
*函数参数:无
*注意:已经在Key_Scan()中完成调用,重复使用会使无法检测到键值
*/
static void Read_Key_Value(void)
{
	uint8_t postion = 0;

	for(postion = 0;postion < 16;postion ++)
		{
	if((key_t.value >> postion & 0x01) == 1)
		break;
	}
	if(postion ++ >= 16)
		postion = 0;
	key_t.key_used_value = postion;                 //获得键值
    key_t.value = 0;                                //清除真值
}

/*
*函数名称:void KeyScan(void)
*函数功能:按键扫描
*函数参数:无
*/
void KeyScan(void)
{
	static uint8_t position = 0;
	static uint16_t buf_key_status = 0;
	if(key_t.status != 0)               //再次检测到按键动作
	{
		if(key_t.status == buf_key_status)  //与上一次检测结果相同
		{
			key_t.value = key_t.status;     //接收按键状态队列
			buf_key_status = 0;             //清零对比状态队列
            key_t.status = 0;               //清空状态队列
			Read_Key_Value();               //读取按键键值
		}
		else                                //与上次检测结果不相同
		{
			buf_key_status = 0;             //按键状态队列清空
			buf_key_status |= key_t.status; //获取按键状态队列值
			key_t.status = 0;               //清空按键状态队列
		}
	}
	else
		Key_Scan_Once(position);

	position ++;
	if(position >= 16)
		position = 0;
}

/******将num_buff全部清除为'0'*******/
static void clean_num_buff(void)
{
	uint8_t i;
	for(i = 0;i < 7;i++)
	{	
		if(num_buff[i] == '\0')
		{
			num_buff[i] = '0';
		}
	}
    if((down_count == 0) || (down_count == 4))
        num_buff[6] = '\0';
	num_buff[7] = '\0';
}
/*******************************光标移动附属函数*******************************************************/
/*光标从速度跳转到PID*/
static void cur_1_to_2(void)
{
    LCD_Close_Cur(80+right_count*9-1,156);    //关闭之前光标
    down_count = 1;                         //下落计数加1
    right_count = 1;                        //计数值归位
    LCD_Show_Cur(38,172);                   //在PID参数最开始显示光标
}
/*光标从PID跳转到速度*/
static void cur_2_to_1(void)
{
    LCD_Close_Cur(30+right_count*9-1, 172);	//关闭之前的光标
    down_count = 0;                     //下落计数值清零
    right_count = 1;                //右移计数归位
    LCD_Show_Cur(88,156);           //光标设定在设定速度参数的最开始位置
}


/*光标从速度设置跳转到角度设置*/
static void cur_1_to_4(void)
{
    LCD_Close_Cur(80+right_count*9-1, 156);	//关闭之前的光标
    down_count = 4;                    //下落计数值设置为最多
    right_count = 1;                  //右位移计数归位
    LCD_Show_Cur(178,196);              //光标设置在角度控制的最开始位置
}

/*光标从角度跳转到速度*/
static void cur_4_to_1(void)
{
    LCD_Close_Cur(170+right_count*9-1,196);   //关闭之前光标
    down_count = 0;
    right_count = 1;                        //右移计数归位
    LCD_Show_Cur(88,156);                   //在速度参数最开始显示光标
}

/*光标从PID跳转到角度*/
static void cur_3_to_4(void)
{
    LCD_Close_Cur(30+right_count*9-1,172+(down_count-1)*20);  //关闭之前光标
    down_count = 4;                                 //光标计数值加一
    right_count = 1;                                //右移计数归位
    LCD_Show_Cur(178,196);                          //在角度设置数字最开始显示光标
}

/*光标从角度跳转到PID*/
static void cur_4_to_3(void)
{
    LCD_Close_Cur(170+right_count*9-1, 196);  //关闭之前光标
    down_count = 3;                 //down_count计数值减一
    right_count = 1;                //左右移计数值归位
    LCD_Show_Cur(38,212);           //在PID有关参数最开始参数数值之前显示光标
}

/*光标在PID参数之间向上跳转*/
static void cur_PID_up(void)
{
    LCD_Close_Cur(30+right_count*9-1, 172+(down_count-1)*20); //关闭之前光标
    down_count -= 1;                //down_count计数值减一
    LCD_Show_Cur(30+right_count*8,172+(down_count-1)*20);   //在正上或者正下方显示光标
}

/*光标在PID参数之间向下跳转*/
static void cur_PID_down(void)
{
    LCD_Close_Cur(30+right_count*9-1, 172+(down_count-1)*20); //关闭之前光标
    down_count += 1;                //down_count计数值减一
    LCD_Show_Cur(30+right_count*9-1,172+(down_count-1)*20);   //在正上或者正下方显示光标
}

/*光标在行之间移动时关闭之前所在位置的光标*/
static void cur_close_line(void)
{
    switch(down_count)
    {
        case 0:LCD_Close_Cur(80+right_count*9-1,156);break;
        case 1:
        case 2:
        case 3:LCD_Close_Cur(30+right_count*9-1,172+(down_count-1)*20);break;
        case 4:LCD_Close_Cur(170+right_count*9-1,196);break;
    }
}

/*光标行间移动*/
static void cur_line_turn(void)
{
    switch(down_count)
    {
        case 0:LCD_Show_Cur(80+right_count*9-1,156);break;
        case 1:
        case 2:
        case 3:LCD_Show_Cur(30+right_count*9-1,172+(down_count-1)*20);break;
        case 4:LCD_Show_Cur(170+right_count*9-1,196);break;
    }
}


/*光标向上移动*/
void up_action(void)
{
    switch(down_count)
    {
        case 0:cur_1_to_4();break;          //速度跳转到角度
        case 1:cur_2_to_1();break;          //PID跳转到速度
        case 2:
        case 3:cur_PID_up();break;       //PID之间的跳转
        case 4:cur_4_to_3();break;          //角度跳转到PID
    }
    key_t.key_used_value = 0;               //清除键值
}


/*光标向下移动*/
void down_action(void)
{
    switch(down_count)
    {
        case 0:cur_1_to_2();break;          //速度跳转到角度
        case 1:
        case 2:cur_PID_down();break;       //PID之间的跳转
        case 3:cur_3_to_4();break;          //PID到角度转化
        case 4:cur_4_to_1();break;          //角度跳转到速度
    }
    key_t.key_used_value = 0;               //清除键值
}

/*光标向左移动*/
void left_action(void)
{
    cur_close_line();                   //执行关闭之前的光标的操作
    if(right_count == 1)                                    //如果已经移动到最左边
    {
        if((down_count == 0) || (down_count == 4))   //速度,角度,限制宽度6
            right_count = 6;                        //向左移到头,返回右边
        else right_count = 7;                        //PID限制宽度7
    }
    else right_count -= 1;                      //右移位数计数减一
    cur_line_turn();                        //执行光标后移操作
    key_t.key_used_value = 0;               //清除键值
}

/*光标向右移动*/
void right_action(void)
{
    cur_close_line();          //关闭之前光标
    if((right_count == 6) && (down_count == 0 || down_count == 4))  //速度角度光标到最右端
        right_count = 6;
    else if(right_count == 7)               //pid在最右端
        right_count = 7;
    else right_count += 1;          //右移计数加一
    cur_line_turn();
    key_t.key_used_value = 0;               //清除键值
}
/***************************************有关光标移动的函数结束***********************************************/

/*********************************按键操作********************************************************/
/*按键11执行的操作,向上移动光标*/
static void key11_action(void)
{
	if(change_num_flag)
	{
		auxiliary_lock = 1;				//开锁
		change_num_flag = 0;			//清除参数改变标志
		set_mand_Variable();			//在光标向上进行移动时,标志之前修改结束,将修改值保存
	}
    up_action();
}

/*按键12执行的操作,向下移动光标*/
static void key12_action(void)
{
	if(change_num_flag)
	{
		auxiliary_lock = 1;				//开锁
		change_num_flag = 0;			//清除参数改变标志
		set_mand_Variable();				//在光标向上进行移动时,
	}
    down_action();
}
/*按键13执行的操作*/
static void key13_action(void)
{
	left_action();
}
/*按键14执行的操作*/
static void key14_action(void)
{
	right_action();
}

/*在速度设置参数处显示小数点*/
static void set_speed_point(void)
{
    my_printf(1,"%c",80+(right_count-1)*9,156,'.',0,RED);
    right_action();
}
/*在PID设置参数处显示小数点*/
static void set_pid_point(void)
{
    my_printf(1,"%c",30+(right_count-1)*9,172+(down_count-1)*20,'.',0,RED);
    right_action();
}
/*在角度设置参数处显示小数点*/
static void set_angle_point(void)
{
    my_printf(1,"%c",170+(right_count-1)*9,196,'.',0,RED);
    right_action();
}
/*按下小数点键执行的操作,在小数点获取时也进行了光标的右移操作*/
/*按键15执行的操作*/
static void key15_action(void)
{
    num_buff[right_count-1] = '.';                        //获取小数点
    
    switch(down_count)
    {
        case 0:set_speed_point();break;
        case 1:
        case 2:
        case 3:set_pid_point();break;
        case 4:set_angle_point();break;               //写下小数点后光标右移一位
    }                                  
    key_t.key_used_value = 0;               //键值清零
	if(auxiliary_lock) 
    {
        auxiliary_lock = 0;
        clean_num_flag = 1;	//置位清除标志
    }
    if(change_num_flag != 1)
        change_num_flag = 1;					//执行了参数修改操作
}
/*******************************小数点结束显示****************************************************/
/*清空指定数组*/
void clean_array(uint8_t *p_buf)
{
	while(*p_buf != '\0')
	{
		*p_buf++ = '\0';					//全部清除赋0
	}
}

/*在速度设置位全部除了操作位写零*/
static void clean_speed(void)
{
	uint8_t counter;
	
	for(counter = 0;counter < 6;counter++)
	{
		if(num_buff[counter] == '0')
			my_printf(1, "%d", 80+counter*9,156,0,1,RED);
	}
}
/*在角度设置位全部除了操作位写零*/
static void clean_angle(void)
{
	uint8_t counter;
	for(counter = 0;counter < 6;counter++)
	{
		if(num_buff[counter] == '0')
			my_printf(1, "%d", 170+counter*9,196,0,1,RED);
	}
}
/*在Kp设置位全部除了操作位写零*/
static void clean_Kp(void)
{
	uint8_t counter;
	for(counter = 0;counter < 7;counter++)
	{
		if(num_buff[counter] == '0')
			my_printf(1, "%d", 30+counter*9,172,0,1,RED);
	}
}

/*在Ki设置位全部除了操作位写零*/
static void clean_Ki(void)
{
	uint8_t counter;
	for(counter = 0;counter < 7;counter++)
	{
		if(num_buff[counter] == '0')
			my_printf(1, "%d", 30+counter*9,192,0,1,RED);
	}
}
/*在Kd设置位全部除了操作位写零*/
static void clean_Kd(void)
{
	uint8_t counter;
	for(counter = 0;counter < 7;counter++)
	{
		if(num_buff[counter] == '0')
			my_printf(1, "%d", 30+counter*9,212,0,1,RED);
	}
}

/*一个只要更改一个字母就将所有显示位清零,除了本身那一位*/
void new_parameter_set(void)
{
	switch(down_count)
	{
		case 0:clean_speed();break;
		case 1:clean_Kp();break;
		case 2:clean_Ki();break;
		case 3:clean_Kd();break;
		case 4:clean_angle();break;
	}
}

/**********************************************结束**************************************************************/

/**************************************************************************************************************/
/*在速度的位置显示*/
static void show_in_speed(uint8_t num)
{
	my_printf(1,"%d",80+(right_count-1)*9,156,num,1,RED);
	right_action();
}
/*在PID的位置显示*/
static void show_in_pid(uint8_t num)
{
	my_printf(1,"%d",30+(right_count-1)*9,172+(down_count-1)*20,num,1,RED);
	right_action();
}
/*在角度的位置显示*/
static void show_int_angle(uint8_t num)
{
	my_printf(1,"%d",170+(right_count-1)*9,196,num,1,RED);
	right_action();
}

/*在按下数字键后在lcd上面直接显示数字,在该步骤中实现了数字显示后光标同时右移*/
static void direct_show_num(uint8_t num)
{
    switch(down_count)
    {
        case 0:show_in_speed(num);break;
        case 1:
        case 2:
        case 3:show_in_pid(num);break;
        case 4:show_int_angle(num);break;
    }
}

/*显示,获取按键输入参数*/
void num_action(void)
{     
    uint8_t num = 0;                        //暂时计数值
    num =key_t.key_used_value - 1;           //数据获取
    num_buff[right_count-1] = num + '0';    //先进行存储获取的参数
    direct_show_num(num);                   //LCD实时显示数字,显示后光标自动后移 	    
    if((down_count == 0) || (down_count == 4))			//速度与角度限制输入宽度6
    {
        num_buff[6] = '\0';
        num_buff[7] = '\0';
    }
    key_t.key_used_value = 0;                   //检测到的键值清零          
}

///////////////////////////////////以上为准备程序//////////////////////////////////////////////////

/////////////////////////////以上为对应的OLED上面的显示的操作//////////////////////////////////////////////////////////////
/*模式按键的进行的操作*/
void mode_key_action(void)
{
    cur_flag = !cur_flag;           //每次按下模式键,光标进行打开与关闭的操作
    if(cur_flag) 
	{
		LCD_Show_Cur(88,156); //在初始位置显示光标
    }
	else 
    {
        set_mand_Variable();			//最后一次修改的参数的赋值与存储
        cur_close_line();              //调参结束后关闭光标
        down_count = 0;     
        right_count = 1;                //位置移动计数清零
        auxiliary_lock = 1;
		clean_num_flag = 0;				//上锁变量复位
    }
    key_t.key_used_value = 0;           //键值清零
}

/*方向键以及小数点键进行的操作*/
static void dir_job()
{
	switch (key_t.key_used_value)
	{
		case 11:key11_action();break;
		case 12:key12_action();break;
		case 13:key13_action();break;
		case 14:key14_action();break;
        case 15:key15_action();break;			//执行对应按键的操作
        default: break;
	}
	key_t.key_used_value = 0;	//键值清零
}

///////////////////////////////将字符串数字之中的数字转换成浮点数的形式///////////////////////
/*将存储在缓冲区的数据转换成数值,在串口改变设定值时也会使用,在读取eeprom赋值时也需使用*/
double turn_to_num(uint8_t *p_buf)
{
    uint8_t i = 0,flag = 0;
    uint8_t point_location = 0; //小数点所在位置
    uint8_t turn_count = 0;     //转换过程中计数变量
	double turned_num = 0.0;			//转换完成的组合数字
	uint32_t integer_num = 0.0;		//整数部分
	double decimal_num = 0.0;		//小数部分
    
    while(*p_buf != '\0')
    {
        if(*p_buf != '.')
        {
            if(!flag)
            {
                integer_num = integer_num * 10 + *p_buf - '0';
            }
            else 
            {
            	decimal_num = *p_buf - '0';				//获取小数位数字
                for(turn_count = i - point_location;turn_count != 0;turn_count--)
                    decimal_num *= 0.1;
                turned_num += decimal_num;
            }
            i++; 
			p_buf++;
        }
        else 
        {
            point_location = i;flag = 1;i++;p_buf++;
        }
    }
    return turned_num += integer_num;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/*设置并存储速度参数*/
static void set_mand_speed(void)
{
	set_speed = turn_to_num(num_buff);
	supplement_fun(6, num_buff);
	eeprom_write(10, num_buff, 6);
	clean_array(num_buff);					//到此num_buff使用周期完成,进行销毁清除操作
	grid_counter = 0;
}
/*设置并存储Kp参数*/
static void set_mand_Kp(void)
{
	pid.Kp = turn_to_num(num_buff);
	supplement_fun(7, num_buff);
	eeprom_write(30, num_buff, 7);
	clean_array(num_buff);					//到此num_buff使用周期完成,进行销毁清除操作
	grid_counter = 0;
}
/*设置并存储Ki参数*/
static void set_mand_Ki(void)
{
	pid.Ki = turn_to_num(num_buff);
	supplement_fun(7, num_buff);
	eeprom_write(40, num_buff, 7);
	clean_array(num_buff);					//到此num_buff使用周期完成,进行销毁清除操作
	grid_counter = 0;
}
/*设置并存储Kd参数*/
static void set_mand_Kd(void)
{
	pid.Kd = turn_to_num(num_buff);
	supplement_fun(7, num_buff);
	eeprom_write(50, num_buff, 7);
	clean_array(num_buff);                  //到此num_buff使用周期完成,进行销毁清除操作
	grid_counter = 0;
}
/*设置并存储角度参数*/
static void set_mand_angle(void)
{
	set_angle = turn_to_num(num_buff);
	supplement_fun(6, num_buff);
	eeprom_write(20, num_buff, 6);
    grid_counter = 0;                       //重新设置,计数清零
	clean_array(num_buff);					//到此num_buff使用周期完成,进行销毁清除操作
	grid_counter = 0;
}

/*将获取到的数据传输给对应参数变量*/
void set_mand_Variable(void)
{
    switch(down_count)
    {
        case 0:set_mand_speed();break;                               //赋予按键读取的数据
        case 1:set_mand_Kp();break;
        case 2:set_mand_Ki();break;
        case 3:set_mand_Kd();break;                                 //PID参数获取
        case 4:set_mand_angle();break;                              //角度设定值获取
    }
}

/*有关电机旋转方向的控制引脚初始化*/
void Motor_IO_Init(void)
{
	RCC->AHB1ENR |= 1 << 4;
	GPIO_Set(GPIOE, PIN0 | PIN1,GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PD);
}
/*模式转换过程中要执行的功能*/
static void chang_mode_job(void)
{
	static double mand_temp = 0.0;
    static uint8_t mand_flag = 0;
    
	set_angle_falg = !set_angle_falg;
	clean_before();
	if(set_angle_falg)		//处于按照角度转的模式
	{
        if(!mand_flag)
        {
            mand_temp = set_speed;	//保存之前的设定转速
            mand_flag = 1;
        }
		PWM_VAL = 280;
		set_speed = 36;		//更新转速设定值
		grid_counter = 0;
	}
	else
	{
        pwm_num = 250;              //保证能转
		set_speed = mand_temp;
		mand_temp = 0;mand_flag = 0;				//恢复设定转速值,清零暂存值
	}
	
}
/*控制电机的按键的功能*/
static void moter_control_key(void)
{
    switch(key_t.key_used_value)
    {
    	case 1:chang_mode_job();break;			//第一个按键在非调参状态用来切换转角度,还是转指定速度
        case 13:PEout(0) = 1;PEout(1) = 0;grid_counter = 0;break;            //正转
        case 14:PEout(0) = 0;PEout(1) = 1;grid_counter = 0;break;            //反转
        case 15:PEout(0) = 0;PEout(1) = 0;grid_counter = 0;;break;            //停止
        default :break;
    }
    key_t.key_used_value = 0;                       //检测到的键值清零
}

void Key_Action(void)
{    
    if(key_t.key_used_value == 16)                  //设定参数键
    {
        mode_key_action();                          //执行模式设置
    }
    if(cur_flag == 1)               //处于调参模式
    {
        if((key_t.key_used_value >= 1) && (key_t.key_used_value <= 10))       //检测到数字键 0 ~ 9
        {
        	if(auxiliary_lock) 
            {
                auxiliary_lock = 0;
                clean_num_flag = 1;		//清除之前数字标志置一	
            }
        	change_num_flag = 1;						//执行了调参标志置一
            num_action();                               //执行数字设置显示操作
        }
        else dir_job();					//只有在调参模式允许光标移动,进行参数修改
        if(clean_num_flag)
        {
        	auxiliary_lock = 0;		
			clean_num_flag = 0;			//形成上锁
			clean_num_buff();			//执行清除操作
			new_parameter_set();		//更新显示oled显示的数值,未设置位显示0
		}
    }
    else                            //处于非调参模式,更改电机运行状态
    {
        moter_control_key();        //执行控制转动操作
        if(!set_angle_falg)
        {
			speed_thing();			//没有执行转指定角度操作,显示速度的坐标
            LCD_Show_Speed();
        }
		else						//显示角度对应的坐标
        {
			angle_thing();
            LCD_Show_angle();
        }
    }
}

/*******************************输入不足补全对应数组的函数**************************************************/
//当输入位数小于数组限制最大位数     对于速度角度来说应该是6个字符        对于pid参数来说应该是7位
//当只输入整数,即没有输入小数点    在数字的最后一位加上小数点1,之后补0到规定位数
//当输入了小数点,直接在后面补充0直到限制位数
//该函数会在存储设定参数之前使用,还会在usart中的存储中使用到
//len 限制的宽度,调解完毕之后的数组1宽度为len
//*p_buf 数组的首地址

void supplement_fun(uint8_t len,uint8_t *p_buf)
{
    uint8_t flag = 0;               //是否有小数点标志
    uint8_t counter = 0;            //数组中已经存在的字符的个数
    
    while(*p_buf != '\0')
    {
        if(*p_buf != '.')			//记录除去小数点的数字的个数
        {
            p_buf++;counter++;
        }
		else
		{
			counter++;flag = 1;p_buf++;		//标志位置一,计数值增加1
		}	
    }
	if(counter < len)
	{
        if(flag == 0)
        {
            *p_buf++ = '.';
            for(len -= (counter+1);len != 0;len--)
            {
                *p_buf++ = '0';
            }
        }
        else
            for(len -= counter;len != 0;len--)
                *p_buf++ = '0';
	}
	else if(counter > len)
	{
		p_buf -= (counter - len);
		*p_buf = '\0';
	}
}



















