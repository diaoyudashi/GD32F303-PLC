#include "main.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay(uint32_t ms) { uint32_t s = tick; while ((tick - s) < ms); }
int main(void) {
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    BSP_USART_Init();
    LED_ERR_OFF;
    { uint8_t j[64]; BSP_USART_GetRxData(COM_PLC, j, 64); }
    uint32_t rx_total = 0, ack_cnt = 0;
    while (1) {
        /* 每次最多处理 32 字节，防止 RX 噪声淹死主循环 */
        uint8_t d;
        for (int i = 0; i < 32; i++) {
            if (!BSP_USART_GetRxData(COM_PLC, &d, 1)) break;
            rx_total++;
            if (d == 0x05) { BSP_USART_SendByte(COM_PLC, 0x06); ack_cnt++; }
        }
        /* 慢闪 + 收到数据快闪 */
        if (ack_cnt > 0) {
            LED_RUN_ON;  delay(100);
            LED_RUN_OFF; delay(100);
            ack_cnt = 0;
        } else if (rx_total > 0) {
            LED_RUN_ON;  delay(300);
            LED_RUN_OFF; delay(300);
            rx_total = 0;
        } else {
            LED_RUN_ON;  delay(500);
            LED_RUN_OFF; delay(500);
        }
    }
}
