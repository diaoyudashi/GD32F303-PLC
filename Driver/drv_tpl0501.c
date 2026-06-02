#include "drv_tpl0501.h"
#include "bsp_gpio.h"

static void TPL0501_Delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 18; i++) { __NOP(); }
}

/* CS1 + CS2 select which chip; SCLK + DIN are shared */
static void TPL0501_SCLK_Low(void)  { HAL_GPIO_WritePin(DPOT_SCLK_PORT, DPOT_SCLK_PIN, GPIO_PIN_RESET); }
static void TPL0501_SCLK_High(void) { HAL_GPIO_WritePin(DPOT_SCLK_PORT, DPOT_SCLK_PIN, GPIO_PIN_SET); }
static void TPL0501_DIN_Low(void)   { HAL_GPIO_WritePin(DPOT_DIN_PORT, DPOT_DIN_PIN, GPIO_PIN_RESET); }
static void TPL0501_DIN_High(void)  { HAL_GPIO_WritePin(DPOT_DIN_PORT, DPOT_DIN_PIN, GPIO_PIN_SET); }
static void TPL0501_CS1_Low(void)   { HAL_GPIO_WritePin(DPOT_CS1_PORT, DPOT_CS1_PIN, GPIO_PIN_RESET); }
static void TPL0501_CS1_High(void)  { HAL_GPIO_WritePin(DPOT_CS1_PORT, DPOT_CS1_PIN, GPIO_PIN_SET); }
static void TPL0501_CS2_Low(void)   { HAL_GPIO_WritePin(DPOT_CS2_PORT, DPOT_CS2_PIN, GPIO_PIN_RESET); }
static void TPL0501_CS2_High(void)  { HAL_GPIO_WritePin(DPOT_CS2_PORT, DPOT_CS2_PIN, GPIO_PIN_SET); }

void TPL0501_Init(void)
{
    /* GPIO already initialized by BSP_GPIO_Init() */
}

/* Write 8-bit value to TPL0501 #1 (CS=PC9) */
void TPL0501_SetChannel1(uint8_t val)
{
    uint8_t i;
    TPL0501_CS1_Low();
    TPL0501_Delay();

    for (i = 0; i < 8; i++) {
        TPL0501_SCLK_Low();
        if (val & 0x80) TPL0501_DIN_High();
        else            TPL0501_DIN_Low();
        TPL0501_Delay();
        TPL0501_SCLK_High();
        TPL0501_Delay();
        val <<= 1;
    }

    TPL0501_SCLK_Low();
    TPL0501_Delay();
    TPL0501_CS1_High();
}

/* Write 8-bit value to TPL0501 #2 (CS=PC5) */
void TPL0501_SetChannel2(uint8_t val)
{
    uint8_t i;
    TPL0501_CS2_Low();
    TPL0501_Delay();

    for (i = 0; i < 8; i++) {
        TPL0501_SCLK_Low();
        if (val & 0x80) TPL0501_DIN_High();
        else            TPL0501_DIN_Low();
        TPL0501_Delay();
        TPL0501_SCLK_High();
        TPL0501_Delay();
        val <<= 1;
    }

    TPL0501_SCLK_Low();
    TPL0501_Delay();
    TPL0501_CS2_High();
}

/* Write both chips sequentially (shared SCLK/DIN, independent CS) */
void TPL0501_Set(uint8_t ch1_val, uint8_t ch2_val)
{
    TPL0501_SetChannel1(ch1_val);
    TPL0501_SetChannel2(ch2_val);
}
