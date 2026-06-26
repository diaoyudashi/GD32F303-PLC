/**
 * @file    bsp_uart.c
 * @brief   GD32F303 串口初始化 + 发送/接收
 *
 *  USART1 PA9(TX) PA10(RX)  19200 8N1  ← PLC下载口 GX Works
 *  USART2 PA2(TX) PA3(RX)   19200 8N1  ← Modbus 232 LCD
 *  UART4  PC10(TX) PC11(RX) 19200 8N1  ← Modbus 485 伺服
 */

#include "bsp_uart.h"

/* ---- 环形缓冲 ---- */
static volatile uint8_t uart1_rx[UART1_RX_BUF_SIZE];
static volatile uint8_t uart2_rx[UART2_RX_BUF_SIZE];
static volatile uint8_t uart4_rx[UART4_RX_BUF_SIZE];
static volatile uint16_t uart1_rx_head = 0, uart1_rx_tail = 0;
static volatile uint16_t uart2_rx_head = 0, uart2_rx_tail = 0;
static volatile uint16_t uart4_rx_head = 0, uart4_rx_tail = 0;

/* ---- 中断服务 ---- */
void USART1_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE) != RESET) {
        uint8_t d = usart_data_receive(USART1);
        uint16_t next = (uart1_rx_head + 1) % UART1_RX_BUF_SIZE;
        if (next != uart1_rx_tail) {
            uart1_rx[uart1_rx_head] = d;
            uart1_rx_head = next;
        }
    }
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE) != RESET) {
        usart_data_receive(USART1); /* 清 IDLE 标志 */
    }
}

void USART2_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) != RESET) {
        uint8_t d = usart_data_receive(USART2);
        uint16_t next = (uart2_rx_head + 1) % UART2_RX_BUF_SIZE;
        if (next != uart2_rx_tail) {
            uart2_rx[uart2_rx_head] = d;
            uart2_rx_head = next;
        }
    }
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE) != RESET) {
        usart_data_receive(USART2);
    }
}

void UART4_IRQHandler(void)
{
    if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE) != RESET) {
        uint8_t d = usart_data_receive(UART4);
        uint16_t next = (uart4_rx_head + 1) % UART4_RX_BUF_SIZE;
        if (next != uart4_rx_tail) {
            uart4_rx[uart4_rx_head] = d;
            uart4_rx_head = next;
        }
    }
    if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_IDLE) != RESET) {
        usart_data_receive(UART4);
    }
}

/* ---- 初始化 ---- */
void BSP_USART_Init(void)
{
    /* ==================== USART1  PLC下载口 PA9 TX / PA10 RX ==================== */
    rcu_periph_clock_enable(RCU_USART1);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);   /* TX */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10); /* RX */

    usart_deinit(USART1);
    usart_baudrate_set(USART1, 19200);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_interrupt_enable(USART1, USART_INT_RBNE);
    usart_interrupt_enable(USART1, USART_INT_IDLE);
    usart_enable(USART1);

    nvic_irq_enable(USART1_IRQn, NVIC_PRIO_USART1, 0);

    /* ==================== USART2  Modbus LCD PA2 TX / PA3 RX ==================== */
    rcu_periph_clock_enable(RCU_USART2);
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);   /* TX */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3); /* RX */

    usart_deinit(USART2);
    usart_baudrate_set(USART2, 19200);
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    usart_interrupt_enable(USART2, USART_INT_RBNE);
    usart_interrupt_enable(USART2, USART_INT_IDLE);
    usart_enable(USART2);

    nvic_irq_enable(USART2_IRQn, NVIC_PRIO_USART2, 0);

    /* ==================== UART4  Modbus 485 伺服 PC10 TX / PC11 RX ==================== */
    rcu_periph_clock_enable(RCU_UART4);
    rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  /* TX */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11); /* RX */

    usart_deinit(UART4);
    usart_baudrate_set(UART4, 19200);
    usart_word_length_set(UART4, USART_WL_8BIT);
    usart_stop_bit_set(UART4, USART_STB_1BIT);
    usart_parity_config(UART4, USART_PM_NONE);
    usart_hardware_flow_rts_config(UART4, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART4, USART_CTS_DISABLE);
    usart_receive_config(UART4, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART4, USART_TRANSMIT_ENABLE);
    usart_interrupt_enable(UART4, USART_INT_RBNE);
    usart_interrupt_enable(UART4, USART_INT_IDLE);
    usart_enable(UART4);

    nvic_irq_enable(UART4_IRQn, NVIC_PRIO_UART4, 0);
}

/* ---- 发送单字节 ---- */
void BSP_USART_SendByte(ComID_t com, uint8_t data)
{
    switch (com) {
    case COM_PLC:  usart_data_transmit(USART1, data); while (usart_flag_get(USART1, USART_FLAG_TBE) == RESET); break;
    case COM_LCD:  usart_data_transmit(USART2, data); while (usart_flag_get(USART2, USART_FLAG_TBE) == RESET); break;
    case COM_SEVO: usart_data_transmit(UART4,  data); while (usart_flag_get(UART4,  USART_FLAG_TBE) == RESET); break;
    }
}

/* ---- 发送缓冲 ---- */
void BSP_USART_SendBuf(ComID_t com, uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        BSP_USART_SendByte(com, buf[i]);
    }
}

/* ---- 读取接收数据 ---- */
uint8_t BSP_USART_GetRxData(ComID_t com, uint8_t *buf, uint16_t maxlen)
{
    volatile uint8_t *rxbuf;
    volatile uint16_t *head, *tail;
    uint16_t size;

    switch (com) {
    case COM_PLC:  rxbuf = uart1_rx; head = &uart1_rx_head; tail = &uart1_rx_tail; size = UART1_RX_BUF_SIZE; break;
    case COM_LCD:  rxbuf = uart2_rx; head = &uart2_rx_head; tail = &uart2_rx_tail; size = UART2_RX_BUF_SIZE; break;
    case COM_SEVO: rxbuf = uart4_rx; head = &uart4_rx_head; tail = &uart4_rx_tail; size = UART4_RX_BUF_SIZE; break;
    default: return 0;
    }

    uint16_t cnt = 0;
    while (*tail != *head && cnt < maxlen) {
        buf[cnt++] = rxbuf[*tail];
        *tail = (*tail + 1) % size;
    }
    return cnt;
}
