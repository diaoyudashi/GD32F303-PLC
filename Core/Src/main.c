#include "main.h"
#include "bsp_gpio.h"
volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay(uint32_t ms) { uint32_t s = tick; while ((tick - s) < ms); }

#define U0_BASE  0x40013800U
#define U0_SR    (*(volatile uint32_t *)(U0_BASE + 0x00))
#define U0_DR    (*(volatile uint32_t *)(U0_BASE + 0x04))
#define U0_BRR   (*(volatile uint32_t *)(U0_BASE + 0x08))
#define U0_CR1   (*(volatile uint32_t *)(U0_BASE + 0x0C))
#define U0_CR2   (*(volatile uint32_t *)(U0_BASE + 0x10))
#define U0_CR3   (*(volatile uint32_t *)(U0_BASE + 0x14))
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

    /* === ��ȫ���� STM32 USART_Init ˳�� === */
    U0_BRR = 0x00000EA6;       /* 72MHz/19200 */
    U0_CR2 = 0x0000;           /* Ĭ�� */
    U0_CR3 = 0x0000;           /* Ĭ�� */
    U0_CR1 = CR1_TE | CR1_RE;  /* Step1: TX+RX, UE=0 (ƥ�� USART_Init) */
    U0_CR1 |= CR1_UE;          /* Step2: ��ʹ�� (ƥ�� USART_Cmd) */

    while (1) {
        if (U0_SR & SR_RXNE) {
            uint8_t d = (uint8_t)U0_DR;
            if (d == 0x05 && (U0_SR & SR_TXE)) {
                U0_DR = 0x06;       /* 非阻塞 TX */
            }
        }
        LED_RUN_ON;  delay(500);
        LED_RUN_OFF; delay(500);
    }
}
