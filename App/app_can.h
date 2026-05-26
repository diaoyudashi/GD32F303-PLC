#ifndef __APP_CAN_H
#define __APP_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void APP_CAN_Init(void);
void APP_CAN_Run(void);
void APP_CAN_Send(uint32_t id, uint8_t *data, uint8_t len);
uint8_t APP_CAN_FrameReady(void);
void APP_CAN_GetFrame(uint32_t *id, uint8_t *data, uint8_t *len);

#ifdef __cplusplus
}
#endif

#endif /* __APP_CAN_H */
