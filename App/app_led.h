#ifndef __APP_LED_H
#define __APP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    LED_OFF = 0,
    LED_ON,
    LED_SLOW_BLINK,  /* 1Hz */
    LED_FAST_BLINK,  /* 5Hz */
} LED_Pattern;

void APP_LED_Init(void);
void APP_LED_Set(uint8_t led_id, LED_Pattern pattern);
void APP_LED_Run(void);  /* called from main loop, handles blink timing */

#ifdef __cplusplus
}
#endif

#endif /* __APP_LED_H */
