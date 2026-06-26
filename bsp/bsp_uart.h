/**
 * @file    bsp_uart.h
 * @brief   GD32F303 串口初始化 — 3路UART + DMA + 中断
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "main.h"

/* ---- 串口号 ---- */
typedef enum {
    COM_PLC  = 0,   /* USART1 PA9/PA10   PLC下载口 GX Works */
    COM_LCD  = 1,   /* USART2 PA2/PA3    Modbus 232 昆仑通泰LCD */
    COM_SEVO = 3,   /* UART4  PC10/PC11  Modbus 485 伺服驱动器 */
} ComID_t;

/* ---- 环形缓冲大小 ---- */
#define UART1_RX_BUF_SIZE   512
#define UART1_TX_BUF_SIZE   512
#define UART2_RX_BUF_SIZE   512
#define UART2_TX_BUF_SIZE   512
#define UART4_RX_BUF_SIZE   512
#define UART4_TX_BUF_SIZE   512

/* ---- 函数声明 ---- */
void BSP_USART_Init(void);
void BSP_USART_SendByte(ComID_t com, uint8_t data);
void BSP_USART_SendBuf(ComID_t com, uint8_t *buf, uint16_t len);
uint8_t BSP_USART_GetRxData(ComID_t com, uint8_t *buf, uint16_t maxlen);

#endif
