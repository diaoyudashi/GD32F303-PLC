#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================= System Clock ================================ */
#define SYSTEM_CLOCK_HZ         72000000U
#define APB1_CLOCK_HZ           36000000U
#define APB2_CLOCK_HZ           72000000U
#define SYSTICK_FREQ_HZ         1000U

/* =========================== BLDC Motor PWM ================================ */
#define MOTOR_PWM_FREQ_HZ       16000U
#define MOTOR_PWM_DEADTIME_NS   1000U

/* ============================= ADC Sampling ================================ */
#define ADC_BUF_SIZE            5
#define ADC_FILTER_ALPHA        0.1f

/* ============================= RS-485 ====================================== */
#define RS485_BAUDRATE          115200U
#define RS485_RXBUF_SIZE        256U
#define RS485_TXBUF_SIZE        256U
#define RS485_FRAME_TIMEOUT_MS  5U

/* ============================= CAN ========================================= */
#define CAN_BAUDRATE            500000U
#define CAN_RXBUF_SIZE          64U
#define CAN_TXBUF_SIZE          64U

/* ============================= EEPROM ====================================== */
#define EEPROM_START_ADDR       0x00
#define EEPROM_SIZE             64

/* ============================= IRQ Priorities (0=highest, 15=lowest) ======== */
#define NVIC_PRIO_TIM1_BRK      0
#define NVIC_PRIO_TIM1_UP       1
#define NVIC_PRIO_HALL_EXTI     2
#define NVIC_PRIO_CAN_RX0       3
#define NVIC_PRIO_USART1        4
#define NVIC_PRIO_USART2        5
#define NVIC_PRIO_ADC_DMA       6
#define NVIC_PRIO_TIM3          7
#define NVIC_PRIO_SYSTICK       15

/* ============================= Software Timers ============================== */
#define SOFTTIMER_MAX           8

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONFIG_H */
