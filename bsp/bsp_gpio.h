/**
 * @file    bsp_gpio.h
 * @brief   GD32F303 PLC 全部 GPIO 初始化
 * @note    输入: 上拉/下拉按外部电路配置
 *          输出: 推挽, 默认全部关断
 */

#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "main.h"

void BSP_GPIO_Init(void);

#endif
