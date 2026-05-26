#include "bsp_gpio.h"

void BSP_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    /* GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    /* ---- Outputs: set default level first ---- */
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_PIN, GPIO_PIN_RESET);  /* RX mode default */
    HAL_GPIO_WritePin(EEPROM_CS_PORT, EEPROM_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EEPROM_SK_PORT, EEPROM_SK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EEPROM_DI_PORT, EEPROM_DI_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DPOT_CS_PORT, DPOT_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DPOT_SCLK_PORT, DPOT_SCLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DPOT_DIN1_PORT, DPOT_DIN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DPOT_DIN2_PORT, DPOT_DIN2_PIN, GPIO_PIN_RESET);

    /* ---- Outputs ---- */
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    gpio.Pin = LED1_PIN;
    HAL_GPIO_Init(LED1_PORT, &gpio);
    gpio.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_PORT, &gpio);

    gpio.Pin = RS485_DIR_PIN;
    HAL_GPIO_Init(RS485_DIR_PORT, &gpio);

    gpio.Pin = EEPROM_CS_PIN;
    HAL_GPIO_Init(EEPROM_CS_PORT, &gpio);
    gpio.Pin = EEPROM_SK_PIN;
    HAL_GPIO_Init(EEPROM_SK_PORT, &gpio);
    gpio.Pin = EEPROM_DI_PIN;
    HAL_GPIO_Init(EEPROM_DI_PORT, &gpio);

    gpio.Pin = DPOT_CS_PIN;
    HAL_GPIO_Init(DPOT_CS_PORT, &gpio);
    gpio.Pin = DPOT_SCLK_PIN;
    HAL_GPIO_Init(DPOT_SCLK_PORT, &gpio);
    gpio.Pin = DPOT_DIN1_PIN;
    HAL_GPIO_Init(DPOT_DIN1_PORT, &gpio);
    gpio.Pin = DPOT_DIN2_PIN;
    HAL_GPIO_Init(DPOT_DIN2_PORT, &gpio);

    /* ---- Inputs ---- */
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;

    /* EEPROM DO */
    gpio.Pin = EEPROM_DO_PIN;
    HAL_GPIO_Init(EEPROM_DO_PORT, &gpio);

    /* Hall sensors */
    gpio.Pin = HALL_U_PIN;
    HAL_GPIO_Init(HALL_U_PORT, &gpio);
    gpio.Pin = HALL_V_PIN;
    HAL_GPIO_Init(HALL_V_PORT, &gpio);
    gpio.Pin = HALL_W_PIN;
    HAL_GPIO_Init(HALL_W_PORT, &gpio);

    /* Encoder (TIM3 AF will override in bsp_tim, but init as input first) */
    gpio.Pin = ENC_A_PIN;
    HAL_GPIO_Init(ENC_A_PORT, &gpio);
    gpio.Pin = ENC_B_PIN;
    HAL_GPIO_Init(ENC_B_PORT, &gpio);
    gpio.Pin = ENC_Z_PIN;
    HAL_GPIO_Init(ENC_Z_PORT, &gpio);

    /* Current sense (analog inputs, configured by bsp_adc) */

    /* Photo sensor stage1 monitors (analog, configured by bsp_adc) */

    /* Photo sensor stage3 outputs (digital comparators) */
    gpio.Pin = PHOTO1_OUT_PIN;
    HAL_GPIO_Init(PHOTO1_OUT_PORT, &gpio);
    gpio.Pin = PHOTO2_OUT_PIN;
    HAL_GPIO_Init(PHOTO2_OUT_PORT, &gpio);
}
