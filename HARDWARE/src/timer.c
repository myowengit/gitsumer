#include "timer.h"

uint8_t pulse_counter = 0;            //脉冲个数计数值

/*
*函数功能:定时器5初始化,输入捕获
*/
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{
    RCC->APB1ENR |= 1 << 3;
    RCC->AHB1ENR |= 1 << 0;
    GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);
    GPIO_AF_Set(GPIOA,0,2);
    
    TIM5->ARR = arr;
    TIM5->PSC = psc;
    
    TIM5->CCMR1 |= 1 << 0;
    TIM5->CCMR1 |= 0 << 4;
    TIM5->CCMR1 |= 0 << 10;
    
    TIM5->CCER |= 0 << 1;
    TIM5->CCER |= 1 << 0;
    
    TIM5->EGR = 1 << 0;
    TIM5->DIER |= 1 << 1;
    TIM5->DIER |= 1 << 0;
    TIM5->CR1 |= 0x01;
    MY_NVIC_Init(2,0,TIM5_IRQn,2);
}
u8 TIM5CH1_CAPTURE_STA = 0;
u32 TIM5CH1_CAPTURE_VAL;

/*
*函数功能:TIM5 捕获中断服务函数
*/
void TIM5_IRQHandler(void)
{
    u16 tsr;
    tsr = TIM5->SR;
    if((TIM5CH1_CAPTURE_STA & 0x80) == 0)
    {
        if(tsr & 0x01)
        {
            if(TIM5CH1_CAPTURE_STA & 0x40)
            {
                if((TIM5CH1_CAPTURE_STA & 0x3F) == 0x3F)
                {
                    TIM5CH1_CAPTURE_STA |= 0x80;
                    TIM5CH1_CAPTURE_VAL = 0xFFFFFFFF;
                }else TIM5CH1_CAPTURE_STA ++;
            }
        }
        if(tsr & 0x02)
        {
            if(TIM5CH1_CAPTURE_STA & 0x40)
            {
                TIM5CH1_CAPTURE_STA |= 0x80;
                TIM5CH1_CAPTURE_VAL = TIM5->CCR1;
                TIM5->CCER = ~(1 << 1);
            }
            else
            {
                TIM5CH1_CAPTURE_STA = 0;
                TIM5CH1_CAPTURE_VAL = 0;
                TIM5CH1_CAPTURE_STA |= 0x40;
                TIM5->CR1 &= ~(1 << 0);
                TIM5->CNT = 0;
                TIM5->CCER |= 1 << 1;
                TIM5->CR1 |= 0x01;
            }
        }
    }
    TIM5->SR = 0;
    Motor_pulse_get();
    grid_counter++;
}




















