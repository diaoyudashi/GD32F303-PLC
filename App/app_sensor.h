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

/* Set comparator threshold via PWM DAC (PA4/PA5) */
void APP_Sensor1_SetThreshold(uint16_t val);
void APP_Sensor2_SetThreshold(uint16_t val);

/* Set TPL0501 digital pot gain (0-255) */
void APP_Sensor1_SetGain(uint8_t val);
void APP_Sensor2_SetGain(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* __APP_SENSOR_H */
