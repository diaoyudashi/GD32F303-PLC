/**
 * @file    bsp_gpio.c
 * @brief   GD32F303 PLC 全部 GPIO 初始化
 * @note    35路输入 + 23路输出 + 2路LED + 3串口 + 1 DAC
 */

#include "bsp_gpio.h"

void BSP_GPIO_Init(void)
{
    /* ==================== 时钟使能 ==================== */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_AF);     /* 复用时钟(串口/TIM) */

    /* ==================== 输出: 先设默认电平, 再配模式 ==================== */

    /* 设备控制 (PB/PG) */
    gpio_bit_reset(GPIOB, GPIO_PIN_9);   /* Y00 风扇    默认关 */
    gpio_bit_reset(GPIOB, GPIO_PIN_8);   /* Y01 上布灯  默认关 */
    gpio_bit_reset(GPIOB, GPIO_PIN_5);   /* Y02 下布灯  默认关 */
    gpio_bit_reset(GPIOG, GPIO_PIN_15);  /* Y03 油机    默认关 */
    gpio_bit_reset(GPIOG, GPIO_PIN_14);  /* Y04 开幅    默认关 */
    gpio_bit_reset(GPIOG, GPIO_PIN_13);  /* Y05 变频器通电 默认关 */
    gpio_bit_reset(GPIOG, GPIO_PIN_12);  /* Y06 中央除尘 默认关 */
    gpio_bit_reset(GPIOG, GPIO_PIN_11);  /* Y07 喷气吹油 默认关 */

    /* 断纱输出 (PC/PE) */
    gpio_bit_reset(GPIOC, GPIO_PIN_13);  /* Y10 上断纱输出 默认关 */
    gpio_bit_reset(GPIOE, GPIO_PIN_6);   /* Y11 中断纱输出 默认关 */
    gpio_bit_reset(GPIOE, GPIO_PIN_5);   /* Y12 下断纱输出 默认关 */

    /* 三色灯 + 吹气 (PE) */
    gpio_bit_reset(GPIOE, GPIO_PIN_4);   /* Y13 三色灯R 默认关 */
    gpio_bit_reset(GPIOE, GPIO_PIN_3);   /* Y14 三色灯G 默认关 */
    gpio_bit_reset(GPIOE, GPIO_PIN_2);   /* Y15 三色灯Y 默认关 */
    gpio_bit_reset(GPIOE, GPIO_PIN_1);   /* Y16 门口吹气 默认关 */
    gpio_bit_reset(GPIOE, GPIO_PIN_0);   /* Y17 中央吹气 默认关 */

    /* 变频器 (PF) */
    gpio_bit_reset(GPIOF, GPIO_PIN_13);  /* Y20 变频器运行 默认关 */
    gpio_bit_reset(GPIOF, GPIO_PIN_14);  /* Y21 备用     默认关 */
    gpio_bit_reset(GPIOF, GPIO_PIN_15);  /* Y22 备用     默认关 */

    /* 系统指示灯 (PF9/PF10, 低电平亮, 默认灭=高) */
    gpio_bit_set(GPIOF, GPIO_PIN_9);     /* PF9  错误灯 默认灭 */
    gpio_bit_set(GPIOF, GPIO_PIN_10);    /* PF10 运行灯 默认灭 */

    /* ---- 配置输出模式: 推挽 50MHz ---- */
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,
              GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,
              GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,
              GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,
              GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* ==================== 输入: 上拉(接常开触点) ==================== */

    /* 伺服差分脉冲 (PA/PD/PC) — 输入捕获, 浮空或上拉 */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,
              GPIO_PIN_1 | GPIO_PIN_12);
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,
              GPIO_PIN_2 | GPIO_PIN_13);
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, GPIO_PIN_7);

    /* 开关量输入 — 上拉 (常开触点, 闭合=低电平) */
    gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_9);   /* X05 COUNT */
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_8);   /* X06 SafeDoor */
    gpio_init(GPIOG, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_7);   /* X07 RUN */
    gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_8);   /* X10 STOP */
    gpio_init(GPIOG, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_4);   /* X11 JOB */
    gpio_init(GPIOG, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_5);   /* X12 变频器故障 */

    /* 伺服故障 (PE/PG) — 上拉 (故障=低电平) */
    gpio_init(GPIOE, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ,
              GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_init(GPIOG, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_6);   /* X17 伺服5故障 */

    /* 断电检测 */
    gpio_init(GPIOG, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_3);   /* X20 PVD */

    /* 故障输入 — 上拉 */
    gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ,
              GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);  /* X21保险 X22探针 X27断纱 */
    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ,
              GPIO_PIN_0 | GPIO_PIN_1);                /* X23探布 X24缺油 */
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ,
              GPIO_PIN_6 | GPIO_PIN_7);                /* X25上断纱 X26中断纱 */
    gpio_init(GPIOF, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_11);  /* X30缺气 */

    /* ==================== 串口 AF 配置 (在 bsp_uart.c 中详细配置) ==================== */
    /* 这里只做占位, 实际由 BSP_USART_Init() 接管 */
}
