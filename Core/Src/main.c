#include "main.h"
#include <stdio.h>
#include "bsp_clock.h"
#include "bsp_gpio.h"
#include "bsp_tim.h"
#include "bsp_adc.h"
#include "bsp_uart.h"
#include "drv_tpl0501.h"
#include "app_system.h"
#include "app_led.h"
#include "app_motor.h"

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
    return ch;
}

int main(void)
{
    HAL_Init();
    BSP_Clock_Init();
    BSP_GPIO_Init();
    BSP_USART2_Debug_Init();

    printf("\r\n--- BOOT ---\r\n");

    TPL0501_Init();
    TPL0501_Set(255, 255);
    printf("POT max gain\r\n");

    BSP_ADC1_Init();
    BSP_ADC1_Start_DMA();
    printf("ADC started\r\n");

    BSP_TIM1_PWM_Init();
    BSP_TIM3_Encoder_Init();
    APP_Motor_Init();
    printf("Motor ready\r\n");

    APP_System_Init();
    APP_LED_Init();
    APP_LED_Set(0, LED_SLOW_BLINK);

    APP_Motor_SetSpeed(50);  /* default duty */

    uint32_t last = 0;
    uint8_t  running = 0;

    while (1) {
        APP_System_Run();
        APP_LED_Run();
        APP_Motor_Run();

        if (APP_System_GetTick() - last >= 200) {
            last = APP_System_GetTick();

            uint32_t raw1 = BSP_ADC_GetChannel(ADC_CH_PHOTO1);
            uint32_t raw2 = BSP_ADC_GetChannel(ADC_CH_PHOTO2);

            #define THR  3950
            uint8_t tri1 = (raw1 < THR);
            uint8_t tri2 = (raw2 < THR);

            if ((tri1 || tri2) && !running) {
                APP_Motor_Start();
                running = 1;
            } else if (!tri1 && !tri2 && running) {
                APP_Motor_Stop();
                running = 0;
            }

            printf("S1=%4lu S2=%4lu  MTR=%s  H=%d  I=%u\r\n",
                   raw1, raw2,
                   running ? "ON" : "OFF",
                   APP_Motor_GetHall(),
                   APP_Motor_GetCurrent());
        }
    }
}

void Error_Handler(void)
{
    __disable_irq();
    APP_LED_Set(0, LED_FAST_BLINK);
    APP_LED_Set(1, LED_FAST_BLINK);
    while (1) APP_LED_Run();
}
