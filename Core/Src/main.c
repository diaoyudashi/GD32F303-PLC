#include "stm32f10x.h"
#include "gxworks.h"

volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay_ms(uint32_t ms) { uint32_t s = tick; while ((tick - s) < ms); }

/* FX2N-style ENQ direct ACK */
void USART0_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t d = USART_ReceiveData(USART1);
        if (d == 0x05) {
            USART_SendData(USART1, 0x06);
        } else {
            GXWorks_FeedByte(d);
        }
    }
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    
    /* USART1: PA9/PA10 19200 8N1 */
    /* LED pins: PF9(ERR), PF10(RUN) */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    GPIO_InitTypeDef lg;
    lg.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    lg.GPIO_Speed = GPIO_Speed_50MHz;
    lg.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOF, &lg);
    GPIO_SetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_10); /* LED off */

    /* USART1: PA9/PA10 19200 8N1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef g;
    g.GPIO_Pin = GPIO_Pin_9; g.GPIO_Speed = GPIO_Speed_50MHz; g.GPIO_Mode = GPIO_Mode_AF_PP; GPIO_Init(GPIOA, &g);
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

    GPIO_SetBits(GPIOF, GPIO_Pin_9);

    while (1) {
        GXWorks_SendTx();
        GPIO_ResetBits(GPIOF, GPIO_Pin_10);  delay_ms(200);
        GPIO_SetBits(GPIOF, GPIO_Pin_10); delay_ms(200);
    }
}
