#ifndef __BSP_TIMEBASE_H
#define __BSP_TIMEBASE_H

#include "stm32f10x.h"

/********************基本定时器TIM参数定义，只限TIM6、7************/
//#define BASIC_TIM6 // 如果使用TIM7，注释掉这个宏即可

//#ifdef  BASIC_TIM6 // 使用基本定时器TIM6
#define            BASIC_TIM                   TIM6
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM6
#define            BASIC_TIM_Period            (1000-1)
#define            BASIC_TIM_Prescaler         71
#define            BASIC_TIM_IRQ               TIM6_IRQn
#define            BASIC_TIM_IRQHandler        TIM6_IRQHandler


#define            BASIC_TIM_2                   TIM7
//#define            BASIC_TIM_APBxClock_FUN_2     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK_2              RCC_APB1Periph_TIM7
#define            BASIC_TIM_Period_2            1000-1
#define            BASIC_TIM_Prescaler_2         71
#define            BASIC_TIM_IRQ_2               TIM7_IRQn
#define            BASIC_TIM_IRQHandler_2        TIM7_IRQHandler




void BASIC_TIM_Init(void);

#endif	/* __BSP_TIMEBASE_H */