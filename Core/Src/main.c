/**
 * @file    main.c
 * @brief   串口验证 — USART1 持续发送, 串口助手观察
 *
 *  连接: PA9(TX) -> 串口工具 RX (USB转TTL或RS232)
 *  配置: 19200 8N1
 *  结果: 每秒输出一次 "GD32F303-PLC USART1 OK [xxx]"
 *        LED_RUN 1Hz 闪烁
 */

#include "main.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"

volatile uint32_t g_tick_ms = 0;
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

    uint32_t cnt = 0;
    char buf[64];

    while (1) {
        /* 运行灯 1Hz 闪烁 */
        LED_RUN_ON;  delay_ms(500);
        LED_RUN_OFF; delay_ms(500);

        /* USART1 输出测试信息 */
        int len = 0;
        for (int i = 0; "GD32F303-PLC USART1 OK ["[i]; i++)
            buf[len++] = "GD32F303-PLC USART1 OK ["[i];
        uint32_t n = cnt;
        char num[10]; int ni = 0;
        do { num[ni++] = '0' + (n % 10); n /= 10; } while (n);
        while (ni) buf[len++] = num[--ni];
        buf[len++] = ']';
        buf[len++] = '\r';
        buf[len++] = '\n';

        BSP_USART_SendBuf(COM_PLC, (uint8_t*)buf, len);
        cnt++;
    }
}
