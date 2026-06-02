#include "app_motor.h"
#include "bsp_tim.h"
#include "bsp_adc.h"
#include "bsp_gpio.h"
#include "drv_eg3112.h"
#include "app_system.h"

/* ======================== Internals ======================== */

static MotorState motor_state = MOTOR_STOP;
static uint8_t    motor_speed = 0;     /* user-set duty 0..255 */
static uint8_t    last_speed  = 50;    /* restore on re-start */
static uint32_t   align_start = 0;

/* ---- Hall sensor (5-pin motor connector: PC6/7/8) ---- */
static uint8_t read_hall(void)
{
    uint8_t h = 0;
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)) h |= 0x02;  /* bit2 (U) */
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)) h |= 0x01;  /* bit0 (V) */
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)) h |= 0x04;  /* bit1 (W) */
    return h;
}

/* ---- 6-step commutation via GPIO low-side ---- */
static void six_step(uint8_t step, uint8_t duty)
{
    TIM_TypeDef *t = htim1.Instance;
    uint16_t d = (uint16_t)duty * t->ARR / 255;

    HAL_GPIO_WritePin(LS_U_PORT, LS_U_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LS_V_PORT, LS_V_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LS_W_PORT, LS_W_PIN, GPIO_PIN_RESET);

    switch (step) {
    case 0: t->CCR1 = d; t->CCR2 = 0; t->CCR3 = 0; HAL_GPIO_WritePin(LS_W_PORT, LS_W_PIN, GPIO_PIN_SET); break;
    case 1: t->CCR1 = d; t->CCR2 = 0; t->CCR3 = 0; HAL_GPIO_WritePin(LS_V_PORT, LS_V_PIN, GPIO_PIN_SET); break;
    case 2: t->CCR1 = 0; t->CCR2 = 0; t->CCR3 = d; HAL_GPIO_WritePin(LS_V_PORT, LS_V_PIN, GPIO_PIN_SET); break;
    case 3: t->CCR1 = 0; t->CCR2 = 0; t->CCR3 = d; HAL_GPIO_WritePin(LS_U_PORT, LS_U_PIN, GPIO_PIN_SET); break;
    case 4: t->CCR1 = 0; t->CCR2 = d; t->CCR3 = 0; HAL_GPIO_WritePin(LS_U_PORT, LS_U_PIN, GPIO_PIN_SET); break;
    case 5: t->CCR1 = 0; t->CCR2 = d; t->CCR3 = 0; HAL_GPIO_WritePin(LS_W_PORT, LS_W_PIN, GPIO_PIN_SET); break;
    }
}

/* Hall→step: calibrated from S=0:H5,1:H4,2:H6,3:H2,4:H3,5:H1 */
static const int8_t hall_map[8] = { -1, 5, 3, 4, 1, 0, 2, -1 };

/* ======================== Public API ======================== */

void APP_Motor_Init(void)
{
    motor_state = MOTOR_STOP;
}

void APP_Motor_Start(void)
{
    if (motor_state != MOTOR_STOP) return;
    EG3112_Enable();
    motor_state = MOTOR_ALIGN;
    last_speed  = motor_speed ? motor_speed : 50;
    align_start = APP_System_GetTick();
}

void APP_Motor_Stop(void)
{
    EG3112_Disable();
    motor_state = MOTOR_STOP;
}

void APP_Motor_EmergencyStop(void)
{
    EG3112_Brake();
    motor_state = MOTOR_FAULT;
}

void APP_Motor_SetSpeed(uint8_t duty)
{
    motor_speed = duty;
}

uint8_t APP_Motor_GetSpeed(void)
{
    return motor_speed;
}

void APP_Motor_Run(void)
{
    switch (motor_state) {

    case MOTOR_ALIGN:
        /* 100 ms lock to U→V, then closed-loop */
        if (APP_System_GetTick() - align_start < 100) {
            TIM_TypeDef *t = htim1.Instance;
            HAL_GPIO_WritePin(LS_U_PORT, LS_U_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LS_V_PORT, LS_V_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LS_W_PORT, LS_W_PIN, GPIO_PIN_SET);
            t->CCR1 = (uint16_t)80 * t->ARR / 255;
            t->CCR2 = 0;
            t->CCR3 = 0;
        } else {
            motor_state = MOTOR_RUN;
        }
        break;

    case MOTOR_RUN: {
        int8_t s = hall_map[read_hall() & 0x07];
        if (s >= 0) six_step((uint8_t)s, last_speed);
        break;
    }

    case MOTOR_STOP:
    case MOTOR_FAULT:
    default:
        break;
    }
}

MotorState APP_Motor_GetState(void)
{
    return motor_state;
}

uint8_t APP_Motor_GetHall(void)
{
    return read_hall();
}

uint16_t APP_Motor_GetCurrent(void)
{
    uint32_t max_adc = 0;
    for (uint8_t i = 0; i < 3; i++) {
        uint32_t v = BSP_ADC_GetChannel(ADC_CH_CURRENT_U + i);
        if (v > max_adc) max_adc = v;
    }
    return (uint16_t)max_adc;
}
