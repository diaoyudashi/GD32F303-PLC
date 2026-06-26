/**
 * @file    plc_io.h
 * @brief   PLC 软元件 X/Y 与 GD32F303ZET6 物理 IO 映射表
 * @note    与 PLC_20220210_V1.0.4 工程完全兼容
 */

#ifndef __PLC_IO_H
#define __PLC_IO_H

#include "gd32f30x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================== 位带操作宏 ======================== */
#define BITBAND(addr, bit)  *((volatile unsigned long *)(0x42000000 + ((unsigned long)(addr) - 0x40000000) * 32 + (bit) * 4))

/* GPIO 位带定义 */
#define PAout(n)  BITBAND(GPIOA + 0x0C, n)   /* PAx 输出 */
#define PAin(n)   BITBAND(GPIOA + 0x08, n)   /* PAx 输入 */
#define PBout(n)  BITBAND(GPIOB + 0x0C, n)
#define PBin(n)   BITBAND(GPIOB + 0x08, n)
#define PCout(n)  BITBAND(GPIOC + 0x0C, n)
#define PCin(n)   BITBAND(GPIOC + 0x08, n)
#define PDout(n)  BITBAND(GPIOD + 0x0C, n)
#define PDin(n)   BITBAND(GPIOD + 0x08, n)
#define PEout(n)  BITBAND(GPIOE + 0x0C, n)
#define PEin(n)   BITBAND(GPIOE + 0x08, n)
#define PFout(n)  BITBAND(GPIOF + 0x0C, n)
#define PFin(n)   BITBAND(GPIOF + 0x08, n)
#define PGout(n)  BITBAND(GPIOG + 0x0C, n)
#define PGin(n)   BITBAND(GPIOG + 0x08, n)

/* ======================== X 输入（软元件 → 物理IO） ======================== */

/* 伺服差分脉冲（5路，可测频率） */
#define X00  PAin(12)  /* 伺服1 差分脉冲 —— TIM1_ETR 输入捕获测频 */
#define X01  PAin(1)   /* 伺服2 差分脉冲 —— TIM2_CH2 输入捕获测频 */
#define X02  PDin(2)   /* 伺服3 差分脉冲 —— TIM3_ETR 输入捕获测频 */
#define X03  PDin(13)  /* 伺服4 差分脉冲 —— TIM4_CH2 输入捕获测频 */
#define X04  PCin(7)   /* 伺服5 差分脉冲 —— TIM3_CH2 输入捕获测频 */

/* 电机圈数 / 开关量 */
#define X05  PCin(9)   /* 电机圈数 COUNT 光电开关 ~2000-3000ms/圈（EXTI 中断测周期）*/

/* 运行控制 */
#define X06  PAin(8)   /* 安全门 SafeDoor 输入 */
#define X07  PGin(7)   /* RUN 运行输入 */

/* 操作输入 */
#define X10  PCin(8)   /* STOP 停止输入 */
#define X11  PGin(4)   /* JOB 点动输入 */
#define X12  PGin(5)   /* 变频器故障输入 */

/* 伺服故障（5路） */
#define X13  PEin(15)  /* 伺服1 故障输入 */
#define X14  PEin(14)  /* 伺服2 故障输入 */
#define X15  PEin(13)  /* 伺服3 故障输入 */
#define X16  PEin(12)  /* 伺服4 故障输入 */
#define X17  PGin(6)   /* 伺服5 故障输入 */

/* 电源及系统 */
#define X20  PGin(3)   /* 断电检测 PVD */

/* 故障输入 */
#define X21  PCin(3)   /* 保险故障检测 */
#define X22  PCin(5)   /* 探针故障 */
#define X23  PBin(0)   /* 探布故障 */
#define X24  PBin(1)   /* 缺油故障 */
#define X25  PAin(6)   /* 上断纱故障 */
#define X26  PAin(7)   /* 中断纱故障 */
#define X27  PCin(4)   /* 下断纱故障 */

/* 其他 */
#define X30  PFin(11)  /* 缺气故障 */
/* X31~X34 备用 */

/* ======================== Y 输出（软元件 → 物理IO） ======================== */

/* 设备控制 */
#define Y00  PBout(9)  /* 风扇 */
#define Y01  PBout(8)  /* 上布灯 */
#define Y02  PBout(5)  /* 下布灯 */
#define Y03  PGout(15) /* 油机 */
#define Y04  PGout(14) /* 开幅 */
#define Y05  PGout(13) /* 变频器通电 */
#define Y06  PGout(12) /* 中央除尘 */
#define Y07  PGout(11) /* 喷气吹油 */

/* 断纱输出 */
#define Y10  PCout(13) /* 上断纱输出 */
#define Y11  PEout(6)  /* 中断纱输出 */
#define Y12  PEout(5)  /* 下断纱输出 */

/* 三色灯 */
#define Y13  PEout(4)  /* 三色灯 R 红灯 */
#define Y14  PEout(3)  /* 三色灯 G 绿灯 */
#define Y15  PEout(2)  /* 三色灯 Y 黄灯 */

/* 吹气 */
#define Y16  PEout(1)  /* 门口位吹气 */
#define Y17  PEout(0)  /* 中央吹气 */

/* 变频器 */
#define Y20  PFout(13) /* 变频器运行 */
#define Y21  PFout(14) /* 备用输出 */
#define Y22  PFout(15) /* 备用输出 */

/* ======================== 系统状态指示灯（非PLC软元件） ======================== */
#define LED_ERR_ON    PFout(9) = 0   /* 错误指示灯 PF9  (低电平亮) */
#define LED_ERR_OFF   PFout(9) = 1
#define LED_RUN_ON    PFout(10) = 0  /* 运行指示灯 PF10 (低电平亮) */
#define LED_RUN_OFF   PFout(10) = 1

#ifdef __cplusplus
}
#endif

#endif /* __PLC_IO_H */
