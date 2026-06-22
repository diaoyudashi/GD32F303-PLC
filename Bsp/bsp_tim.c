#include "bsp_tim.h"
#include "bsp_gpio.h"
#include "app_config.h"

TIM_HandleTypeDef htim1 = {0};
TIM_HandleTypeDef htim2 = {0};
TIM_HandleTypeDef htim3 = {0};

/* ========================== TIM1: 6-step BLDC PWM ========================== */
void BSP_TIM1_PWM_Init(void)
{
    TIM_OC_InitTypeDef oc = {0};
    TIM_BreakDeadTimeConfigTypeDef bdt = {0};
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_TIM1_CLK_ENABLE();

    /* PA8=TIM1_CH1, PA9=TIM1_CH2, PA10=TIM1_CH3 (HIN)
     * PB13=TIM1_CH1N, PB14=TIM1_CH2N, PB15=TIM1_CH3N (LIN)
     */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* PB13/14/15 = GPIO push-pull (low-side control); init in BSP_GPIO_Init */

    /* TIM1 base: 72MHz, center-aligned, period = 72M/16k = 4500 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;                   /* 72MHz / (0+1) = 72MHz */
    htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
    htim1.Init.Period = SYSTEM_CLOCK_HZ / MOTOR_PWM_FREQ_HZ;  /* 4500 */
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&htim1);

    /* Complementary PWM CH1/CH1N, CH2/CH2N, CH3/CH3N */
    oc.OCMode = TIM_OCMODE_PWM1;
    oc.Pulse = 0;  /* all off initially */
    oc.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    oc.OCFastMode = TIM_OCFAST_ENABLE;
    oc.OCIdleState = TIM_OCIDLESTATE_RESET;
    oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    HAL_TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_3);

    /* Dead-time ~1us: DTG = 1us * 72MHz = 72 */
    bdt.OffStateRunMode = TIM_OSSR_ENABLE;
    bdt.OffStateIDLEMode = TIM_OSSI_ENABLE;
    bdt.LockLevel = TIM_LOCKLEVEL_OFF;
    bdt.DeadTime = 72;  /* 72 * 13.89ns ≈ 1μs */
    bdt.BreakState = TIM_BREAK_DISABLE;
    bdt.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    bdt.BreakFilter = 0;
    bdt.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&htim1, &bdt);

    /* TIM1 TRGO → trigger ADC */
    TIM_MasterConfigTypeDef master = {0};
    master.MasterOutputTrigger = TIM_TRGO_UPDATE;
    master.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim1, &master);

    /* Enable preload on all channels */
    __HAL_TIM_ENABLE_OCxPRELOAD(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_ENABLE_OCxPRELOAD(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_ENABLE_OCxPRELOAD(&htim1, TIM_CHANNEL_3);

    /* NVIC for TIM1 break */
    HAL_NVIC_SetPriority(TIM1_BRK_IRQn, NVIC_PRIO_TIM1_BRK, 0);
    HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, NVIC_PRIO_TIM1_UP, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
}

void BSP_TIM1_PWM_Start(void)
{
    /* HS only; complementary managed by SixStep() */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_MOE_ENABLE(&htim1);
}

void BSP_TIM1_PWM_StartFOC(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_MOE_ENABLE(&htim1);
}

void BSP_TIM1_PWM_Stop(void)
{
    __HAL_TIM_MOE_DISABLE(&htim1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
}

/* ========================== TIM2: PWM DAC for Photo ======================== */
void BSP_TIM2_PWMDAC_Init(void)
{
    TIM_OC_InitTypeDef oc = {0};
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_TIM2_CLK_ENABLE();

    /* PA4 = TIM2_CH3, PA5 = TIM2_CH4 → PWM → RC-filter → DAC */
    gpio.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* TIM2 on APB1 = 36MHz * 2 = 72MHz */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 255;        /* 8-bit PWM resolution */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&htim2);

    oc.OCMode = TIM_OCMODE_PWM1;
    oc.Pulse = 128;  /* mid-scale default */
    oc.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim2, &oc, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&htim2, &oc, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

void BSP_TIM2_SetDAC_CH3(uint16_t val) { __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, val); }
void BSP_TIM2_SetDAC_CH4(uint16_t val) { __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, val); }

/* ========================== TIM3: Encoder ================================== */
void BSP_TIM3_Encoder_Init(void)
{
    TIM_Encoder_InitTypeDef enc = {0};
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_TIM3_CLK_ENABLE();

    /* PC6 = TIM3_CH1, PC7 = TIM3_CH2 (encoder A/B) */
    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &gpio);

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0xFFFF;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    enc.EncoderMode = TIM_ENCODERMODE_TI12;
    enc.IC1Polarity = TIM_ICPOLARITY_RISING;
    enc.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    enc.IC1Prescaler = TIM_ICPSC_DIV1;
    enc.IC1Filter = 6;  /* digital filter for noise */
    enc.IC2Polarity = TIM_ICPOLARITY_RISING;
    enc.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    enc.IC2Prescaler = TIM_ICPSC_DIV1;
    enc.IC2Filter = 6;
    HAL_TIM_Encoder_Init(&htim3, &enc);

    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    HAL_NVIC_SetPriority(TIM3_IRQn, NVIC_PRIO_TIM3, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

int32_t BSP_TIM3_GetEncoderCount(void)
{
    return (int32_t)(int16_t)__HAL_TIM_GET_COUNTER(&htim3);
}

void BSP_TIM3_ResetEncoder(void)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
}

/* ========================== Hall Sensor EXTI =============================== */
void BSP_Hall_EXTI_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    /* PB1, PB4, PC11: Hall U/V/W, input with pull-up, interrupt on both edges */
    gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio.Pull = GPIO_PULLUP;

    gpio.Pin = HALL_U_PIN;
    HAL_GPIO_Init(HALL_U_PORT, &gpio);
    gpio.Pin = HALL_V_PIN;
    HAL_GPIO_Init(HALL_V_PORT, &gpio);
    gpio.Pin = HALL_W_PIN;
    HAL_GPIO_Init(HALL_W_PORT, &gpio);

    /* PB1 → EXTI1, PB4 → EXTI4, PC11 → EXTI15_10 */
    HAL_NVIC_SetPriority(EXTI1_IRQn, NVIC_PRIO_HALL_EXTI, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_SetPriority(EXTI4_IRQn, NVIC_PRIO_HALL_EXTI, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, NVIC_PRIO_HALL_EXTI, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
