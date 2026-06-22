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
#include "app_foc.h"

#define FOC_MODE  1   /* 0=6-step, 1=FOC */

volatile uint32_t pb8_cnt = 0;

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

    /* PA4/PA5 = input + pull-down → quiet 0V threshold */
    {
        GPIO_InitTypeDef g = {0};
        g.Mode = GPIO_MODE_INPUT;
        g.Pull = GPIO_NOPULL;   /* external resistor divider sets 2V */
        g.Pin  = GPIO_PIN_4 | GPIO_PIN_5;
        HAL_GPIO_Init(GPIOA, &g);
    }

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
    APP_LED_Set(1, LED_ON);   /* green solid */

    APP_Motor_SetSpeed(50);

    /* PB8 EXTI input */
    { GPIO_InitTypeDef g = {0}; g.Mode = GPIO_MODE_IT_FALLING; g.Pull = GPIO_PULLUP;
      g.Pin = PB8_PIN; HAL_GPIO_Init(PB8_PORT, &g); }

    uint32_t last = 0;
    uint8_t  running = 0;
    uint8_t  broken  = 0;
    uint8_t  brk_ch  = 0;
    uint8_t  pb4_prev = 0;

    while (1) {
        APP_System_Run();
        APP_LED_Run();
        /* Overcurrent: same stop as yarn break — no brake, no short */
        #define OC_THR  3500
        {
            static uint16_t i_max = 0;
            uint16_t i0 = (uint16_t)BSP_ADC_GetChannel(ADC_CH_CURRENT_U);
            uint16_t i1 = (uint16_t)BSP_ADC_GetChannel(ADC_CH_CURRENT_V);
            uint16_t i2 = (uint16_t)BSP_ADC_GetChannel(ADC_CH_CURRENT_W);
            uint16_t im  = i0; if (i1 > im) im = i1; if (i2 > im) im = i2;
            if (im > i_max) i_max = im;
            /* if (running) printf("I=%4u/%4u/%4u MAX=%4u\r\n", i0, i1, i2, i_max); */
            if (im > OC_THR && running) {
                if(FOC_MODE) FOC_Stop(); else {
                    HAL_GPIO_WritePin(LS_U_PORT, LS_U_PIN, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LS_V_PORT, LS_V_PIN, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LS_W_PORT, LS_W_PIN, GPIO_PIN_RESET);
                    APP_Motor_Stop();
                }
                running = 0; broken = 1; brk_ch = 9;
            }
        }

        if(FOC_MODE){if(running)FOC_Run();} else APP_Motor_Run();
        APP_Sensor_Run();
        APP_Hall_Update();

        /* CH=1: YELLOW blink — both LEDs in sync */
        static uint32_t ch1_t = 0; static uint8_t ch1_ph = 0;
        if (broken && brk_ch == 1) {
            if (APP_System_GetTick() - ch1_t >= 500) { ch1_t = APP_System_GetTick(); ch1_ph ^= 1; }
            if (ch1_ph) { APP_LED_Set(0, LED_ON); APP_LED_Set(1, LED_ON); }   /* YELLOW */
            else        { APP_LED_Set(0, LED_OFF); APP_LED_Set(1, LED_OFF); }  /* OFF */
        }

        /* CH=3: RED↔YELLOW alternate */
        static uint32_t ch3_t = 0; static uint8_t ch3_ph = 0;
        if (broken && brk_ch == 3) {
            if (APP_System_GetTick() - ch3_t >= 500) { ch3_t = APP_System_GetTick(); ch3_ph ^= 1; }
            if (ch3_ph) { APP_LED_Set(0, LED_ON); APP_LED_Set(1, LED_OFF); }  /* RED */
            else        { APP_LED_Set(0, LED_ON); APP_LED_Set(1, LED_ON);  }  /* YELLOW */
        }

        if (APP_System_GetTick() - last >= 100) {
            last = APP_System_GetTick();

            uint8_t  trig    = APP_Hall_IsTriggered();
            uint32_t revs    = APP_Hall_GetRevs();
            uint8_t  pb4     = APP_Hall_GetPB4();

            /* PB4 edge → clear alarm */
            if (pb4 != pb4_prev && broken) { broken = 0; brk_ch = 0; }
            pb4_prev = pb4;

            /* Hall trigger */
            if (trig && !running && !broken) {
                if(FOC_MODE) FOC_Init(); else APP_Motor_Start();
                APP_Sensor_ResetPulse(); running = 1;
            } else if (!trig && running) {
                if(FOC_MODE) FOC_Stop(); else APP_Motor_Stop();
                running = 0;
            }

            if (!trig) { broken = 0; brk_ch = 0; }

            /* Yarn break */
            if (running) {
                brk_ch = APP_Sensor_WhichBroken();
                if (brk_ch) {
                    if(FOC_MODE) FOC_Stop(); else APP_Motor_Stop();
                    running = 0; broken = 1;
                }
            }

            /* LED: CH=2 red, CH=9 fast-red, OK=green, CH=1+3 in 1ms loop */
            if (brk_ch == 9) { APP_LED_Set(0, LED_FAST_BLINK); APP_LED_Set(1, LED_OFF); }
            else if (broken && brk_ch == 2) { APP_LED_Set(0, LED_SLOW_BLINK); APP_LED_Set(1, LED_OFF); }
            else if (!broken) { APP_LED_Set(0, LED_OFF); APP_LED_Set(1, LED_ON); }

            uint16_t i_u = (uint16_t)BSP_ADC_GetChannel(ADC_CH_CURRENT_U);
            uint16_t i_v = (uint16_t)BSP_ADC_GetChannel(ADC_CH_CURRENT_V);
            uint16_t i_w = (uint16_t)BSP_ADC_GetChannel(ADC_CH_CURRENT_W);
            uint16_t i_mx = i_u; if (i_v > i_mx) i_mx = i_v; if (i_w > i_mx) i_mx = i_w;

            uint16_t i0=BSP_ADC_GetChannel(0),i1=BSP_ADC_GetChannel(1),i2=BSP_ADC_GetChannel(2);
            printf("SPD=%d I=%u/%u/%u REVS=%lu %s\r\n",
                   APP_Motor_GetSpeed(),i0,i1,i2,
                   revs,running?"ON":"OFF");
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
