#ifndef __APP_SENSOR_H
#define __APP_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Two identical photo-electric sensors, each with 3-stage op-amp conditioning.
 * Sensor 1: PB0=stage1_monitor, PA4=stage3_threshold, PA15=stage3_output
 * Sensor 2: PA0=stage1_monitor, PA5=stage3_threshold, PB9=stage3_output
 */

void APP_Sensor_Init(void);
void APP_Sensor_Run(void);

uint16_t APP_Sensor1_GetAnalog(void);
uint16_t APP_Sensor2_GetAnalog(void);
uint8_t  APP_Sensor1_GetOutput(void);
uint8_t  APP_Sensor2_GetOutput(void);

/* Yarn-break detection */
uint8_t APP_Sensor_Broken(void);       /* 1=either channel broken */
uint8_t APP_Sensor_WhichBroken(void);  /* 1=P1(upper), 2=P2(lower), 0=none */
void APP_Sensor_ResetPulse(void);

/* Called from EXTI ISR */
void APP_Sensor_Pulse_ISR(uint16_t pin);
uint32_t APP_Sensor_GetISRCnt1(void);
uint32_t APP_Sensor_GetISRCnt2(void);

void APP_Sensor1_SetThreshold(uint16_t val);
void APP_Sensor2_SetThreshold(uint16_t val);
void APP_Sensor1_SetGain(uint8_t val);
void APP_Sensor2_SetGain(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* __APP_SENSOR_H */
