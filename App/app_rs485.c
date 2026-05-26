#include "app_rs485.h"
#include "bsp_uart.h"

#define FRAME_BUF_SIZE 256
static uint8_t  frame_buf[FRAME_BUF_SIZE];
static uint16_t frame_len = 0;
static uint8_t  frame_ready = 0;
static uint32_t last_byte_time = 0;

void APP_RS485_Init(void)
{
    BSP_USART1_RS485_Init();
}

void APP_RS485_Run(void)
{
    /* Collect bytes from ISR ring buffer into frame buffer */
    while (BSP_USART1_RS485_Available() > 0 && frame_len < FRAME_BUF_SIZE) {
        uint8_t ch = BSP_USART1_RS485_Read();
        frame_buf[frame_len++] = ch;
        last_byte_time = HAL_GetTick();
    }

    /* Frame timeout: if no new byte for > RS485_FRAME_TIMEOUT_MS and len > 0 */
    if (frame_len > 0 && !frame_ready) {
        if (HAL_GetTick() - last_byte_time >= 5) {  /* 5ms timeout */
            frame_ready = 1;
        }
    }
}

void APP_RS485_Send(uint8_t *data, uint16_t len)
{
    BSP_USART1_RS485_Send(data, len);
}

uint8_t APP_RS485_FrameReady(void)
{
    return frame_ready;
}

void APP_RS485_GetFrame(uint8_t *buf, uint16_t *len)
{
    for (uint16_t i = 0; i < frame_len; i++) {
        buf[i] = frame_buf[i];
    }
    *len = frame_len;
    frame_len = 0;
    frame_ready = 0;
}
