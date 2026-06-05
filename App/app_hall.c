#include "app_hall.h"
#include "bsp_gpio.h"
#include "bsp_adc.h"

static volatile uint32_t rev_count = 0;
static uint8_t  pc11_prev = 1;
static uint32_t pc11_last_tick = 0;

/* PB1 ADC trigger: magnet close ~2550, magnet far ~3000.
 * ON when ADC < 2800, OFF when ADC > 2850 (hysteresis) */
#define TRIG_ON_THR   2800
#define TRIG_OFF_THR  2850

static uint8_t trig_state = 0;

void APP_Hall_Init(void)
{
    pc11_prev = HAL_GPIO_ReadPin(HALL_W_PORT, HALL_W_PIN) ? 1 : 0;
}

void APP_Hall_Update(void)
{
    /* PC11: bipolar latch → edge-detect for rev counting */
    uint8_t pc11 = HAL_GPIO_ReadPin(HALL_W_PORT, HALL_W_PIN) ? 1 : 0;
    if (pc11_prev != pc11) {
        uint32_t now = HAL_GetTick();
        if (now - pc11_last_tick > 5) {
            rev_count++;
            pc11_last_tick = now;
        }
    }
    pc11_prev = pc11;

    /* PB1: hysteresis trigger */
    uint32_t adc = BSP_ADC_GetChannel(ADC_CH_HALL_TRIG);
    if (adc < TRIG_ON_THR)       trig_state = 1;
    else if (adc > TRIG_OFF_THR) trig_state = 0;
}

uint16_t APP_Hall_GetPB1_ADC(void)
{
    return (uint16_t)BSP_ADC_GetChannel(ADC_CH_HALL_TRIG);
}

uint8_t APP_Hall_IsTriggered(void)
{
    return trig_state;
}

uint32_t APP_Hall_GetRevs(void)   { return rev_count; }
void     APP_Hall_ResetRevs(void) { rev_count = 0; }

uint8_t APP_Hall_GetPB4(void)
{
    return (HAL_GPIO_ReadPin(HALL_V_PORT, HALL_V_PIN) == GPIO_PIN_RESET) ? 1 : 0;
}
