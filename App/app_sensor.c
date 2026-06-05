#include "app_sensor.h"
#include "bsp_adc.h"
#include "bsp_tim.h"
#include "bsp_gpio.h"
#include "drv_tpl0501.h"
#include "utils_filter.h"

static LPF_1st sensor1_filter, sensor2_filter;

/* Yarn pulse tracking */
static volatile uint32_t last_pulse_tick = 0;   /* ms timestamp of last falling edge */
#define YARN_TIMEOUT_MS  250

void APP_Sensor_Init(void)
{
    LPF_Init(&sensor1_filter, 0.05f);
    LPF_Init(&sensor2_filter, 0.05f);
    TPL0501_Init();
    TPL0501_Set(5, 5);

    /* PB9 + PA15: EXTI falling edge for yarn pulse detection */
    __HAL_RCC_AFIO_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Mode = GPIO_MODE_IT_FALLING;
    g.Pull = GPIO_PULLUP;
    g.Pin  = PHOTO2_OUT_PIN;   /* PB9 */
    HAL_GPIO_Init(PHOTO2_OUT_PORT, &g);
    g.Pin  = PHOTO1_OUT_PIN;   /* PA15 */
    HAL_GPIO_Init(PHOTO1_OUT_PORT, &g);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    last_pulse_tick = HAL_GetTick();  /* avoid false broken at startup */
}

void APP_Sensor_Pulse_ISR(uint16_t pin)
{
    if (pin == PHOTO1_OUT_PIN || pin == PHOTO2_OUT_PIN) {
        last_pulse_tick = HAL_GetTick();
    }
}

uint8_t APP_Sensor_Broken(void)
{
    return (HAL_GetTick() - last_pulse_tick > YARN_TIMEOUT_MS) ? 1 : 0;
}

void APP_Sensor_Run(void)
{
    /* Read ADC values and update filters */
    LPF_UpdateInt(&sensor1_filter, BSP_ADC_GetChannel(ADC_CH_PHOTO1));
    LPF_UpdateInt(&sensor2_filter, BSP_ADC_GetChannel(ADC_CH_PHOTO2));
}

uint16_t APP_Sensor1_GetAnalog(void)
{
    return (uint16_t)sensor1_filter.output;
}

uint16_t APP_Sensor2_GetAnalog(void)
{
    return (uint16_t)sensor2_filter.output;
}

uint8_t APP_Sensor1_GetOutput(void)
{
    return (HAL_GPIO_ReadPin(PHOTO1_OUT_PORT, PHOTO1_OUT_PIN) == GPIO_PIN_SET) ? 1 : 0;
}

uint8_t APP_Sensor2_GetOutput(void)
{
    return (HAL_GPIO_ReadPin(PHOTO2_OUT_PORT, PHOTO2_OUT_PIN) == GPIO_PIN_SET) ? 1 : 0;
}

void APP_Sensor1_SetThreshold(uint16_t val)
{
    BSP_TIM2_SetDAC_CH3(val);
}

void APP_Sensor2_SetThreshold(uint16_t val)
{
    BSP_TIM2_SetDAC_CH4(val);
}

void APP_Sensor1_SetGain(uint8_t val)
{
    TPL0501_Set(val, val);
}

void APP_Sensor2_SetGain(uint8_t val)
{
    TPL0501_Set(val, val);
}
