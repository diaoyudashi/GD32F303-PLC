/**
 * @file    main.c
 * @brief   USART0 ���Ĵ������� �� �ƿ�GD32��, ��ȫ����STM32�ο�
 */
#include "main.h"
#include "bsp_gpio.h"

volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay(uint32_t ms) { uint32_t s = tick; while ((tick - s) < ms); }

/* USART0 �Ĵ��� (0x40013800) */
#define U0_BASE  0x40013800U
#define U0_SR    (*(volatile uint32_t *)(U0_BASE + 0x00))
#define U0_DR    (*(volatile uint32_t *)(U0_BASE + 0x04))
#define U0_BRR   (*(volatile uint32_t *)(U0_BASE + 0x08))
#define U0_CR1   (*(volatile uint32_t *)(U0_BASE + 0x0C))
#define U0_CR2   (*(volatile uint32_t *)(U0_BASE + 0x10))
#define U0_CR3   (*(volatile uint32_t *)(U0_BASE + 0x14))

/* λ���� */
#define CR1_UE   (1U<<13)
#define CR1_TE   (1U<<3)
#define CR1_RE   (1U<<2)
#define CR1_RXNEIE (1U<<5)
#define SR_RXNE  (1U<<5)
#define SR_TXE   (1U<<7)
#define SR_TC    (1U<<6)

static volatile uint8_t ack_pending = 0;

void USART0_IRQHandler(void)
{
    if (U0_SR & SR_RXNE) {
        uint8_t d = (uint8_t)U0_DR;
        if (d == 0x05 && !ack_pending) {
            /* ENQ -> ACK, 不等待, TX 自行完成 */
            U0_DR = 0x06;
            ack_pending = 1;  /* 防止连续噪声重复发 ACK */
        }
    }
    /* 等 TX 完成后清除标志 */
    if (ack_pending && (U0_SR & SR_TXE)) {
        ack_pending = 0;
    }
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    BSP_GPIO_Init();
    LED_ERR_OFF;

    /* ---- USART0 ���Ĵ�����ʼ�� (����STM32 USART1) ---- */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_AF_PP,      GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    U0_BRR = 0x00000EA6;   /* 72MHz / 19200 = 3750 = 0xEA6 */
    U0_CR2 = 0;            /* Ĭ�� */
    U0_CR3 = 0;            /* Ĭ�� (��DMA,������) */
    U0_CR1 = CR1_TE | CR1_RE | CR1_RXNEIE | CR1_UE;  /* TX+RX+RXNE�ж�+ʹ�� */

    nvic_irq_enable(USART0_IRQn, 3, 0);

    while (1) {
        LED_RUN_ON;  delay(500);
        LED_RUN_OFF; delay(500);
    }
}
