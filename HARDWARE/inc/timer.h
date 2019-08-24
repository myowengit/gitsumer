#ifndef __TIMER_H__
#define __TIMER_H__
#include "sys.h"
#include "motor.h"

extern u8 TIM5CH1_CAPTURE_STA;
extern u32 TIM5CH1_CAPTURE_VAL;
extern uint8_t pulse_counter;            //脉冲个数计数值ֵ
    
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);

#endif /*__TIMER_H__*/

