#ifndef __DRV_EG3112_H
#define __DRV_EG3112_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* EG3112 Gate Driver interface (3-phase BLDC, controlled via TIM1)
 *
 * EG3112 pin 2 = HIN (high-side), pin 3 = LIN (low-side)
 * Connected to TIM1 complementary PWM outputs.
 *
 * This driver encapsulates:
 *   - Enable/disable all phases (MOE control)
 *   - Brake / emergency stop
 *   - Phase-by-phase duty cycle setting
 */

void EG3112_Enable(void);
void EG3112_Disable(void);
void EG3112_Brake(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_EG3112_H */
