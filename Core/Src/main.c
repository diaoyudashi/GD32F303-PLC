/**
 * @file    main.c
 * @brief   GX Works Õ®—∂≤‚ ‘ °™ USART1 19200 Œ’ ÷
 */

#include "main.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"

volatile uint32_t g_tick_ms = 0;
volatile uint8_t  gx_ok = 0;

void SysTick_Handler(void) { g_tick_ms++; }
void delay_ms(uint32_t ms) { uint32_t s = g_tick_ms; while ((g_tick_ms - s) < ms); }

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    BSP_USART_Init();

    LED_ERR_OFF;

    while (1) {
        uint8_t d;
        while (BSP_USART_GetRxData(COM_PLC, &d, 1) > 0) {
            if (d == 0x05 || d == 0x02) {
                BSP_USART_SendByte(COM_PLC, 0x06);
                gx_ok = 1;
            }
        }
        if (gx_ok) {
            LED_RUN_ON;  delay_ms(100);
            LED_RUN_OFF; delay_ms(100);
        } else {
            LED_RUN_ON;  delay_ms(500);
            LED_RUN_OFF; delay_ms(500);
        }
    }
}
