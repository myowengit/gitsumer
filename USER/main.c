#include "glob.h"

uint8_t set_speed_began[] = "70.000";
uint8_t set_angle_began[] = "60.000";
uint8_t Kp_began[] = "0.500000";
uint8_t Ki_began[] = "3.000000";
uint8_t Kd_began[] = "1.600000";

int main(void)
{   
	Every_Init();
	eerpom_fist_use();
    pwm_num = 250;          //设置初始pwm_num值,保障电机可以转动
	while (1)
    {
        Key_Action();
		get_motor_speed();
		usart_job();
        PWM_VAL = pwm_num;	
    }
}


/*
*函数功能:各种内容的初始化
*函数参数:无
*/
void Every_Init(void)
{
	Stm32_Clock_Init(336,8,2,7);
	delay_init(168);
    uart_init(84,115200); 
    TIM3_Int_Init(50,8400-1);
    TIM14_PWM_Init(500-1,84-1);	//1Mhz的计数频率,2Khz的PWM. 
    TIM5_CH1_Cap_Init(0XFFFFFFFF,84-1);//以1Mhz的频率计数
    PID_init();
    KEY_Init();
    Motor_IO_Init();            //初始化电机控制转向IO
    eeprom_init();
	LCD_Init();
	LCD_Clear(WHITE);
	BACK_COLOR = WHITE;
	interface_job();		//基础界面显示
}

/*
*函数功能:在第一次启动时将数据初始值写入eeprom
*函数参数:无
*/
void eerpom_fist_use(void)
{
	uint8_t chack = 0;
	uint8_t i;
	uint8_t buff[8] = {0};

	eeprom_read(1, &chack, 1);

	if(chack != 'X')
	{
		eeprom_write(1, "X", 1);
		eeprom_write(10,set_speed_began,6);
		eeprom_write(20,set_angle_began,6);
		eeprom_write(30,Kp_began,7);
		eeprom_write(40,Ki_began,7);
		eeprom_write(50,Kd_began,7);
	}                                               //将初始值写入eeprom
    
	eeprom_read(10, buff, 6);
	for(i = 0;i < 6;i++)
		my_printf(1,"%c",80+i*9,156,buff[i],0,RED);
	set_speed = turn_to_num(buff);
	
	eeprom_read(20, buff, 6);
    for(i = 0;i < 6;i++)
		my_printf(1,"%c",170+i*9,196,buff[i],0,RED);
	set_angle = turn_to_num(buff);
	
	eeprom_read(30, buff, 7);
    for(i = 0;i < 7;i++)
		my_printf(1,"%c",30+i*9,172,buff[i],0,RED);
	pid.Kp = turn_to_num(buff);
	
	eeprom_read(40, buff, 7);
    for(i = 0;i < 7;i++)
		my_printf(1,"%c",30+i*9,192,buff[i],0,RED);
	pid.Ki = turn_to_num(buff);
	
	eeprom_read(50, buff, 7);
    for(i = 0;i < 7;i++)
		my_printf(1,"%c",30+i*9,212,buff[i],0,RED);
    pid.Kd = turn_to_num(buff);                           //数据赋值以及显示
}









