#include "motor.h"

uint16_t pwm_num = 0;

/*说明:角度指定值最小为0.5389*/

int speed_buff[20] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0
};                           //存储每次读取到的高电平的时间
uint8_t flag = 0;									//一次速度值的需要数据读取完毕标志
int grid_counter = 0;							//转过的栅格

/*交换两个整数的值*/
void swap(int *p, int *q)
{
	int buf;

	buf = *p;
	*p = *q;
	*q = buf;

	return;
}
/*快速排序*/
void quick_sort(int *a, int low, int high)
{
	int i = low;
	int j = high;
	int key = a[low];
	if (low >= high)  //如果low >= high说明排序结束了
	{
		return;
	}
	while (low < high)  //该while循环结束一次表示比较了一轮
	{
		while (low < high && key <= a[high])
		{
			--high;  //向前寻找
		}
		if (key > a[high])
		{
			swap(&a[low], &a[high]);
			++low;
		}
		while (low < high && key >= a[low])
		{
			++low;  //向后寻找
		}
		if (key < a[low])
		{
			swap(&a[low], &a[high]);
			--high;
		}
	}
	quick_sort(a, i, low - 1);  //用同样的方式对分出来的左边的部分进行同上的做法
	quick_sort(a, low + 1, j);  //用同样的方式对分出来的右边的部分进行同上的做法
}

/*查找一个有序数组中的众数*/
int find_mode_number(int *arr, int len)
{
	int many = 1, less = 1;
	int value = 0;
	for (int i = 0; i < len; i++) {
		for (int j = i; j < len; j++)
		{
			if (arr[j] == arr[j + 1]) {
				less++;
			}
			else {
				if (many < less) {
					swap(&many, &less);
					value = arr[j];
				}
				less = 1;
				break;
			}
		}
	}
	return value;
}

/*
*函数功能:PWM初始化
*函数参数:arr 自动重装值
*		  psc 时钟预分频数
*/
void TIM14_PWM_Init(u32 arr,u32 psc)
{
    RCC->APB1ENR |= 1 << 8;
    RCC->AHB1ENR |= 1 << 5;
    GPIO_Set(GPIOF,PIN9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
    GPIO_AF_Set(GPIOF,9,9);
    
    TIM14->ARR = arr;
    TIM14->PSC = psc;
    TIM14->CCMR1 |= 6 << 4;
    TIM14->CCMR1 |= 1 << 3;
    TIM14->CCER |= 1 << 0;
    TIM14->CCER |= 1 << 1;
    TIM14->CR1  |= 1 << 7;
    TIM14->CR1  |= 1 << 0;
}

/*获取电机高电平脉冲时间*/
void Motor_pulse_get(void)
{
    int time = 0;                          //获取转动时间
    long long temp = 0;
    static uint8_t counter = 0;
    
	if(TIM5CH1_CAPTURE_STA & 0x80)              //获取高脉冲时间
    {
        if(flag == 0)
        {
            temp = TIM5CH1_CAPTURE_STA & 0x3F;
            temp *= 0xFFFFFFFF;
            temp += TIM5CH1_CAPTURE_VAL;
            time = temp;
            
            if(counter < 20)               //脉冲个数计数值,每记录20个进行一次速度转换
            {                            
                speed_buff[counter] = time; 
                counter++;      //计数值清零
            }
            else 
            {
                flag = 1;
                counter = 0;
            }
        }
		if(set_angle_falg)
		{
			counter++;
		}
        TIM5CH1_CAPTURE_STA = 0;
    }
}
/*获取电机转速*/
void get_motor_speed(void)
{
    int mode_num = 0;
    long sum_time = 0;      //记录总时间
    uint8_t i = 0;
    static uint8_t over_time_counter = 0;   //超时计数,如果多次flag都没有再进行置一操作,说明电机已经进入了停止状态
    
    if(flag == 1)
    {
        quick_sort(speed_buff,0,sizeof(speed_buff)/sizeof(int)-1);
        mode_num = find_mode_number(speed_buff,sizeof(speed_buff) / sizeof(int));
        for(i = 0;i < 20;i++)
        {
            speed_buff[i] = 0;                              //使用过后进行清空操作
        }
        sum_time = 334 * 2 * mode_num;
        if(sum_time != 0)
        {
            flag = 0;
            pid.ActualSpeed = 1000000.0 / sum_time;
        }
        else
        {
            flag = 0;
            pid.ActualSpeed = 0;
        }
        if(set_angle_falg) get_moter_angle();
        PWM_Send_Out();
        over_time_counter = 0;
    }
    else
    {
        over_time_counter++;
        if(over_time_counter > 30)          //30次没有检测到即为停止转动
        {
            over_time_counter = 0;
            pid.ActualSpeed = 0;
        }
    }
    my_printf(1,"%f",170,180,ActuallAngle,5,RED);
    my_printf(1, "%f", 80,140,pid.ActualSpeed,5,RED);
}

/*获取转过的角度*/
void get_moter_angle(void)
{
    uint16_t counter = 0;
    uint8_t flag = 0;               //标志,第一次获取设置速度
    int grid_counter_buff = 0;
    
    if(!flag)
    {
       set_speed = 30;
       turn_angle_start();
       flag = 1;
    }
    if(grid_counter > 300)
    {
        grid_counter = -grid_counter;
    }
    grid_counter_buff = grid_counter;
    counter = (uint16_t)((set_angle + 1)/1.1); 
    
    if(grid_counter_buff < 0)
        grid_counter_buff = -grid_counter_buff;
    ActuallAngle = (2*grid_counter_buff -1)*0.5389;				//获取实际转动角度
    
    if((counter - grid_counter < 20) && (grid_counter - counter < 20))
    {
        PEout(0) = 0;PEout(1) = 0;
        flag = 0;
    }
    else if(grid_counter > counter)
    {
        PEout(0) = !PEout(0);
        PEout(1) = !PEout(1);
        grid_counter = -grid_counter;
    }
}

/*指定转动角度时特殊的启动方法*/
void turn_angle_start(void)
{
	PWM_VAL = 280;				//设置最小起转PWM对应值
}


