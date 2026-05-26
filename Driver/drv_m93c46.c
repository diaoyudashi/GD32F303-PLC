#include "drv_m93c46.h"
#include "bsp_gpio.h"

/* Internal soft-delay for timing (approx 1us operations) */
static void M93C46_Delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 36; i++) { __NOP(); }
}

static void M93C46_SK_High(void) { HAL_GPIO_WritePin(EEPROM_SK_PORT, EEPROM_SK_PIN, GPIO_PIN_SET); }
static void M93C46_SK_Low(void)  { HAL_GPIO_WritePin(EEPROM_SK_PORT, EEPROM_SK_PIN, GPIO_PIN_RESET); }
static void M93C46_CS_High(void) { HAL_GPIO_WritePin(EEPROM_CS_PORT, EEPROM_CS_PIN, GPIO_PIN_SET); }
static void M93C46_CS_Low(void)  { HAL_GPIO_WritePin(EEPROM_CS_PORT, EEPROM_CS_PIN, GPIO_PIN_RESET); }
static void M93C46_DI_High(void) { HAL_GPIO_WritePin(EEPROM_DI_PORT, EEPROM_DI_PIN, GPIO_PIN_SET); }
static void M93C46_DI_Low(void)  { HAL_GPIO_WritePin(EEPROM_DI_PORT, EEPROM_DI_PIN, GPIO_PIN_RESET); }
static uint8_t M93C46_DO_Read(void) { return (HAL_GPIO_ReadPin(EEPROM_DO_PORT, EEPROM_DO_PIN) == GPIO_PIN_SET) ? 1 : 0; }

/* Send a start bit (CS low→high, then DI high with SK high→low) */
static void M93C46_SendStart(void)
{
    M93C46_CS_High();
    M93C46_Delay();
    M93C46_DI_High();
    M93C46_Delay();
    M93C46_SK_High();
    M93C46_Delay();
    M93C46_SK_Low();
    M93C46_Delay();
}

/* Send N bits to the device (MSB first) */
static void M93C46_SendBits(uint16_t val, uint8_t nbits)
{
    while (nbits--) {
        if (val & (1U << nbits)) {
            M93C46_DI_High();
        } else {
            M93C46_DI_Low();
        }
        M93C46_Delay();
        M93C46_SK_High();
        M93C46_Delay();
        M93C46_SK_Low();
        M93C46_Delay();
    }
}

/* Read N bits from the device (MSB first) */
static uint16_t M93C46_ReadBits(uint8_t nbits)
{
    uint16_t val = 0;
    GPIO_InitTypeDef gpio = {0};

    /* Switch DO back to input (if previously used as output) */
    gpio.Pin = EEPROM_DO_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(EEPROM_DO_PORT, &gpio);

    while (nbits--) {
        M93C46_SK_High();
        M93C46_Delay();
        val = (val << 1) | M93C46_DO_Read();
        M93C46_SK_Low();
        M93C46_Delay();
    }
    return val;
}

void M93C46_Init(void)
{
    /* GPIO already initialized by BSP_GPIO_Init() */
}

void M93C46_WriteEnable(void)
{
    M93C46_SendStart();
    M93C46_SendBits(M93C46_OP_EWEN, 3);
    M93C46_SendBits(0x0180, 9);  /* 11XXXXX in 9 bits (16-bit org) */
    M93C46_CS_Low();
    M93C46_Delay();
}

void M93C46_WriteDisable(void)
{
    M93C46_SendStart();
    M93C46_SendBits(M93C46_OP_EWDS, 3);
    M93C46_SendBits(0x0000, 9);  /* 00XXXXX */
    M93C46_CS_Low();
    M93C46_Delay();
}

uint16_t M93C46_Read(uint8_t addr)
{
    uint16_t val;
    M93C46_WriteEnable();
    M93C46_SendStart();
    M93C46_SendBits(M93C46_OP_READ, 3);
    M93C46_SendBits(addr & 0x3F, 6);   /* 6-bit address (16-bit org, 64 words) */
    M93C46_DI_Low();                     /* dummy 0 */
    M93C46_Delay();
    val = M93C46_ReadBits(16);
    M93C46_CS_Low();
    return val;
}

void M93C46_Write(uint8_t addr, uint16_t data)
{
    M93C46_WriteEnable();
    M93C46_SendStart();
    M93C46_SendBits(M93C46_OP_WRITE, 3);
    M93C46_SendBits(addr & 0x3F, 6);
    M93C46_SendBits(data, 16);
    M93C46_CS_Low();
    M93C46_Delay();

    /* Wait for write completion (DO goes high when ready) */
    M93C46_CS_High();
    M93C46_Delay();
    {
        uint32_t timeout = 100000;
        while (M93C46_DO_Read() == 0 && timeout--) {
            __NOP();
        }
    }
    M93C46_CS_Low();
}

void M93C46_EraseAll(void)
{
    M93C46_WriteEnable();
    M93C46_SendStart();
    M93C46_SendBits(M93C46_OP_ERAL, 3);
    M93C46_SendBits(0x0100, 9);  /* 10XXXXX */
    M93C46_CS_Low();
    M93C46_Delay();

    /* Wait for completion */
    M93C46_CS_High();
    M93C46_Delay();
    {
        uint32_t timeout = 100000;
        while (M93C46_DO_Read() == 0 && timeout--) {
            __NOP();
        }
    }
    M93C46_CS_Low();
}

void M93C46_WriteAll(uint16_t data)
{
    M93C46_WriteEnable();
    M93C46_SendStart();
    M93C46_SendBits(M93C46_OP_WRAL, 3);
    M93C46_SendBits(0x0080, 9);  /* 01XXXXX */
    M93C46_SendBits(data, 16);
    M93C46_CS_Low();
    M93C46_Delay();

    M93C46_CS_High();
    M93C46_Delay();
    {
        uint32_t timeout = 100000;
        while (M93C46_DO_Read() == 0 && timeout--) {
            __NOP();
        }
    }
    M93C46_CS_Low();
}
