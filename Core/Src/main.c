#include "stm32f10x.h"

volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay_ms(uint32_t ms) { uint32_t s = tick; while ((tick-s) < ms); }

static uint8_t  rx_buf[256];
static volatile int16_t  rx_cnt = 0;
static volatile uint8_t  reply_flag = 0;
static const char Ascll[] = "0123456789ABCDEF";

static void tx_frame(uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    USART_SendData(USART1, 0x02);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    sum = sum + 0x02;
    for (uint16_t i = 0; i < len; i++) {
        USART_SendData(USART1, data[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        sum = sum + data[i];
    }
    USART_SendData(USART1, 0x03);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    sum = sum + 0x03;
    USART_SendData(USART1, Ascll[(sum >> 4) & 0x0F]);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, Ascll[sum & 0x0F]);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART0_IRQHandler(void)
{
    /* 仅处理 RXNE */
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        uint8_t byte_data = (uint8_t)(USART_ReceiveData(USART1) & 0x7F);

        if (byte_data == 0x05) {
            /* ENQ: 直接回单字节 ACK */
            USART_SendData(USART1, 0x06);
        } else if (byte_data == 0x02) {
            /* STX: 开始新帧 */
            rx_cnt = 1;
            rx_buf[0] = 0x02;
        } else if (rx_cnt > 0) {
            /* 帧内数据 */
            if (rx_cnt < 250) {
                rx_buf[rx_cnt] = byte_data;
                rx_cnt = rx_cnt + 1;
            }
            /* 收到 ETX 后, 再收 2 字节校验 → 帧完整 */
            if (rx_cnt >= 5) {
                if (rx_buf[rx_cnt - 3] == 0x03) {
                    /* 校验和计算 */
                    uint8_t cal_sum = 0;
                    int16_t k = 0;
                    while (rx_buf[k] != 0x03) {
                        cal_sum = cal_sum + rx_buf[k];
                        k = k + 1;
                    }
                    /* ASCII 校验和 -> 数值 */
                    uint8_t rx_sum = 0;
                    uint8_t ch = rx_buf[rx_cnt - 2];
                    if (ch >= '0' && ch <= '9') {
                        rx_sum = (uint8_t)((ch - '0') << 4);
                    } else if (ch >= 'A' && ch <= 'F') {
                        rx_sum = (uint8_t)((ch - 'A' + 10) << 4);
                    }
                    ch = rx_buf[rx_cnt - 1];
                    if (ch >= '0' && ch <= '9') {
                        rx_sum = rx_sum | (uint8_t)(ch - '0');
                    } else if (ch >= 'A' && ch <= 'F') {
                        rx_sum = rx_sum | (uint8_t)(ch - 'A' + 10);
                    }

                    if (cal_sum == rx_sum) {
                        /* 校验成功 */
                        GPIO_ResetBits(GPIOF, GPIO_Pin_9); /* ERR 灯亮 */
                        reply_flag = 1;
                    } else {
                        /* 校验失败 — 也回复, 做诊断 */
                        GPIO_SetBits(GPIOF, GPIO_Pin_9);   /* ERR 灯灭 */
                        reply_flag = 1;                    /* 无条件回复 */
                    }
                    rx_cnt = 0;
                }
            }
        }
    }

    /* 清溢出 */
    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET) {
        USART_ReceiveData(USART1);
    }
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);

    /* GPIOF9,10 = LED */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    GPIO_InitTypeDef gpio_cfg;
    
    gpio_cfg.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    gpio_cfg.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_cfg.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &gpio_cfg);
    GPIO_SetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);

    /* USART1: PA9=TX, PA10=RX */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    
    gpio_cfg.GPIO_Pin = GPIO_Pin_9;
    gpio_cfg.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_cfg.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio_cfg);

    
    gpio_cfg.GPIO_Pin = GPIO_Pin_10;
    gpio_cfg.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_cfg.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_cfg);

    USART_InitTypeDef usart_cfg;
    
    usart_cfg.USART_BaudRate = 19200;
    usart_cfg.USART_WordLength = USART_WordLength_8b;
    usart_cfg.USART_StopBits = USART_StopBits_1;
    usart_cfg.USART_Parity = USART_Parity_No;
    usart_cfg.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_cfg.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &usart_cfg);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

    NVIC_InitTypeDef nvic_cfg;
    
    nvic_cfg.NVIC_IRQChannel = 37;
    nvic_cfg.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_cfg.NVIC_IRQChannelSubPriority = 0;
    nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_cfg);

    uint32_t last_blink = 0;
    uint8_t  led = 1;
    while (1) {
        if (reply_flag) {
            reply_flag = 0;
            uint16_t plc_model = 0x5EF6;
            uint8_t reply_data[4];
            uint8_t lo = (uint8_t)(plc_model & 0xFF);
            uint8_t hi = (uint8_t)((plc_model >> 8) & 0xFF);
            reply_data[0] = Ascll[(lo >> 4) & 0x0F];
            reply_data[1] = Ascll[lo & 0x0F];
            reply_data[2] = Ascll[(hi >> 4) & 0x0F];
            reply_data[3] = Ascll[hi & 0x0F];
            tx_frame(reply_data, 4);
        }
        if ((tick - last_blink) >= 500) {
            last_blink = tick;
            if (led) { GPIO_ResetBits(GPIOF, GPIO_Pin_10); led = 0; }
            else     { GPIO_SetBits(GPIOF, GPIO_Pin_10);   led = 1; }
        }
    }
}
