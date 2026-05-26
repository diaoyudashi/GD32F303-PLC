#ifndef __APP_EEPROM_H
#define __APP_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Parameter layout in EEPROM (16-bit words, address 0-63 for M93C46) */
#define EEPROM_ADDR_MAGIC       0   /* 2 bytes: magic number 0xA55A */
#define EEPROM_ADDR_MOTOR_MAXSPD 1
#define EEPROM_ADDR_MOTOR_POLES  2
#define EEPROM_ADDR_MOTOR_KP     3
#define EEPROM_ADDR_MOTOR_KI     4
#define EEPROM_ADDR_CAN_ID       5
#define EEPROM_ADDR_RS485_ADDR   6
#define EEPROM_ADDR_SENSOR1_GAIN 7
#define EEPROM_ADDR_SENSOR2_GAIN 8
#define EEPROM_ADDR_SENSOR1_TH   9
#define EEPROM_ADDR_SENSOR2_TH   10
#define EEPROM_ADDR_CRC          63  /* last word: CRC16 */

void APP_EEPROM_Init(void);
void APP_EEPROM_LoadDefaults(void);
void APP_EEPROM_Load(void);
void APP_EEPROM_Save(void);
uint8_t APP_EEPROM_IsValid(void);

/* Individual parameter access */
uint16_t APP_EEPROM_Read(uint8_t addr);
void     APP_EEPROM_Write(uint8_t addr, uint16_t val);

#ifdef __cplusplus
}
#endif

#endif /* __APP_EEPROM_H */
