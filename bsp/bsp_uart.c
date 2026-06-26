/**
 * @file    bsp_uart.c
 * @brief   3路UART — 中断接收 + 轮询发送 (暂不用DMA)
 */

#include "bsp_uart.h"

static volatile uint8_t u1rx[UART1_RX_BUF_SIZE], u2rx[UART2_RX_BUF_SIZE], u4rx[UART4_RX_BUF_SIZE];
static volatile uint16_t u1rh, u1rt, u2rh, u2rt, u4rh, u4rt;

void USART1_IRQHandler(void) {
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)) {
        uint8_t d = usart_data_receive(USART1);
        uint16_t n = (u1rh + 1) % UART1_RX_BUF_SIZE;
        if (n != u1rt) { u1rx[u1rh] = d; u1rh = n; }
    }
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE))
        usart_data_receive(USART1);
}
void USART2_IRQHandler(void) {
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE)) {
        uint8_t d = usart_data_receive(USART2);
        uint16_t n = (u2rh + 1) % UART2_RX_BUF_SIZE;
        if (n != u2rt) { u2rx[u2rh] = d; u2rh = n; }
    }
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE))
        usart_data_receive(USART2);
}
void UART4_IRQHandler(void) {
    if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE)) {
        uint8_t d = usart_data_receive(UART4);
        uint16_t n = (u4rh + 1) % UART4_RX_BUF_SIZE;
        if (n != u4rt) { u4rx[u4rh] = d; u4rh = n; }
    }
    if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_IDLE))
        usart_data_receive(UART4);
}

static void _uart_init(uint32_t uart, uint32_t gpio, uint32_t tx, uint32_t rx, uint32_t baud) {
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(gpio);
    gpio_init((gpio == RCU_GPIOA) ? GPIOA : (gpio == RCU_GPIOC) ? GPIOC : GPIOA,
              GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, tx);
    gpio_init((gpio == RCU_GPIOA) ? GPIOA : (gpio == RCU_GPIOC) ? GPIOC : GPIOA,
              GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, rx);
    usart_deinit(uart);
    usart_baudrate_set(uart, baud);
    usart_word_length_set(uart, USART_WL_8BIT);
    usart_stop_bit_set(uart, USART_STB_1BIT);
    usart_parity_config(uart, USART_PM_NONE);
    usart_hardware_flow_rts_config(uart, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(uart, USART_CTS_DISABLE);
    usart_receive_config(uart, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart, USART_TRANSMIT_ENABLE);
    usart_interrupt_enable(uart, USART_INT_RBNE);
    usart_interrupt_enable(uart, USART_INT_IDLE);
    usart_enable(uart);
}

void BSP_USART_Init(void) {
    rcu_periph_clock_enable(RCU_USART1);
    _uart_init(USART1, RCU_GPIOA, GPIO_PIN_9,  GPIO_PIN_10, 19200);
    nvic_irq_enable(USART1_IRQn, NVIC_PRIO_USART1, 0);

    rcu_periph_clock_enable(RCU_USART2);
    _uart_init(USART2, RCU_GPIOA, GPIO_PIN_2,  GPIO_PIN_3,  9600);
    nvic_irq_enable(USART2_IRQn, NVIC_PRIO_USART2, 0);

    rcu_periph_clock_enable(RCU_UART4);
    _uart_init(UART4,  RCU_GPIOC, GPIO_PIN_10, GPIO_PIN_11, 19200);
    nvic_irq_enable(UART4_IRQn, NVIC_PRIO_UART4, 0);
}

/* 轮询发送 (简单可靠) */
void BSP_USART_SendByte(ComID_t com, uint8_t data) {
    uint32_t uart;
    switch (com) {
    case COM_PLC:  uart = USART1; break;
    case COM_LCD:  uart = USART2; break;
    case COM_SEVO: uart = UART4;  break;
    default: return;
    }
    usart_data_transmit(uart, data);
    while (!usart_flag_get(uart, USART_FLAG_TBE));
}
void BSP_USART_SendBuf(ComID_t com, uint8_t *buf, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) BSP_USART_SendByte(com, buf[i]);
}

uint8_t BSP_USART_GetRxData(ComID_t com, uint8_t *buf, uint16_t maxlen) {
    volatile uint8_t *rx; volatile uint16_t *h, *t; uint16_t sz;
    switch (com) {
    case COM_PLC:  rx = u1rx; h = &u1rh; t = &u1rt; sz = UART1_RX_BUF_SIZE; break;
    case COM_LCD:  rx = u2rx; h = &u2rh; t = &u2rt; sz = UART2_RX_BUF_SIZE; break;
    case COM_SEVO: rx = u4rx; h = &u4rh; t = &u4rt; sz = UART4_RX_BUF_SIZE; break;
    default: return 0;
    }
    uint16_t cnt = 0;
    while (*t != *h && cnt < maxlen) { buf[cnt++] = rx[*t]; *t = (*t + 1) % sz; }
    return cnt;
}
