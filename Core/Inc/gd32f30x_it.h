#ifndef __GD32F30X_IT_H
#define __GD32F30X_IT_H
#include "gd32f30x.h"
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
#endif
