/**
 * @file    main.c
 * @brief   逐步诊断 — LED 信号告诉你卡在哪一步
 *
 *  LED_RUN:
 *    1Hz 慢闪 = BSP_GPIO OK
 *    2Hz      = BSP_USART_Init OK (到主循环了)
 *
 *  LED_ERR:
 *    常亮 = BSP_USART_Init 卡死了
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

    /* 阶段1: GPIO 初始化 */
    BSP_GPIO_Init();
    LED_ERR_OFF;

    /* 验证: 如果程序到这, LED_RUN 慢闪 3 秒 */
    for (int i = 0; i < 3; i++) {
        LED_RUN_ON;  delay_ms(500);
        LED_RUN_OFF; delay_ms(500);
    }

    /* 阶段2: UART 初始化 (如果卡死, LED_ERR 不会亮) */
    BSP_USART_Init();
    LED_ERR_ON;   /* <- 到了这里说明 UART 初始化没卡死 */
    delay_ms(200);
    LED_ERR_OFF;

    /* 阶段3: 主循环 GX Works 握手 */
    uint8_t gx_ok = 0;
    while (1) {
        uint8_t d;
        while (BSP_USART_GetRxData(COM_PLC, &d, 1) > 0) {
            if (d == 0x05 || d == 0x02) {
                BSP_USART_SendByte(COM_PLC, 0x06);
                gx_ok = 1;
            }
        }
        if (gx_ok) {
            LED_RUN_ON;  delay_ms(50);
            LED_RUN_OFF; delay_ms(50);
        } else {
            LED_RUN_ON;  delay_ms(250);
            LED_RUN_OFF; delay_ms(250);
        }
    }
}
