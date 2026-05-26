#include "bsp_can.h"
#include "app_config.h"

CAN_HandleTypeDef hcan1 = {0};

/* RX buffer */
#define CAN_RX_BUF_SIZE 32
static struct {
    uint32_t id;
    uint8_t  data[8];
    uint8_t  len;
} can_rx_buf[CAN_RX_BUF_SIZE];
static volatile uint8_t can_rx_head = 0;
static volatile uint8_t can_rx_tail = 0;

void BSP_CAN1_Init(void)
{
    CAN_FilterTypeDef filter = {0};
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* CAN1 remap to PA11(RX)/PA12(TX) */
    __HAL_AFIO_REMAP_CAN1_2();

    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    gpio.Pin = GPIO_PIN_11;  /* RX */
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Pin = GPIO_PIN_12;  /* TX */
    HAL_GPIO_Init(GPIOA, &gpio);

    hcan1.Instance = CAN1;

    /* APB1 = 36MHz → 500kbps: prescaler = 6, tq = 1/(36M/6) = 166.7ns
     * 1 bit = (1+5+2) * tq = 8 * 166.7ns = 1.33μs → 750kbps...
     * Let's recalc: target 500k → bit time = 2μs
     * prescaler=6 → tq=166.7ns, need 12 tq → BS1=7, BS2=4 → (1+7+4)=12*166.7=2μs */
    hcan1.Init.Prescaler = 6;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_7TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;
    HAL_CAN_Init(&hcan1);

    /* Filter: accept all IDs on FIFO0 */
    filter.FilterIdHigh = 0x0000;
    filter.FilterIdLow = 0x0000;
    filter.FilterMaskIdHigh = 0x0000;
    filter.FilterMaskIdLow = 0x0000;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterBank = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan1, &filter);

    /* Activate FIFO0 message pending interrupt */
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, NVIC_PRIO_CAN_RX0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

    HAL_CAN_Start(&hcan1);
}

void BSP_CAN1_Send(uint32_t id, uint8_t *data, uint8_t len)
{
    CAN_TxHeaderTypeDef header = {0};
    uint32_t mailbox;

    if (len > 8) len = 8;

    header.StdId = id & 0x7FF;
    header.ExtId = 0;
    header.IDE = CAN_ID_STD;
    header.RTR = CAN_RTR_DATA;
    header.DLC = len;
    header.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage(&hcan1, &header, data, &mailbox);
}

uint8_t BSP_CAN1_Receive(uint32_t *id, uint8_t *data, uint8_t *len)
{
    if (can_rx_head == can_rx_tail) return 0;

    *id = can_rx_buf[can_rx_tail].id;
    for (uint8_t i = 0; i < can_rx_buf[can_rx_tail].len; i++) {
        data[i] = can_rx_buf[can_rx_tail].data[i];
    }
    *len = can_rx_buf[can_rx_tail].len;
    can_rx_tail = (can_rx_tail + 1) % CAN_RX_BUF_SIZE;
    return 1;
}

uint8_t BSP_CAN1_MessagePending(void)
{
    return can_rx_head != can_rx_tail;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef header;
    uint8_t next = (can_rx_head + 1) % CAN_RX_BUF_SIZE;

    if (next != can_rx_tail) {
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, can_rx_buf[can_rx_head].data);
        can_rx_buf[can_rx_head].id = header.StdId;
        can_rx_buf[can_rx_head].len = header.DLC;
        can_rx_head = next;
    }
}
