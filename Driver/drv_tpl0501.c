#include "drv_tpl0501.h"
#include "bsp_gpio.h"

static void TPL0501_Delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 18; i++) { __NOP(); }
}

static void TPL0501_CS_Low(void)  { HAL_GPIO_WritePin(DPOT_CS_PORT, DPOT_CS_PIN, GPIO_PIN_RESET); }
static void TPL0501_CS_High(void) { HAL_GPIO_WritePin(DPOT_CS_PORT, DPOT_CS_PIN, GPIO_PIN_SET); }
static void TPL0501_SCLK_Low(void)  { HAL_GPIO_WritePin(DPOT_SCLK_PORT, DPOT_SCLK_PIN, GPIO_PIN_RESET); }
static void TPL0501_SCLK_High(void) { HAL_GPIO_WritePin(DPOT_SCLK_PORT, DPOT_SCLK_PIN, GPIO_PIN_SET); }
static void TPL0501_DIN1_Low(void)  { HAL_GPIO_WritePin(DPOT_DIN1_PORT, DPOT_DIN1_PIN, GPIO_PIN_RESET); }
static void TPL0501_DIN1_High(void) { HAL_GPIO_WritePin(DPOT_DIN1_PORT, DPOT_DIN1_PIN, GPIO_PIN_SET); }
static void TPL0501_DIN2_Low(void)  { HAL_GPIO_WritePin(DPOT_DIN2_PORT, DPOT_DIN2_PIN, GPIO_PIN_RESET); }
static void TPL0501_DIN2_High(void) { HAL_GPIO_WritePin(DPOT_DIN2_PORT, DPOT_DIN2_PIN, GPIO_PIN_SET); }

void TPL0501_Init(void)
{
    /* GPIO already initialized by BSP_GPIO_Init() */
}

/* Write 8-bit value to channel 1 */
void TPL0501_SetChannel1(uint8_t val)
{
    uint8_t i;
    TPL0501_CS_Low();
    TPL0501_Delay();

    for (i = 0; i < 8; i++) {
        TPL0501_SCLK_Low();
        if (val & 0x80) {
            TPL0501_DIN1_High();
        } else {
            TPL0501_DIN1_Low();
        }
        TPL0501_Delay();
        TPL0501_SCLK_High();
        TPL0501_Delay();
        val <<= 1;
    }

    TPL0501_SCLK_Low();
    TPL0501_Delay();
    TPL0501_CS_High();
}

/* Write 8-bit value to channel 2 */
void TPL0501_SetChannel2(uint8_t val)
{
    uint8_t i;
    TPL0501_CS_Low();
    TPL0501_Delay();

    for (i = 0; i < 8; i++) {
        TPL0501_SCLK_Low();
        if (val & 0x80) {
            TPL0501_DIN2_High();
        } else {
            TPL0501_DIN2_Low();
        }
        TPL0501_Delay();
        TPL0501_SCLK_High();
        TPL0501_Delay();
        val <<= 1;
    }

    TPL0501_SCLK_Low();
    TPL0501_Delay();
    TPL0501_CS_High();
}

/* Simultaneously program both channels (shared CS/SCLK, independent DIN) */
void TPL0501_Set(uint8_t ch1_val, uint8_t ch2_val)
{
    uint8_t i;
    TPL0501_CS_Low();
    TPL0501_Delay();

    for (i = 0; i < 8; i++) {
        TPL0501_SCLK_Low();

        if (ch1_val & 0x80) TPL0501_DIN1_High();
        else TPL0501_DIN1_Low();

        if (ch2_val & 0x80) TPL0501_DIN2_High();
        else TPL0501_DIN2_Low();

        TPL0501_Delay();
        TPL0501_SCLK_High();
        TPL0501_Delay();

        ch1_val <<= 1;
        ch2_val <<= 1;
    }

    TPL0501_SCLK_Low();
    TPL0501_Delay();
    TPL0501_CS_High();
}
