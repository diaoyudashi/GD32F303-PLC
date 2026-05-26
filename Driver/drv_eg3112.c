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

void EG3112_SetPhaseDuty(uint8_t u, uint8_t v, uint8_t w)
{
    BSP_TIM1_SetDuty(u, v, w);
}
