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
#include "app_hall.h"
#include "app_sensor.h"

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

    printf("POT max gain\r\n");

    BSP_ADC1_Init();
    BSP_ADC1_Start_DMA();
    printf("ADC started\r\n");

    /* BSP_TIM2_PWMDAC_Init();  TODO: fix TIM2 hang */

    BSP_TIM1_PWM_Init();
    printf("TIM1 OK\r\n");

    BSP_TIM3_Encoder_Init();
    printf("TIM3 OK\r\n");

    APP_Motor_Init();
    printf("Motor OK\r\n");

    APP_Sensor_Init();
    printf("Sensor OK\r\n");

    APP_Hall_Init();
    printf("Hall OK\r\n");

    printf("Motor ready\r\n");

    APP_System_Init();
    APP_LED_Init();
    APP_LED_Set(0, LED_ON);   /* LED1 = RED off */
    APP_LED_Set(0, LED_OFF);
    APP_LED_Set(1, LED_ON);   /* LED2 = GREEN solid */

    APP_Motor_SetSpeed(50);

    uint32_t last = 0;
    uint8_t  running = 0;
    uint8_t  broken = 0;

    while (1) {
        APP_System_Run();
        APP_LED_Run();
        APP_Motor_Run();
        APP_Sensor_Run();
        APP_Hall_Update();

        if (APP_System_GetTick() - last >= 100) {
            last = APP_System_GetTick();

            uint16_t pb1_adc = APP_Hall_GetPB1_ADC();
            uint8_t  trig    = APP_Hall_IsTriggered();
            uint32_t revs    = APP_Hall_GetRevs();

            /* Hall trigger: start/stop motor */
            if (trig && !running && !broken) { APP_Motor_Start(); running = 1; }
            else if (!trig && running)       { APP_Motor_Stop();  running = 0; }

            /* Yarn break: motor running + no pulse >250ms */
            if (running && APP_Sensor_Broken()) {
                APP_Motor_Stop();
                running = 0;
                broken  = 1;
                APP_LED_Set(0, LED_SLOW_BLINK);  /* RED 1Hz blink */
                APP_LED_Set(1, LED_OFF);
            }

            /* Recover: Hall trigger clears again */
            if (broken && !trig) {
                broken = 0;
                APP_LED_Set(0, LED_OFF);
                APP_LED_Set(1, LED_ON);  /* GREEN solid */
            }

            uint16_t p1_a = APP_Sensor1_GetAnalog();
            uint16_t p2_a = APP_Sensor2_GetAnalog();
            uint8_t  p1_d = APP_Sensor1_GetOutput();
            uint8_t  p2_d = APP_Sensor2_GetOutput();

            printf("PB1=%4u T=%d REVS=%lu | P1=%d A=%4u P2=%d A=%4u | BRK=%d MTR=%s\r\n",
                   pb1_adc, trig, revs,
                   p1_d, p1_a, p2_d, p2_a,
                   broken,
                   running ? "ON":"OFF");
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
