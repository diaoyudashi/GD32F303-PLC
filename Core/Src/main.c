/**
 * @file    main.c
 * @brief   GD32F303 PLC 主调度
 * @note    扫描周期: SystemInit → 外设初始化 → PLC 扫描循环
 */

#include "main.h"

volatile uint32_t g_tick_ms = 0;  /* 1ms 系统时基 */

/* ---- SysTick ---- */
void SysTick_Handler(void)
{
    g_tick_ms++;
}

/* ---- 入口 ---- */
int main(void)
{
    /* SysTick: 1ms 时基 */
    SysTick_Config(SystemCoreClock / 1000);

    /* TODO: BSP 初始化 */
    // BSP_GPIO_Init();
    // BSP_USART_Init();
    // BSP_TIM_Init();
    // BSP_DAC_Init();

    /* 最小验证: PB8+PB9 1Hz 闪烁 */
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9);

    while (1) {
        /* TODO: PLC 扫描调度 */
        // PLC_IO_Refresh();
        // PLC_ProInstructParse();
        // MODBUS_Poll();
        // GXWorks_Poll();

        gpio_bit_set(GPIOB,   GPIO_PIN_8 | GPIO_PIN_9);
        for (volatile uint32_t i = 0; i < 36000000; i++) { __NOP(); /* ~500ms @120MHz */ }
        gpio_bit_reset(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
        for (volatile uint32_t i = 0; i < 36000000; i++) { __NOP(); }
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1);
}
