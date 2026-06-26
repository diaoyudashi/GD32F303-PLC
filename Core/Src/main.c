/**
 * @file    main.c
 * @brief   USART1 TX 测试 — PA9 持续发送 0x55 (01010101 方波)
 *
 *  万用表测 PA9: 应该看到 ~1.6V 左右 (50%占空比方波平均值)
 *  示波器: 应该看到 19.2kHz 方波
 *  LED_RUN: 1Hz 闪烁 = 程序在跑
 */

#include "main.h"
#include "bsp_gpio.h"

volatile uint32_t g_tick_ms = 0;
void SysTick_Handler(void) { g_tick_ms++; }
void delay_ms(uint32_t ms) { uint32_t s = g_tick_ms; while ((g_tick_ms - s) < ms); }

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    LED_ERR_OFF;

    /* USART1: PA9(TX) AF_PP, 持续发送 */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART1);
    rcu_periph_clock_enable(RCU_AF);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    usart_deinit(USART1);
    usart_baudrate_set(USART1, 19200);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_enable(USART1);

    while (1) {
        /* 持续发送 0x55 = 01010101, 产生方波方便测量 */
        usart_data_transmit(USART1, 0x55);
        while (!usart_flag_get(USART1, USART_FLAG_TBE));

        LED_RUN_ON;  delay_ms(500);
        LED_RUN_OFF; delay_ms(500);
    }
}
