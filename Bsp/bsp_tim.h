#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim1;  /* BLDC 6-step PWM */
extern TIM_HandleTypeDef htim2;  /* PWM DAC for photo threshold */
extern TIM_HandleTypeDef htim3;  /* Encoder */

void BSP_TIM1_PWM_Init(void);
void BSP_TIM1_PWM_Start(void);
void BSP_TIM1_PWM_StartFOC(void);  /* all 6 complementary outputs */
void BSP_TIM1_PWM_Stop(void);

void BSP_TIM2_PWMDAC_Init(void);
void BSP_TIM2_SetDAC_CH3(uint16_t val);  /* PA4, photo1 threshold */
void BSP_TIM2_SetDAC_CH4(uint16_t val);  /* PA5, photo2 threshold */

void BSP_TIM3_Encoder_Init(void);
int32_t BSP_TIM3_GetEncoderCount(void);
void BSP_TIM3_ResetEncoder(void);

void BSP_Hall_EXTI_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_TIM_H */
