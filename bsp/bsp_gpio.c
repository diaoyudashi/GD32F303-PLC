#include "gd32f30x.h"

void BSP_GPIO_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOG);

    /* LED: PF9(ERR), PF10(RUN) */
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_bit_set(GPIOF, GPIO_PIN_9 | GPIO_PIN_10);

    /* X 输入 — 上拉 */
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_12);
    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_2 | GPIO_PIN_13);
    gpio_init(GPIOE, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_init(GPIOF, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_11);
    gpio_init(GPIOG, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    /* Y 输出 — 默认关 */
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}
