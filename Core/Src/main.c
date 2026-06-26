/**
 * @file    main.c
 * @brief   GX Works 通讯测试 + 完整协议处理
 */

#include "main.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "gxworks.h"

volatile uint32_t g_tick_ms = 0;
void SysTick_Handler(void) { g_tick_ms++; }
void delay_ms(uint32_t ms) { uint32_t s = g_tick_ms; while ((g_tick_ms - s) < ms); }

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    BSP_USART_Init();

    LED_ERR_OFF;
    { uint8_t junk[64]; BSP_USART_GetRxData(COM_PLC, junk, 64); }

    while (1) {
        GXWorks_Poll();
        LED_RUN_ON;  delay_ms(500);
        LED_RUN_OFF; delay_ms(500);
    }
}
