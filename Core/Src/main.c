#include "main.h"
#include "bsp_gpio.h"
#include "gxworks.h"

volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay(uint32_t ms) { uint32_t s = tick, limit = ms * 10; while ((tick - s) < ms && --limit); }

#define U0_BASE  0x40013800U
#define U0_SR    (*(volatile uint32_t *)(U0_BASE + 0x00))
#define U0_DR    (*(volatile uint32_t *)(U0_BASE + 0x04))
#define U0_BRR   (*(volatile uint32_t *)(U0_BASE + 0x08))
#define U0_CR1   (*(volatile uint32_t *)(U0_BASE + 0x0C))
#define U0_CR2   (*(volatile uint32_t *)(U0_BASE + 0x10))
#define CR1_UE   (1U<<13)
#define CR1_TE   (1U<<3)
#define CR1_RE   (1U<<2)
#define SR_RXNE  (1U<<5)
#define SR_TXE   (1U<<7)

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    LED_ERR_OFF;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_AF_PP,      GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    U0_BRR = 0x00000EA6;
    U0_CR2 = 0;
    U0_CR1 = CR1_TE | CR1_RE;
    U0_CR1 |= CR1_UE;

    while (1) {
        /* ���� �� ι��Э�鴦���� */
        if (U0_SR & SR_RXNE) {
            GXWorks_FeedByte((uint8_t)U0_DR);
        }
        /* ���� �� Э����Ӧ */
        GXWorks_SendTx();

        LED_RUN_ON;  delay(100);
        LED_RUN_OFF; delay(100);
    }
}
