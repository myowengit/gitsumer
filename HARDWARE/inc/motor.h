/*
*说明:电机控制的PWM输出口为PF9
*电机正方转控制IO 
*		IN1    PE0
*		IN2    PE1
*/
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "sys.h"
#include "timer.h"
#include "pid.h"
#include "delay.h"
#include "oled.h"
#include "my_printf.h"
#include "key.h"

//通过改变TIM14->CCR1的值来改变占空比，从而控制电机转速
#define PWM_VAL TIM14->CCR1  

extern int speed_buff[20];                           //存储每次读取到的高电平的时间
extern uint8_t flag;
extern uint16_t pwm_num;
extern int grid_counter;

void TIM14_PWM_Init(u32 arr,u32 psc);
void quick_sort(int *a, int low, int high);
int find_mode_number(int *arr, int len);
void get_motor_speed(void);
void Motor_pulse_get(void);
void get_moter_angle(void);


#endif	/*__MOTOR_H__*/

