/**
 * @file    gxworks.c
 * @brief   GX Works 通讯协议 — 基本帧处理 + 读PLC型号
 *
 *  协议:  Mitsubishi MELSEC FX 编程口协议
 *  帧格式: STX(0x02) + 命令 + 数据 + ETX(0x03) + 校验H + 校验L
 */

#include "main.h"
#include "bsp_uart.h"

/* ASCII 十六进制表 */
static const char Ascll[] = "0123456789ABCDEF";

/* PLC 型号数据 (模拟 FX2N) */
#define PLC_TYPE_STR  "FX2N"

/* ---- 帧缓冲 ---- */
static uint8_t  gx_rx[256];
static uint16_t gx_cnt = 0;
static uint8_t  gx_frame_ready = 0;

/* ---- 发送帧 ---- */
static void gx_send_frame(uint8_t *data, uint16_t len)
{
    static uint8_t tx_buf[280];
    uint16_t i;
    uint8_t  sum = 0;

    tx_buf[0] = 0x02;  /* STX */
    for (i = 0; i < len; i++) {
        tx_buf[i + 1] = data[i];
        sum += data[i];
    }
    tx_buf[len + 1] = 0x03;  /* ETX */

    /* checksum: lower 8 bits, ASCII encoded */
    uint8_t cs = sum & 0xFF;
    tx_buf[len + 2] = Ascll[(cs >> 4) & 0x0F];
    tx_buf[len + 3] = Ascll[cs & 0x0F];

    BSP_USART_SendBuf(COM_PLC, tx_buf, len + 4);
}

/* ---- 计算帧校验 ---- */
static uint8_t gx_checksum(uint8_t *frame, uint16_t len)
{
    uint8_t sum = 0;
    /* frame[0]=STX, data from [1], up to ETX at [len-3] */
    for (uint16_t i = 1; i < len - 2; i++) {
        sum += frame[i];
    }
    return sum & 0xFF;
}

/* ---- 处理命令帧 ---- */
static void gx_process_frame(void)
{
    if (gx_cnt < 4) return;  /* 至少 STX + cmd + ETX + csH + csL */

    /* 验证 ETX */
    if (gx_rx[gx_cnt - 3] != 0x03) return;

    /* 验证校验和 */
    uint8_t calc_cs = gx_checksum(gx_rx, gx_cnt);
    uint8_t csH = 0, csL = 0;
    /* ASCII hex -> binary */
    char cH = gx_rx[gx_cnt - 2], cL = gx_rx[gx_cnt - 1];
    if (cH >= '0' && cH <= '9') csH = cH - '0';
    else if (cH >= 'A' && cH <= 'F') csH = cH - 'A' + 10;
    if (cL >= '0' && cL <= '9') csL = cL - '0';
    else if (cL >= 'A' && cL <= 'F') csL = cL - 'A' + 10;
    uint8_t frame_cs = (csH << 4) | csL;

    if (calc_cs != frame_cs) return;  /* 校验失败, 忽略 */

    /* 解析命令 (ASCII 编码) */
    /* 命令格式: cmd(2char) + data(start_addr 4char) + len(2char) */
    /* 简化: 命令字节在 gx_rx[1..2] */
    char cmd[3] = {gx_rx[1], gx_rx[2], 0};

    /* "30" = 读16位数据 (包括PLC型号) */
    if (cmd[0] == '3' && cmd[1] == '0') {
        /* 构造响应: 每个16位值 → 4字节 ASCII hex */
        uint8_t resp[128];
        uint16_t ri = 0;

        /* 读的起始地址在 gx_rx[3..6] (ASCII) */
        /* 读取长度在 gx_rx[7..8] (ASCII) */
        /* 简化: 假设读 D8001 (PLC 型号地址), 返回固定值 0x5EF6 = FX2N(C) */
        uint16_t val = 0x5EF6;  /* PLC型号标志 */
        resp[ri++] = Ascll[(val >> 12) & 0xF];
        resp[ri++] = Ascll[(val >> 8) & 0xF];
        resp[ri++] = Ascll[(val >> 4) & 0xF];
        resp[ri++] = Ascll[val & 0xF];

        gx_send_frame(resp, ri);
        return;
    }

    /* 其他命令: 回 ACK (0x06) */
    {
        uint8_t ack = 0x06;
        gx_send_frame(&ack, 1);
    }
}

/* ---- 轮询: 主循环中调用 ---- */
void GXWorks_Poll(void)
{
    if (gx_frame_ready) {
        gx_frame_ready = 0;
        gx_process_frame();
    }

    /* 从 USART1 环缓冲读取字节 */
    uint8_t d;
    while (BSP_USART_GetRxData(COM_PLC, &d, 1) > 0) {
        if (d == 0x02) {
            /* STX: 开始新帧 */
            gx_cnt = 0;
            gx_rx[gx_cnt++] = d;
        } else if (gx_cnt > 0) {
            gx_rx[gx_cnt++] = d;
            if (d == 0x03 && gx_cnt >= 8) {
                /* ETX 后应有2字节校验, 检查是否收完 */
                /* 继续收2字节 */
            }
            if (gx_cnt > 6 && gx_rx[gx_cnt - 4] == 0x03) {
                /* ETX 在 -4 位置, 后面有2字节校验已收完 */
                gx_frame_ready = 1;
            }
            if (gx_cnt >= 250) {
                gx_cnt = 0;  /* 溢出保护 */
            }
        }
    }
}
