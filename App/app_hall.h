#ifndef __APP_HALL_H
#define __APP_HALL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* External Hall sensors
 * PB1  = linear analog (ADC_IN9) → motor trigger (20%~80% range)
 * PB4  = spare digital
 * PC11 = bipolar latch → revolution counter (edge-detect)
 */

void     APP_Hall_Init(void);
void     APP_Hall_Update(void);        /* call in main loop */
uint16_t APP_Hall_GetPB1_ADC(void);    /* PB1 analog 0-4095 */
uint8_t  APP_Hall_IsTriggered(void);   /* PB1 ADC in trigger range */
uint32_t APP_Hall_GetRevs(void);       /* PC11 accumulated count */
void     APP_Hall_ResetRevs(void);
uint8_t  APP_Hall_GetPB4(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_HALL_H */
