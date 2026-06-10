#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* ======================== LED (NPN transistor drive) ======================= */
#define LED1_PIN                GPIO_PIN_1
#define LED1_PORT               GPIOA
#define LED2_PIN                GPIO_PIN_3
#define LED2_PORT               GPIOC

/* ======================== RS-485 =========================================== */
#define RS485_TX_PIN            GPIO_PIN_6
#define RS485_TX_PORT           GPIOB
#define RS485_RX_PIN            GPIO_PIN_7
#define RS485_RX_PORT           GPIOB
#define RS485_DIR_PIN           GPIO_PIN_2
#define RS485_DIR_PORT          GPIOB

/* ======================== EEPROM M93C46 (Microwire) ======================== */
#define EEPROM_CS_PIN           GPIO_PIN_11
#define EEPROM_CS_PORT          GPIOB
#define EEPROM_SK_PIN           GPIO_PIN_4
#define EEPROM_SK_PORT          GPIOC
#define EEPROM_DI_PIN           GPIO_PIN_7
#define EEPROM_DI_PORT          GPIOA
#define EEPROM_DO_PIN           GPIO_PIN_6
#define EEPROM_DO_PORT          GPIOA

/* ======================== TPL0501 Digital Pot ============================= */
/* TPL0501 pinout: pin4=SCLK, pin5=DIN, pin6=CS# */
/* Two chips share SCLK(PB3) + DIN(PB5), independent CS(PC9/PC5) */
#define DPOT_SCLK_PIN           GPIO_PIN_3
#define DPOT_SCLK_PORT          GPIOB
#define DPOT_DIN_PIN            GPIO_PIN_5
#define DPOT_DIN_PORT           GPIOB
#define DPOT_CS1_PIN            GPIO_PIN_9
#define DPOT_CS1_PORT           GPIOC
#define DPOT_CS2_PIN            GPIO_PIN_5
#define DPOT_CS2_PORT           GPIOC

/* ======================== BLDC Gate Driver EG3112 ========================== */
/* High-side: PA8(HIN)=TIM1_CH1, PA9=TIM1_CH2, PA10=TIM1_CH3 (AF in bsp_tim.c) */
/* Low-side: PB13(LIN_U), PB14(LIN_V), PB15(LIN_W) = GPIO push-pull */
#define LS_U_PIN                GPIO_PIN_13
#define LS_U_PORT               GPIOB
#define LS_V_PIN                GPIO_PIN_14
#define LS_V_PORT               GPIOB
#define LS_W_PIN                GPIO_PIN_15
#define LS_W_PORT               GPIOB

/* ======================== Hall Sensors ===================================== */
#define HALL_U_PIN              GPIO_PIN_1
#define HALL_U_PORT             GPIOB
#define HALL_V_PIN              GPIO_PIN_4
#define HALL_V_PORT             GPIOB
#define HALL_W_PIN              GPIO_PIN_11
#define HALL_W_PORT             GPIOC

/* ======================== Encoder ========================================== */
#define ENC_A_PIN               GPIO_PIN_6
#define ENC_A_PORT              GPIOC
#define ENC_B_PIN               GPIO_PIN_7
#define ENC_B_PORT              GPIOC
#define ENC_Z_PIN               GPIO_PIN_8
#define ENC_Z_PORT              GPIOC

/* ======================== Current Sense (ADC) ============================== */
#define CSENSE_U_PIN            GPIO_PIN_0
#define CSENSE_U_PORT           GPIOC
#define CSENSE_V_PIN            GPIO_PIN_1
#define CSENSE_V_PORT           GPIOC
#define CSENSE_W_PIN            GPIO_PIN_2
#define CSENSE_W_PORT           GPIOC

/* ======================== Photo Sensor ===================================== */
#define PHOTO1_STAGE1_PIN       GPIO_PIN_0
#define PHOTO1_STAGE1_PORT      GPIOB
#define PHOTO1_STAGE3_IN_PIN    GPIO_PIN_4
#define PHOTO1_STAGE3_IN_PORT   GPIOA
#define PHOTO1_OUT_PIN          GPIO_PIN_15
#define PHOTO1_OUT_PORT         GPIOA

#define PHOTO2_STAGE1_PIN       GPIO_PIN_0
#define PHOTO2_STAGE1_PORT      GPIOA
#define PHOTO2_STAGE3_IN_PIN    GPIO_PIN_5
#define PHOTO2_STAGE3_IN_PORT   GPIOA
#define PHOTO2_OUT_PIN          GPIO_PIN_9
#define PHOTO2_OUT_PORT         GPIOB

/* PB8 = spare interrupt input (no ADC) */
#define PB8_PIN                 GPIO_PIN_8
#define PB8_PORT                GPIOB

/* ======================== API ============================================== */
void BSP_GPIO_Init(void);

/* Helper inline for RS-485 direction control */
static inline void BSP_RS485_TX_Mode(void)  { HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_PIN, GPIO_PIN_SET); }
static inline void BSP_RS485_RX_Mode(void)  { HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_PIN, GPIO_PIN_RESET); }

#ifdef __cplusplus
}
#endif

#endif /* __BSP_GPIO_H */
