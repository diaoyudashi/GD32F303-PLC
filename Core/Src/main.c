/**
 * @file    main.c
 * @brief   串口环回测试 — 验证 USART1 硬件链路
 *
 * 测试方法: 用杜邦线短接 PA9(TX) 和 PA10(RX)
 * 结果:    收到回环数据 → LED_RUN 快闪 (50ms)
 *          超时未收到   → LED_ERR 常亮
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

    LED_RUN_OFF;
    LED_ERR_OFF;

    /* ==== USART1 环回测试 ==== */

    /* 发送测试字节 */
    uint8_t test_byte = 0xA5;
    BSP_USART_SendByte(COM_PLC, test_byte);

    /* 等待接收回环数据 (超时 500ms) */
    uint32_t t0 = g_tick_ms;
    uint8_t rx_byte = 0;
    uint8_t rx_cnt = 0;

    while ((g_tick_ms - t0) < 500) {
        if (BSP_USART_GetRxData(COM_PLC, &rx_byte, 1) > 0) {
            rx_cnt = 1;
            break;
        }
    }

    /* ==== 结果判断 ==== */
    if (rx_cnt > 0 && rx_byte == test_byte) {
        /* 环回成功 — USART1 硬件 OK — LED_RUN 快闪 */
        while (1) {
            LED_RUN_ON;  delay_ms(50);
            LED_RUN_OFF; delay_ms(50);
        }
    } else {
        /* 环回失败 — LED_ERR 常亮 */
        LED_ERR_ON;
        while (1);
    }
}
