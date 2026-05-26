#include "app_motor.h"
#include "bsp_tim.h"
#include "bsp_adc.h"
#include "bsp_gpio.h"
#include "drv_eg3112.h"
#include "utils_filter.h"
#include "app_sensor.h"
#include "app_config.h"

/* 6-step commutation table: hall (U,V,W) → active phase pair
 * Hall code order: U=bit2, V=bit1, W=bit0
 * Each entry: {high_side_ch, low_side_ch} where ch=1,2,3 for TIM1 CH1/2/3 */
typedef struct {
    uint8_t hs;  /* high-side channel (TIM1 CHx) */
    uint8_t ls;  /* low-side channel (TIM1 CHx) */
} CommutationStep;

/* Standard 120° Hall commutation table:
 * Hall: 001→W, 010→V, 011→V, 100→U, 101→U, 110→W
 * Step: HS→LS = current path */
static const CommutationStep comm_table[8] = {
    [0] = {0, 0},  /* 000: invalid */
    [1] = {3, 2},  /* 001: W(H) → V(L)  → CH2 H, CH1 L (phase W→V) */
    [2] = {1, 3},  /* 010: U(H) → W(L)  → CH1 H, CH3 L (phase U→W) */
    [3] = {2, 3},  /* 011: V(H) → W(L)  → CH2 H, CH3 L (phase V→W) */
    [4] = {2, 1},  /* 100: V(H) → U(L)  → CH2 H, CH1 L (phase V→U) */
    [5] = {3, 1},  /* 101: W(H) → U(L)  → CH3 H, CH1 L (phase W→U) */
    [6] = {1, 2},  /* 110: U(H) → V(L)  → CH1 H, CH2 L (phase U→V) */
    [7] = {0, 0},  /* 111: invalid */
};

static MotorState motor_state = MOTOR_STOP;
static uint8_t    motor_duty = 0;
static uint16_t   motor_speed = 0;
static uint32_t   motor_current[3] = {0};
static LPF_1st    current_filter[3];

/* ---- Sensor-driven speed control ---- */
#define SENSOR_SPEED_MIN_DUTY   51   /* ~20%, just above start threshold */
#define SENSOR_SPEED_MAX_DUTY   204  /* ~80%, max safe speed */
#define SENSOR_BLOCK_THRESHOLD  2000 /* ADC below this = "blocked" (hand present) */
#define SENSOR_FULL_BLOCK_VAL   200  /* ADC at this = fully blocked → max speed */

void APP_Motor_SetDuty(uint8_t duty)    { motor_duty = duty; }
uint8_t APP_Motor_GetDuty(void)         { return motor_duty; }

void APP_Motor_UpdateFromSensors(void)
{
    uint16_t s1 = APP_Sensor1_GetAnalog();
    uint16_t s2 = APP_Sensor2_GetAnalog();

    uint16_t raw = (s1 < s2) ? s1 : s2;  /* use the most blocked sensor */
    uint8_t  blocked = (raw < SENSOR_BLOCK_THRESHOLD);

    if (!blocked) {
        /* No sensor blocked → stop motor */
        if (motor_state == MOTOR_RUN) {
            APP_Motor_Stop();
        }
        motor_duty = 0;
        return;
    }

    /* Map ADC value linearly to duty: lower ADC = more blocked = faster
     * ADC range: SENSOR_FULL_BLOCK_VAL ~ SENSOR_BLOCK_THRESHOLD
     * Duty range: SENSOR_SPEED_MIN_DUTY ~ SENSOR_SPEED_MAX_DUTY */
    uint32_t span = SENSOR_BLOCK_THRESHOLD - SENSOR_FULL_BLOCK_VAL;
    uint32_t offset = SENSOR_BLOCK_THRESHOLD - raw;

    if (offset > span) offset = span;
    motor_duty = SENSOR_SPEED_MIN_DUTY + (uint8_t)(offset * (SENSOR_SPEED_MAX_DUTY - SENSOR_SPEED_MIN_DUTY) / span);

    /* Start motor if stopped */
    if (motor_state == MOTOR_STOP) {
        APP_Motor_Start();
    }
}

static void APP_Motor_DoCommutation(uint8_t hall)
{
    const CommutationStep *step = &comm_table[hall & 0x07];
    if (step->hs == 0) return; /* invalid state */

    uint8_t duty[3] = {0, 0, 0};
    duty[step->hs - 1] = motor_duty;
    duty[step->ls - 1] = motor_duty;

    EG3112_SetPhaseDuty(duty[0], duty[1], duty[2]);
}

static uint8_t APP_Motor_ReadHall(void)
{
    uint8_t h = 0;
    if (HAL_GPIO_ReadPin(HALL_U_PORT, HALL_U_PIN)) h |= 0x04;
    if (HAL_GPIO_ReadPin(HALL_V_PORT, HALL_V_PIN)) h |= 0x02;
    if (HAL_GPIO_ReadPin(HALL_W_PORT, HALL_W_PIN)) h |= 0x01;
    return h;
}

void APP_Motor_Init(void)
{
    for (uint8_t i = 0; i < 3; i++) {
        LPF_Init(&current_filter[i], ADC_FILTER_ALPHA);
    }
    motor_state = MOTOR_STOP;
}

void APP_Motor_Start(void)
{
    if (motor_state == MOTOR_STOP) {
        motor_state = MOTOR_ALIGN;
        motor_duty = 30;  /* low duty for alignment */
    }
}

void APP_Motor_Stop(void)
{
    EG3112_Disable();
    motor_state = MOTOR_STOP;
    motor_duty = 0;
}

void APP_Motor_EmergencyStop(void)
{
    EG3112_Brake();
    motor_state = MOTOR_FAULT;
    motor_duty = 0;
}

void APP_Motor_Run(void)
{
    switch (motor_state) {
    case MOTOR_ALIGN:
        /* Hold one phase active for rotor alignment */
        EG3112_Enable();
        EG3112_SetPhaseDuty(motor_duty, 0, 0);  /* energize U phase */
        /* After a delay (handled by soft timer elsewhere), transition to START */
        motor_state = MOTOR_RUN;  /* TODO: add proper align timer */
        break;

    case MOTOR_RUN: {
        uint8_t hall = APP_Motor_ReadHall();
        APP_Motor_DoCommutation(hall);

        /* Current limiting */
        uint32_t max_current = 0;
        for (uint8_t i = 0; i < 3; i++) {
            motor_current[i] = LPF_UpdateInt(&current_filter[i],
                                             BSP_ADC_GetChannel(ADC_CH_CURRENT_U + i));
            if (motor_current[i] > max_current) max_current = motor_current[i];
        }

        /* Speed estimation from encoder */
        motor_speed = 0;  /* TODO: compute RPM from TIM3 encoder + time delta */
        break;
    }

    case MOTOR_FAULT:
    case MOTOR_STOP:
    default:
        break;
    }
}

MotorState APP_Motor_GetState(void) { return motor_state; }
uint16_t   APP_Motor_GetSpeed(void) { return motor_speed; }

void APP_Motor_Hall_ISR(uint8_t hall_state)
{
    if (motor_state == MOTOR_RUN) {
        APP_Motor_DoCommutation(hall_state);
    }
}
