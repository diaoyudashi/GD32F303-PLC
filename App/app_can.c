#include "app_can.h"
#include "bsp_can.h"
#include "app_config.h"

typedef struct {
    uint32_t id;
    uint8_t  data[8];
    uint8_t  len;
} CAN_Frame;

#define APP_CAN_RX_BUF_SIZE 16
static CAN_Frame can_rx_queue[APP_CAN_RX_BUF_SIZE];
static volatile uint8_t can_rx_head = 0;
static volatile uint8_t can_rx_tail = 0;

void APP_CAN_Init(void)
{
    BSP_CAN1_Init();
}

void APP_CAN_Run(void)
{
    /* Dequeue frames from BSP layer into app-level queue */
    uint32_t id;
    uint8_t data[8], len;
    while (BSP_CAN1_MessagePending()) {
        if (BSP_CAN1_Receive(&id, data, &len)) {
            uint8_t next = (can_rx_head + 1) % APP_CAN_RX_BUF_SIZE;
            if (next != can_rx_tail) {
                can_rx_queue[can_rx_head].id = id;
                for (uint8_t i = 0; i < len; i++) can_rx_queue[can_rx_head].data[i] = data[i];
                can_rx_queue[can_rx_head].len = len;
                can_rx_head = next;
            }
        }
    }
}

void APP_CAN_Send(uint32_t id, uint8_t *data, uint8_t len)
{
    BSP_CAN1_Send(id, data, len);
}

uint8_t APP_CAN_FrameReady(void)
{
    return can_rx_head != can_rx_tail;
}

void APP_CAN_GetFrame(uint32_t *id, uint8_t *data, uint8_t *len)
{
    if (can_rx_head == can_rx_tail) return;
    *id = can_rx_queue[can_rx_tail].id;
    for (uint8_t i = 0; i < can_rx_queue[can_rx_tail].len; i++) {
        data[i] = can_rx_queue[can_rx_tail].data[i];
    }
    *len = can_rx_queue[can_rx_tail].len;
    can_rx_tail = (can_rx_tail + 1) % APP_CAN_RX_BUF_SIZE;
}
