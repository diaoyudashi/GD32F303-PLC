#ifndef __APP_SYSTEM_H
#define __APP_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    SYS_INIT = 0,
    SYS_IDLE,
    SYS_RUNNING,
    SYS_FAULT,
} SystemState;

typedef void (*SoftTimerCallback)(void);

void     APP_System_Init(void);
void     APP_System_TickInc(void);
void     APP_System_Run(void);

SystemState APP_System_GetState(void);
void     APP_System_SetState(SystemState state);
uint32_t APP_System_GetTick(void);

/* Soft timer API */
int8_t   APP_System_TimerCreate(uint32_t interval_ms, uint8_t repeat, SoftTimerCallback cb);
void     APP_System_TimerStart(int8_t id);
void     APP_System_TimerStop(int8_t id);
void     APP_System_TimerDelete(int8_t id);

#ifdef __cplusplus
}
#endif

#endif /* __APP_SYSTEM_H */
