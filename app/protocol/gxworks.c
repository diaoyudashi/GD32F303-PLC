/**
 * @file    gxworks.c
 * @brief   GX Works MELSEC 协议 — 参考 PLC_Com.c 格式
 */

#include "main.h"
#include "bsp_uart.h"

static const char Ascll[] = "0123456789ABCDEF";
static uint8_t  gx_rx[256];
static int16_t  gx_cnt = -1, gx_etx = -1;

void GXWorks_Poll(void)
{
    uint8_t d;
    while (BSP_USART_GetRxData(COM_PLC, &d, 1) > 0) {

        if (d == 0x02) {
            gx_cnt = 1; gx_etx = -1; gx_rx[0] = 0x02;
        } else if (gx_cnt > 0) {
            gx_rx[gx_cnt++] = d;
            if (d == 0x03) gx_etx = gx_cnt - 1;

            if (gx_etx > 0 && gx_cnt >= gx_etx + 3) {
                /* 校验和 */
                uint8_t sum = 0;
                for (int i = 1; i < gx_etx; i++) sum += gx_rx[i];
                uint8_t fcs = 0;
                { char a=gx_rx[gx_etx+1],b=gx_rx[gx_etx+2];
                  if(a>='0'&&a<='9')fcs|=(a-'0')<<4; else if(a>='A'&&a<='F')fcs|=(a-'A'+10)<<4;
                  if(b>='0'&&b<='9')fcs|=(b-'0'); else if(b>='A'&&b<='F')fcs|=(b-'A'+10); }

                if (sum == fcs) {
                    /* 命令处理 */
                    uint8_t cmdH = gx_rx[2], cmdL = gx_rx[3];
                    uint8_t tx[512]; int ti = 0; uint8_t tsum = 0;

                    if (cmdH == '3' && cmdL == '0') {
                        /* 读字元件 — 返回 4 字节 ASCII hex */
                        uint16_t val = 0x5EF6; /* FX2N(C) */
                        tx[ti++] = 0x02; /* STX */
                        for (int i = 3; i >= 0; i--) {
                            tx[ti] = Ascll[(val >> (i*4)) & 0xF]; tsum += tx[ti]; ti++;
                        }
                    } else {
                        /* 未知命令 — 回 ACK 简单帧 */
                        tx[ti++] = 0x02;
                    }
                    tx[ti++] = 0x03; tsum += 0x03;
                    tx[ti++] = Ascll[(tsum>>4)&0xF];
                    tx[ti++] = Ascll[tsum&0xF];
                    BSP_USART_SendBuf(COM_PLC, tx, ti);
                }
                gx_cnt = -1; gx_etx = -1;
            }
            if (gx_cnt >= 250) { gx_cnt = -1; }
        }
    }
}
