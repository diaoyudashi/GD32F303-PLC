#include "drv_eg3112.h"
#include "bsp_tim.h"

void EG3112_Enable(void)
{
    BSP_TIM1_PWM_Start();
}

void EG3112_Disable(void)
{
    BSP_TIM1_PWM_Stop();
}

void EG3112_Brake(void)
{
    /* Force TIM1 brake → all outputs go to idle state (low) */
    __HAL_TIM_MOE_DISABLE(&htim1);
}
