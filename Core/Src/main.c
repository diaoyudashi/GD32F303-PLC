#include "stm32f10x.h"
#include "gxworks.h"

volatile uint32_t tick = 0;
void SysTick_Handler(void) { tick++; }
void delay_ms(uint32_t ms) { uint32_t s = tick, limit = ms*10; while ((tick-s) < ms && --limit); }

static uint8_t  tx_buf[256];
static uint16_t tx_len = 0;
static uint8_t  tx_busy = 0;

/* DMA TX complete */
void DMA0_Channel3_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4)) {
        DMA_ClearITPendingBit(DMA1_IT_TC4);
        tx_busy = 0;
    }
}

/* ISR: 收到ENQ -> DMA发ACK; 其他给帧处理器 */
void USART0_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
        uint8_t d = USART_ReceiveData(USART1);
        if (d == 0x05 && !tx_busy) {
            /* ENQ -> DMA发送ACK */
            tx_buf[0] = 0x06;
            DMA_Cmd(DMA1_Channel4, DISABLE);
            DMA_SetCurrDataCounter(DMA1_Channel4, 1);
            DMA1_Channel4->CMAR = (uint32_t)tx_buf;
            DMA_Cmd(DMA1_Channel4, ENABLE);
            tx_busy = 1;
        } else if (d != 0x05) {
            GXWorks_FeedByte(d);
        }
    }
    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) == SET)
        USART_ReceiveData(USART1);
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

    /* USART1 + DMA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
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
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);

    /* DMA1 Channel4 = USART1 TX */
    DMA_InitTypeDef dma;
    DMA_DeInit(DMA1_Channel4);
    dma.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    dma.DMA_MemoryBaseAddr = (uint32_t)tx_buf;
    dma.DMA_DIR = DMA_DIR_PeripheralDST;
    dma.DMA_BufferSize = 0;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Normal;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &dma);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

    NVIC_InitTypeDef n;
    n.NVIC_IRQChannel = 30;
    n.NVIC_IRQChannelPreemptionPriority = 3;
    n.NVIC_IRQChannelSubPriority = 0;
    n.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&n);
    n.NVIC_IRQChannel = 37;
    NVIC_Init(&n);

    while (1) {
        GXWorks_SendTx();
        GPIO_ResetBits(GPIOF, GPIO_Pin_10); delay_ms(500);
        GPIO_SetBits(GPIOF, GPIO_Pin_10);   delay_ms(500);
    }
}
