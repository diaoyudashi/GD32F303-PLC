#ifndef __APP_FOC_H
#define __APP_FOC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
void FOC_Init(void);
void FOC_Run(void);
void FOC_Stop(void);
#ifdef __cplusplus
}
#endif
#endif
