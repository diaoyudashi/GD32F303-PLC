/**
 * @file    gxworks.c
 * @brief   GX Works MELSEC FX 协议帧处理 — 移植自FX2N量产源码
 */
#include "main.h"

/* ASCII 表 */
static const uint8_t Ascll[] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
                                0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46};

/* 帧缓冲 */
static uint8_t  rx[256];
static uint16_t rx_cnt = 0;
static uint8_t  tx[280];
static uint16_t tx_cnt = 0;

/* PLC 型号 (D8001 = 0x5EF6 = FX2N) */
static const uint16_t special_d[256] = {
    0x5EF6,  /* [0] D8000 */
    0x5EF6,  /* [1] D8001 — PLC型号 */
};

/* 发送一帧: STX + data[] + ETX + checksum */
static void gx_send(uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    tx_cnt = 0;
    tx[tx_cnt++] = 0x02;  /* STX */
    for (uint16_t i = 0; i < len; i++) {
        tx[tx_cnt++] = data[i];
        sum += data[i];
    }
    tx[tx_cnt++] = 0x03;  /* ETX */
    sum += 0x03;
    tx[tx_cnt++] = Ascll[(sum >> 4) & 0x0F];
    tx[tx_cnt++] = Ascll[sum & 0x0F];
}

/* 计算帧校验和 */
static uint8_t gx_check(uint8_t *frame, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 2; i < len - 3; i++) sum += frame[i];
    return sum;
}

/* ASCII hex 两位 -> 字节 */
static uint8_t asc2byte(uint8_t h, uint8_t l)
{
    uint8_t r = 0;
    if (h >= 'A') h -= 7;
    if (l >= 'A') l -= 7;
    r = ((h - '0') << 4) | (l - '0');
    return r;
}

/* 处理完整帧 */
static void gx_dispatch(void)
{
    if (rx_cnt < 4) return;
    /* 校验和 */
    uint8_t calc = gx_check(rx, rx_cnt);
    uint8_t fcs  = asc2byte(rx[rx_cnt - 2], rx[rx_cnt - 1]);
    if (calc != fcs) { rx_cnt = 0; return; }

    /* 解析命令 (ASCII 2字节) */
    uint8_t cmd = asc2byte(rx[2], rx[3]);

    if (cmd == 0x30) {
        /* 读字元件: 起始地址在 rx[4..7] (4字节ASCII), 长度在 rx[8..9] */
        uint16_t addr = asc2byte(rx[4], rx[5]) * 256 + asc2byte(rx[6], rx[7]);
        uint16_t n    = asc2byte(rx[8], rx[9]);

        /* 构造响应: 每个16位值 → 4字节 ASCII */
        uint8_t resp[256];
        uint16_t ri = 0;
        for (uint16_t i = 0; i < n && (addr + i) < 256; i++) {
            uint16_t v = special_d[addr + i];
            resp[ri++] = Ascll[(v >> 12) & 0xF];
            resp[ri++] = Ascll[(v >> 8) & 0xF];
            resp[ri++] = Ascll[(v >> 4) & 0xF];
            resp[ri++] = Ascll[v & 0xF];
        }
        gx_send(resp, ri);
    } else if (cmd == 0x31) {
        /* 写字元件 — 简单回ACK */
        uint8_t ack[] = {0x06};
        gx_send(ack, 1);
    } else {
        /* 未知命令 — 回 NAK */
        uint8_t nak[] = {0x15};
        gx_send(nak, 1);
    }
    rx_cnt = 0;
}

/* ---- 外部接口 ---- */
void GXWorks_SendTx(void)
{
    if (tx_cnt == 0) return;
    for (uint16_t i = 0; i < tx_cnt; i++) {
        USART_SendData(USART1, tx[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
    tx_cnt = 0;
}

void GXWorks_FeedByte(uint8_t d)
{
    if (d == 0x02) {
        /* STX — 开始新帧 */
        rx_cnt = 0;
        rx[rx_cnt++] = d;
    } else if (rx_cnt > 0) {
        rx[rx_cnt++] = d;
        /* ETX (0x03) 后等 2 字节校验 */
        if (rx_cnt >= 5 && rx[rx_cnt - 3] == 0x03) {
            gx_dispatch();
        }
        if (rx_cnt >= 250) rx_cnt = 0;
    }
}
