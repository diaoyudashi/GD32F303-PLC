/**
 * @file    main.c
 * @brief   GX Works 通讯测试 — RX上拉 + 缓冲清理
 */

#include "main.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"

volatile uint32_t g_tick_ms = 0;
void SysTick_Handler(void) { g_tick_ms++; }
void delay_ms(uint32_t ms) { uint32_t s = g_tick_ms; while ((g_tick_ms - s) < ms); }

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    BSP_USART_Init();

    LED_ERR_OFF;

    /* 清理 UART 初始化时可能收到的浮空噪点 */
    { uint8_t junk[64]; BSP_USART_GetRxData(COM_PLC, junk, 64); }
    delay_ms(100);

    uint8_t gx_ok = 0;
    while (1) {
        uint8_t d;
        while (BSP_USART_GetRxData(COM_PLC, &d, 1) > 0) {
            if (d == 0x05) {
                BSP_USART_SendByte(COM_PLC, 0x06);
                gx_ok = 1;
            }
        }

        if (gx_ok) {
            LED_RUN_ON;  delay_ms(200);
            LED_RUN_OFF; delay_ms(200);
        } else {
            LED_RUN_ON;  delay_ms(500);
            LED_RUN_OFF; delay_ms(500);
        }
    }
}
