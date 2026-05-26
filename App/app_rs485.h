#ifndef __APP_RS485_H
#define __APP_RS485_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void APP_RS485_Init(void);
void APP_RS485_Run(void);
void APP_RS485_Send(uint8_t *data, uint16_t len);
uint8_t APP_RS485_FrameReady(void);
void APP_RS485_GetFrame(uint8_t *buf, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif /* __APP_RS485_H */
