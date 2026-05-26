#ifndef __BSP_UART_H
#define __BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;  /* USART1 remap → RS-485 (PB6/PB7) */
extern UART_HandleTypeDef huart2;  /* USART2 → debug (PA2/PA3) */

void BSP_USART1_RS485_Init(void);
void BSP_USART1_RS485_Send(uint8_t *data, uint16_t len);
uint16_t BSP_USART1_RS485_Available(void);
uint8_t  BSP_USART1_RS485_Read(void);

void BSP_USART2_Debug_Init(void);
void BSP_USART2_Send(uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H */
