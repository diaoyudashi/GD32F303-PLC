#include "app_sensor.h"
#include "bsp_adc.h"
#include "bsp_tim.h"
#include "bsp_gpio.h"
#include "drv_tpl0501.h"
#include "utils_filter.h"

static LPF_1st sensor1_filter, sensor2_filter;
static uint8_t sensor1_gain = 128;
static uint8_t sensor2_gain = 128;

void APP_Sensor_Init(void)
{
    LPF_Init(&sensor1_filter, 0.05f);
    LPF_Init(&sensor2_filter, 0.05f);
    TPL0501_Init();
    TPL0501_Set(sensor1_gain, sensor2_gain);
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
    sensor1_gain = val;
    TPL0501_Set(sensor1_gain, sensor2_gain);
}

void APP_Sensor2_SetGain(uint8_t val)
{
    sensor2_gain = val;
    TPL0501_Set(sensor1_gain, sensor2_gain);
}
