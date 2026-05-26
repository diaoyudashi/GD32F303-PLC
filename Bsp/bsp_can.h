#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

extern CAN_HandleTypeDef hcan1;

void BSP_CAN1_Init(void);
void BSP_CAN1_Send(uint32_t id, uint8_t *data, uint8_t len);
uint8_t BSP_CAN1_Receive(uint32_t *id, uint8_t *data, uint8_t *len);
uint8_t BSP_CAN1_MessagePending(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CAN_H */
