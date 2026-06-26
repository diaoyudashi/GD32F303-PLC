/**
 * @file    main.c
 * @brief   GX Works 通讯测试 — USART1 握手验证
 *
 *  协议: GX Works 发 0x05(ENQ) -> MCU 回 0x06(ACK)
 *  通过: LED_RUN 快闪 (通讯测试成功)
 *  未通: LED_RUN 1Hz 慢闪 (等待连接)
 */

#include "main.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"

volatile uint32_t g_tick_ms = 0;
volatile uint8_t  gx_connected = 0;  /* 收到过 ENQ 就置1 */

void SysTick_Handler(void) { g_tick_ms++; }
void delay_ms(uint32_t ms) {
    uint32_t s = g_tick_ms;
    while ((g_tick_ms - s) < ms);
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    BSP_USART_Init();

    LED_ERR_OFF;

    while (1) {
        /* 检查 USART1 是否收到过 GX Works 的 0x05 */
        uint8_t d;
        while (BSP_USART_GetRxData(COM_PLC, &d, 1) > 0) {
            if (d == 0x05) {
                /* ENQ — 回复 ACK */
                BSP_USART_SendByte(COM_PLC, 0x06);
                gx_connected = 1;
            }
            /* STX (0x02) — 数据帧开始, 也回复 ACK */
            else if (d == 0x02) {
                BSP_USART_SendByte(COM_PLC, 0x06);
                gx_connected = 1;
            }
        }

        if (gx_connected) {
            /* 通讯成功 — 快闪 */
            LED_RUN_ON;  delay_ms(100);
            LED_RUN_OFF; delay_ms(100);
        } else {
            /* 等待连接 — 慢闪 */
            LED_RUN_ON;  delay_ms(500);
            LED_RUN_OFF; delay_ms(500);
        }
    }
}
