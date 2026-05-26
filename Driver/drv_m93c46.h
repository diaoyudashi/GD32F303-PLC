#ifndef __DRV_M93C46_H
#define __DRV_M93C46_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* M93C46 Microwire EEPROM driver (16-bit organization)
 *
 * Pinout:
 *   PB11 = CS  (chip select)
 *   PC4  = SK  (serial clock)
 *   PA7  = DI  (data input to EEPROM)
 *   PA6  = DO  (data output from EEPROM)
 */

/* Instruction set (16-bit org) */
#define M93C46_OP_READ   0x06U  /* 110 */
#define M93C46_OP_WRITE  0x05U  /* 101 */
#define M93C46_OP_ERASE  0x07U  /* 111 */
#define M93C46_OP_EWEN   0x04U  /* 100 11XXXXX */
#define M93C46_OP_EWDS   0x04U  /* 100 00XXXXX */
#define M93C46_OP_ERAL   0x04U  /* 100 10XXXXX */
#define M93C46_OP_WRAL   0x04U  /* 100 01XXXXX */

void M93C46_Init(void);
void M93C46_WriteEnable(void);
void M93C46_WriteDisable(void);
uint16_t M93C46_Read(uint8_t addr);
void M93C46_Write(uint8_t addr, uint16_t data);
void M93C46_EraseAll(void);
void M93C46_WriteAll(uint16_t data);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_M93C46_H */
