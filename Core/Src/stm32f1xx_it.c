#include "main.h"
#include "stm32f1xx_it.h"
#include "app_system.h"
#include "app_motor.h"
#include "bsp_tim.h"
#include "bsp_uart.h"
#include "bsp_adc.h"
#include "bsp_can.h"

/* Cortex-M3 fault handlers */
void NMI_Handler(void)           { while (1); }
void HardFault_Handler(void)     { while (1); }
void MemManage_Handler(void)     { while (1); }
void BusFault_Handler(void)      { while (1); }
void UsageFault_Handler(void)    { while (1); }
void SVC_Handler(void)           {}
void DebugMon_Handler(void)      {}
void PendSV_Handler(void)        {}

/* System tick (1ms) */
void SysTick_Handler(void)
{
    HAL_IncTick();
    APP_System_TickInc();
}

/* ---- Peripheral ISRs ---- */

/* Hall sensor EXTI: PB1 → EXTI1, PB4 → EXTI4, PC11 → EXTI15_10 */
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    uint8_t hall = 0;
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)) hall |= 0x04;
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)) hall |= 0x02;
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11)) hall |= 0x01;
    APP_Motor_Hall_ISR(hall);
}

/* TIM1 */
void TIM1_BRK_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

void TIM1_UP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1);
}

/* TIM3 Encoder */
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

void ADC1_2_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hadc1);
}

void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX1_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_SCE_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}
