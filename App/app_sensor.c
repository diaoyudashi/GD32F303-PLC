#include "app_sensor.h"
#include "bsp_adc.h"
#include "bsp_tim.h"
#include "bsp_gpio.h"
#include "drv_tpl0501.h"
#include "utils_filter.h"

static LPF_1st sensor1_filter, sensor2_filter;

/* Yarn pulse tracking — independent per channel */
static volatile uint32_t pulse1_tick = 0;
static volatile uint32_t pulse2_tick = 0;
static uint32_t motor_start_tick = 0;
#define YARN_TIMEOUT_MS   800
#define YARN_GRACE_MS     800   /* ignore breaks in first 800ms after start */

void APP_Sensor_Init(void)
{
    LPF_Init(&sensor1_filter, 0.05f);
    LPF_Init(&sensor2_filter, 0.05f);
    TPL0501_Init();
    TPL0501_SetChannel1(14);//P1  13
    TPL0501_SetChannel2(8);//P2   8

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
    AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    GPIO_InitTypeDef g = {0};
    g.Mode = GPIO_MODE_IT_FALLING;
    g.Pull = GPIO_PULLUP;
    g.Pin  = PHOTO2_OUT_PIN;
    HAL_GPIO_Init(PHOTO2_OUT_PORT, &g);
    g.Pin  = PHOTO1_OUT_PIN;
    HAL_GPIO_Init(PHOTO1_OUT_PORT, &g);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    pulse1_tick = pulse2_tick = HAL_GetTick();
}

static volatile uint32_t isr_p1_cnt = 0;
static volatile uint32_t isr_p2_cnt = 0;

void APP_Sensor_Pulse_ISR(uint16_t pin)
{
    uint32_t now = HAL_GetTick();
    if (pin == PHOTO1_OUT_PIN) {
        if (now - pulse1_tick > 1) { pulse1_tick = now; isr_p1_cnt++; }
    } else if (pin == PHOTO2_OUT_PIN) {
        if (now - pulse2_tick > 1) { pulse2_tick = now; isr_p2_cnt++; }
    }
}

uint32_t APP_Sensor_GetISRCnt1(void) { return isr_p1_cnt; }
uint32_t APP_Sensor_GetISRCnt2(void) { return isr_p2_cnt; }

void APP_Sensor_ResetPulse(void)
{
    pulse1_tick = pulse2_tick = motor_start_tick = HAL_GetTick();
}

uint8_t APP_Sensor_Broken(void)
{
    uint32_t now = HAL_GetTick();
    if (now - motor_start_tick < YARN_GRACE_MS) return 0;
    return ((now - pulse1_tick > YARN_TIMEOUT_MS) ||
            (now - pulse2_tick > YARN_TIMEOUT_MS)) ? 1 : 0;
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
