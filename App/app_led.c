#include "app_led.h"
#include "bsp_gpio.h"
#include "app_system.h"

static LED_Pattern led_pattern[2] = {LED_OFF, LED_OFF};
static uint32_t led_toggle_time[2] = {0};

void APP_LED_Init(void)
{
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
}

void APP_LED_Set(uint8_t led_id, LED_Pattern pattern)
{
    if (led_id < 2) {
        led_pattern[led_id] = pattern;
    }
}

void APP_LED_Run(void)
{
    uint32_t now = APP_System_GetTick();
    uint32_t interval;

    for (uint8_t i = 0; i < 2; i++) {
        switch (led_pattern[i]) {
        case LED_ON:
            HAL_GPIO_WritePin((i == 0) ? LED1_PORT : LED2_PORT,
                              (i == 0) ? LED1_PIN : LED2_PIN, GPIO_PIN_SET);
            break;
        case LED_OFF:
            HAL_GPIO_WritePin((i == 0) ? LED1_PORT : LED2_PORT,
                              (i == 0) ? LED1_PIN : LED2_PIN, GPIO_PIN_RESET);
            break;
        case LED_SLOW_BLINK: interval = 500; break;  /* 1Hz */
        case LED_FAST_BLINK: interval = 100; break;  /* 5Hz */
        default: continue;
        }

        if (led_pattern[i] == LED_SLOW_BLINK || led_pattern[i] == LED_FAST_BLINK) {
            if (now - led_toggle_time[i] >= interval) {
                led_toggle_time[i] = now;
                HAL_GPIO_TogglePin((i == 0) ? LED1_PORT : LED2_PORT,
                                   (i == 0) ? LED1_PIN : LED2_PIN);
            }
        }
    }
}
