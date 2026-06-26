/**
 * @file    main.h
 * @brief   GD32F303 PLC 主头文件
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f30x.h"
#include "app_config.h"
#include "plc_io.h"

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
