#include "app_eeprom.h"
#include "drv_m93c46.h"

#define EEPROM_MAGIC 0xA55AU

static uint16_t eeprom_shadow[64];  /* RAM copy of EEPROM contents */

static uint16_t CRC16_Calc(const uint16_t *data, uint8_t count)
{
    uint16_t crc = 0xFFFF;
    while (count--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 16; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void APP_EEPROM_Init(void)
{
    M93C46_Init();
}

void APP_EEPROM_LoadDefaults(void)
{
    /* Write default values into shadow RAM */
    for (uint8_t i = 0; i < 64; i++) eeprom_shadow[i] = 0xFFFF;
    eeprom_shadow[EEPROM_ADDR_MAGIC] = EEPROM_MAGIC;
    eeprom_shadow[EEPROM_ADDR_MOTOR_MAXSPD] = 3000;   /* 3000 RPM */
    eeprom_shadow[EEPROM_ADDR_MOTOR_POLES] = 4;       /* 4-pole motor */
    eeprom_shadow[EEPROM_ADDR_MOTOR_KP] = 100;
    eeprom_shadow[EEPROM_ADDR_MOTOR_KI] = 10;
    eeprom_shadow[EEPROM_ADDR_CAN_ID] = 0x001;
    eeprom_shadow[EEPROM_ADDR_RS485_ADDR] = 0x01;
    eeprom_shadow[EEPROM_ADDR_SENSOR1_GAIN] = 128;
    eeprom_shadow[EEPROM_ADDR_SENSOR2_GAIN] = 128;
    eeprom_shadow[EEPROM_ADDR_SENSOR1_TH] = 128;
    eeprom_shadow[EEPROM_ADDR_SENSOR2_TH] = 128;
}

void APP_EEPROM_Load(void)
{
    for (uint8_t i = 0; i < 64; i++) {
        eeprom_shadow[i] = M93C46_Read(i);
    }
}

void APP_EEPROM_Save(void)
{
    /* Calculate CRC over all data except CRC word */
    eeprom_shadow[EEPROM_ADDR_CRC] = CRC16_Calc(eeprom_shadow, 63);

    M93C46_WriteEnable();
    for (uint8_t i = 0; i < 64; i++) {
        M93C46_Write(i, eeprom_shadow[i]);
    }
}

uint8_t APP_EEPROM_IsValid(void)
{
    if (eeprom_shadow[EEPROM_ADDR_MAGIC] != EEPROM_MAGIC) return 0;
    uint16_t calc = CRC16_Calc(eeprom_shadow, 63);
    return (calc == eeprom_shadow[EEPROM_ADDR_CRC]) ? 1 : 0;
}

uint16_t APP_EEPROM_Read(uint8_t addr)
{
    if (addr < 64) return eeprom_shadow[addr];
    return 0xFFFF;
}

void APP_EEPROM_Write(uint8_t addr, uint16_t val)
{
    if (addr < 63) {
        eeprom_shadow[addr] = val;
    }
}
