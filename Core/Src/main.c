#include "stm32f10x.h"
#include "gxworks.h"

volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay_ms(uint32_t ms) { uint32_t s = tick, limit = ms*10; while ((tick-s) < ms && --limit); }

void USART0_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t d = USART_ReceiveData(USART1);
        if (d == 0x05) {
            USART_SendData(USART1, 0x06);
        } else if (d == 0x02) {
            GXWorks_FeedByte(d);
        } else if (d != 0x00) {
            /* 帧内数据字节 */
            GXWorks_FeedByte(d);
        }
    }
    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET) {
        USART_ReceiveData(USART1); /* 清溢出 */
    }
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);

    /* LED */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    GPIO_InitTypeDef g;
    g.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    g.GPIO_Speed = GPIO_Speed_50MHz; g.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &g);
    GPIO_SetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10);

    /* USART1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    g.GPIO_Pin = GPIO_Pin_9; g.GPIO_Mode = GPIO_Mode_AF_PP; GPIO_Init(GPIOA, &g);
    g.GPIO_Pin = GPIO_Pin_10; g.GPIO_Mode = GPIO_Mode_IN_FLOATING; GPIO_Init(GPIOA, &g);

    USART_InitTypeDef u;
    u.USART_BaudRate = 19200;
    u.USART_WordLength = USART_WordLength_8b;
    u.USART_StopBits = USART_StopBits_1;
    u.USART_Parity = USART_Parity_No;
    u.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    u.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &u);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

    NVIC_InitTypeDef n;
    n.NVIC_IRQChannel = USART1_IRQn;
    n.NVIC_IRQChannelPreemptionPriority = 3;
    n.NVIC_IRQChannelSubPriority = 0;
    n.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&n);

    while (1) {
        GXWorks_SendTx();
        GPIO_ResetBits(GPIOF, GPIO_Pin_10); delay_ms(500);
        GPIO_SetBits(GPIOF, GPIO_Pin_10);   delay_ms(500);
    }
}
