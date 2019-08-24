#include "PID.h"

/*
*PID参数不稳定,常用参数
*  Kp       Ki      Kd
* 0.5      3.0     1.6
* 1.3      3.5     1.0
*/

pid_t pid;

double ActualSpeed = 0.0;    //PID调整过的实际转动速度
double ActuallAngle = 0.0;  //实际转动角度

double set_speed = 0.0;      //速度设定值
double set_angle = 0.0;      //角度设定值

/*
*函数功能:PID初始化
*/
void PID_init(void)
{
	pid.SetSpeed = 0.0;
	pid.ActualSpeed = 0.0;
	pid.err = 0.0;
	pid.err_next = 0.0;
	pid.Kp = 0.5;
	pid.Ki = 3.0;
	pid.Kd = 1.6;
}


/*速度度控制PID的实现*/
double PID_realize(void)
{
	double incrementSpeed = 0.0;
    double pid_ActualSpeed = 0.0;

	pid.SetSpeed = set_speed;
    pid_ActualSpeed = pid.ActualSpeed;
	pid.err = pid.SetSpeed - pid_ActualSpeed;
	incrementSpeed = pid.Kp*(pid.err - pid.err_next) + pid.Ki*pid.err + pid.Kd*(pid.err - 2*pid.err_next + pid.err_last);
	pid_ActualSpeed += incrementSpeed;
	pid.err_last = pid.err_next;
	pid.err_next = pid.err;
	return pid_ActualSpeed;
}

/*根据速度值输出对应的PWM占空比*/
void PWM_Send_Out(void)
{
	ActualSpeed = PID_realize();
    
    pwm_num = (uint16_t)(340 - (ActualSpeed - 5)*2.5);
}




















