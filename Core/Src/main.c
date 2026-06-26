/**
 * @file    main.c
 * @brief   GD32F303 PLC 主调度
 * @note    扫描周期: SystemInit → 外设初始化 → PLC 扫描循环
 */

#include "main.h"
#include "bsp_gpio.h"

volatile uint32_t g_tick_ms = 0;  /* 1ms 系统时基 */

void SysTick_Handler(void) { g_tick_ms++; }

void delay_ms(uint32_t ms) {
    uint32_t s = g_tick_ms;
    while ((g_tick_ms - s) < ms);
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);

    /* ---- BSP 初始化 ---- */
    BSP_GPIO_Init();
    /* BSP_USART_Init(); */
    /* BSP_TIM_Init();   */
    /* BSP_DAC_Init();   */

    /* 上电开机: 运行灯亮, 错误灯灭 */
    LED_RUN_ON;
    LED_ERR_OFF;

    while (1) {
        /* ---- PLC 扫描调度 (待实现) ---- */
        // PLC_IO_Refresh();
        // PLC_ProInstructParse();
        // MODBUS_Poll();
        // GXWorks_Poll();

        /* 心跳: 运行灯 1Hz 闪烁 */
        LED_RUN_ON;
        delay_ms(500);
        LED_RUN_OFF;
        delay_ms(500);
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1);
}
