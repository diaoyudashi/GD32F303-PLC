/**
 * @file    bsp_uart.c
 * @brief   GD32F303 3路UART + DMA TX + 中断 RX
 */

#include "bsp_uart.h"

static volatile uint8_t u1rx[UART1_RX_BUF_SIZE], u2rx[UART2_RX_BUF_SIZE], u4rx[UART4_RX_BUF_SIZE];
static volatile uint16_t u1rh, u1rt, u2rh, u2rt, u4rh, u4rt;
static volatile uint8_t u1tc, u2tc;

#define RB_PUSH(rx, h, t, sz, d) do { \
    uint16_t n = ((h) + 1) % (sz); \
    if (n != (t)) { rx[h] = d; h = n; } \
} while(0)

void USART1_IRQHandler(void) {
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
        RB_PUSH(u1rx, u1rh, u1rt, UART1_RX_BUF_SIZE, usart_data_receive(USART1));
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE))
        usart_data_receive(USART1);
}
void USART2_IRQHandler(void) {
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE))
        RB_PUSH(u2rx, u2rh, u2rt, UART2_RX_BUF_SIZE, usart_data_receive(USART2));
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE))
        usart_data_receive(USART2);
}
void UART4_IRQHandler(void) {
    if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE))
        RB_PUSH(u4rx, u4rh, u4rt, UART4_RX_BUF_SIZE, usart_data_receive(UART4));
    if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_IDLE))
        usart_data_receive(UART4);
}
void DMA0_Channel3_IRQHandler(void) {
    if (dma_interrupt_flag_get(DMA0, DMA_CH3, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INT_FLAG_G); u1tc = 1;
    }
}
void DMA0_Channel6_IRQHandler(void) {
    if (dma_interrupt_flag_get(DMA0, DMA_CH6, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INT_FLAG_G); u2tc = 1;
    }
}

static void _uart_cfg(uint32_t uart, uint32_t baud) {
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

static void _dma_tx_cfg(uint32_t uart, dma_channel_enum ch) {
    dma_parameter_struct d = {0};
    dma_deinit(DMA0, ch);
    dma_struct_para_init(&d);
    d.periph_addr = (uint32_t)&USART_DATA(uart);
    d.direction = DMA_MEMORY_TO_PERIPHERAL;
    d.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    d.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    d.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    d.memory_width = DMA_MEMORY_WIDTH_8BIT;
    d.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, ch, &d);
    dma_interrupt_enable(DMA0, ch, DMA_CHXCTL_FTFIE);
}

void BSP_USART_Init(void)
{
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_DMA0);

    /* ====== USART1: PLC下载口 PA9(TX) PA10(RX) 19200 DMA0_CH3 ====== */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART1);
    gpio_init(GPIOA, GPIO_MODE_AF_PP,      GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    _uart_cfg(USART1, 19200);
    usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);
    _dma_tx_cfg(USART1, DMA_CH3);
    nvic_irq_enable(DMA0_Channel3_IRQn, NVIC_PRIO_DMA, 0);
    nvic_irq_enable(USART1_IRQn, NVIC_PRIO_USART1, 0);

    /* ====== USART2: LCD Modbus PA2(TX) PA3(RX) 9600 DMA0_CH6 ====== */
    rcu_periph_clock_enable(RCU_USART2);
    gpio_init(GPIOA, GPIO_MODE_AF_PP,      GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    _uart_cfg(USART2, 9600);
    usart_dma_transmit_config(USART2, USART_TRANSMIT_DMA_ENABLE);
    _dma_tx_cfg(USART2, DMA_CH6);
    nvic_irq_enable(DMA0_Channel6_IRQn, NVIC_PRIO_DMA, 0);
    nvic_irq_enable(USART2_IRQn, NVIC_PRIO_USART2, 0);

    /* ====== UART4: 伺服 Modbus 485 PC10(TX) PC11(RX) 19200 ====== */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_UART4);
    gpio_init(GPIOC, GPIO_MODE_AF_PP,      GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    _uart_cfg(UART4, 19200);
    nvic_irq_enable(UART4_IRQn, NVIC_PRIO_UART4, 0);
}

void BSP_USART_SendBuf(ComID_t com, uint8_t *buf, uint16_t len) {
    if (!len) return;
    switch (com) {
    case COM_PLC:
        u1tc = 0;
        dma_channel_disable(DMA0, DMA_CH3);
        dma_transfer_number_config(DMA0, DMA_CH3, len);
        dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)buf);
        dma_channel_enable(DMA0, DMA_CH3);
        while (!u1tc);
        break;
    case COM_LCD:
        u2tc = 0;
        dma_channel_disable(DMA0, DMA_CH6);
        dma_transfer_number_config(DMA0, DMA_CH6, len);
        dma_memory_address_config(DMA0, DMA_CH6, (uint32_t)buf);
        dma_channel_enable(DMA0, DMA_CH6);
        while (!u2tc);
        break;
    case COM_SEVO:
        for (uint16_t i = 0; i < len; i++) {
            usart_data_transmit(UART4, buf[i]);
            while (!usart_flag_get(UART4, USART_FLAG_TBE));
        }
        break;
    }
}
void BSP_USART_SendByte(ComID_t com, uint8_t data) { BSP_USART_SendBuf(com, &data, 1); }

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
