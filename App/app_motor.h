#ifndef __APP_MOTOR_H
#define __APP_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    MOTOR_STOP = 0,
    MOTOR_ALIGN,
    MOTOR_RUN,
    MOTOR_FAULT,
} MotorState;

/* Init / main-loop */
void APP_Motor_Init(void);
void APP_Motor_Run(void);

/* Start / Stop */
void APP_Motor_Start(void);
void APP_Motor_Stop(void);
void APP_Motor_EmergencyStop(void);

/* Speed control — duty 0..255 (0 = stop, 255 = full) */
void     APP_Motor_SetSpeed(uint8_t duty);
uint8_t  APP_Motor_GetSpeed(void);

/* Status */
MotorState APP_Motor_GetState(void);
uint8_t    APP_Motor_GetHall(void);
uint16_t   APP_Motor_GetCurrent(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_MOTOR_H */
