#ifndef __PID_H__
#define __PID_H__
#include "sys.h"
#include "motor.h"

typedef struct _pid
{
	double SetSpeed;
	double ActualSpeed;
	double err;
	double err_next;
	double err_last;
	double Kp, Ki, Kd;
}pid_t;

extern pid_t pid;
extern double set_speed;      //速度设定值
extern double set_angle;      //角度设定值
extern double ActualSpeed;    //PID调整过的实际转动速度
extern double ActuallAngle;  //PID调整过的实际转动角度

void PID_init(void);
void angle_pid_init(void);
double PID_realize(void);
double PID_realize(void);
void PWM_Send_Out(void);


#endif	/*__PID_H__*/















