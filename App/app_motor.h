#ifndef __APP_MOTOR_H
#define __APP_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    MOTOR_STOP = 0,
    MOTOR_ALIGN,
    MOTOR_START,
    MOTOR_RUN,
    MOTOR_FAULT,
} MotorState;

void APP_Motor_Init(void);
void APP_Motor_Run(void);          /* main loop call, handles speed control */
void APP_Motor_Start(void);
void APP_Motor_Stop(void);
void APP_Motor_EmergencyStop(void);

/* Photo-sensor-driven speed control */
void APP_Motor_SetDuty(uint8_t duty);
uint8_t APP_Motor_GetDuty(void);
void APP_Motor_UpdateFromSensors(void);  /* reads both sensors, maps to speed */

MotorState APP_Motor_GetState(void);
uint16_t APP_Motor_GetSpeed(void); /* RPM */

/* called from ISR context */
void APP_Motor_Hall_ISR(uint8_t hall_state);
void APP_Motor_ADC_ISR(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_MOTOR_H */
