#ifndef __DRV_TPL0501_H
#define __DRV_TPL0501_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* TPL0501-100DCNR Digital Potentiometer Driver (dual-channel, shared bus)
 *
 * Pinout:
 *   PB3 = CS  (shared for both pots)
 *   PB5 = SCLK (shared)
 *   PC9 = DIN1 (channel 1)
 *   PC5 = DIN2 (channel 2)
 *
 * Resolution: 8 bits (0-255), 100kΩ end-to-end
 */

void TPL0501_Init(void);
void TPL0501_Set(uint8_t ch1_val, uint8_t ch2_val);
void TPL0501_SetChannel1(uint8_t val);
void TPL0501_SetChannel2(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_TPL0501_H */
