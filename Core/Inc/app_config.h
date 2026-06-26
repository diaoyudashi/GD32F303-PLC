/**
 * @file    app_config.h
 * @brief   GD32F303 PLC 系统参数配置
 */

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ======================== 系统时钟 ======================== */
#define SYSTEM_CLOCK_HZ         72000000U  /* HXTAL 8MHz → PLL ×30÷2 = 120MHz */
#define APB1_CLOCK_HZ           36000000U   /* AHB/2 */
#define APB2_CLOCK_HZ           72000000U  /* AHB/1 */
#define SYSTICK_FREQ_HZ         1000U       /* 1ms 时基 */

/* ======================== 串口 ======================== */
#define USART1_BAUD             19200U      /* PLC下载口 GX Works */
#define USART2_BAUD             19200U      /* Modbus232 昆仑通泰 LCD */
#define UART4_BAUD              19200U      /* Modbus485 伺服驱动器 */

/* ======================== PLC 参数 ======================== */
#define PLC_SCAN_MAX_US         200U        /* 最大扫描周期(us) */
#define PLC_TMR_COUNT           256         /* T 定时器个数 */
#define PLC_CNT_COUNT           256         /* C 计数器个数 */
#define PLC_X_COUNT             35          /* X 输入点数 X00~X34 */
#define PLC_Y_COUNT             23          /* Y 输出点数 Y00~Y22 */

/* ======================== DAC ======================== */
#define DAC_VREF_MV             3000U       /* 基准电压 3.0V */
#define DAC_VOUT_MAX_MV         10000U      /* 运放放大后最大 10V */

/* ======================== 中断优先级 ======================== */
#define NVIC_PRIO_SYSTICK       15          /* 最低 */
#define NVIC_PRIO_EXTI_COUNT    2           /* 电机圈数计数 */
#define NVIC_PRIO_TIM_ENC       1           /* 编码器/脉冲捕获 */
#define NVIC_PRIO_USART1        3           /* PLC下载口 */
#define NVIC_PRIO_USART2        4           /* LCD 232 */
#define NVIC_PRIO_UART4         4           /* 伺服 485 */
#define NVIC_PRIO_DMA           5           /* DMA */
#define NVIC_PRIO_PVD           6           /* 断电检测 */

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONFIG_H */
