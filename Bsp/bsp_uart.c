#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "app_config.h"

UART_HandleTypeDef huart1 = {0};  /* RS-485 */
UART_HandleTypeDef huart2 = {0};  /* Debug */

/* Simple RX ring buffers */
#define UART_RX_BUF_SIZE 256
static volatile uint8_t uart1_rx_buf[UART_RX_BUF_SIZE];
static volatile uint16_t uart1_rx_head = 0;
static volatile uint16_t uart1_rx_tail = 0;

static volatile uint8_t uart2_rx_buf[UART_RX_BUF_SIZE];
static volatile uint16_t uart2_rx_head = 0;
static volatile uint16_t uart2_rx_tail = 0;

static uint8_t uart1_rx_byte;  /* single-byte for ISR reception */
static uint8_t uart2_rx_byte;

/* ========================== USART1: RS-485 (PB6/PB7) ======================= */
void BSP_USART1_RS485_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* USART1 remap to PB6(TX)/PB7(RX) */
    __HAL_AFIO_REMAP_USART1_ENABLE();

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio.Pin = GPIO_PIN_6;  /* TX */
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin = GPIO_PIN_7;  /* RX */
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &gpio);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = RS485_BAUDRATE;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);

    HAL_NVIC_SetPriority(USART1_IRQn, NVIC_PRIO_USART1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    HAL_UART_Receive_IT(&huart1, &uart1_rx_byte, 1);
}

void BSP_USART1_RS485_Send(uint8_t *data, uint16_t len)
{
    BSP_RS485_TX_Mode();
    HAL_UART_Transmit(&huart1, data, len, 100);
    BSP_RS485_RX_Mode();
}

uint16_t BSP_USART1_RS485_Available(void)
{
    return (uart1_rx_head - uart1_rx_tail) & (UART_RX_BUF_SIZE - 1);
}

uint8_t BSP_USART1_RS485_Read(void)
{
    uint8_t data = uart1_rx_buf[uart1_rx_tail];
    uart1_rx_tail = (uart1_rx_tail + 1) & (UART_RX_BUF_SIZE - 1);
    return data;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uint16_t next = (uart1_rx_head + 1) & (UART_RX_BUF_SIZE - 1);
        if (next != uart1_rx_tail) {
            uart1_rx_buf[uart1_rx_head] = uart1_rx_byte;
            uart1_rx_head = next;
        }
        HAL_UART_Receive_IT(&huart1, &uart1_rx_byte, 1);
    } else if (huart->Instance == USART2) {
        uint16_t next = (uart2_rx_head + 1) & (UART_RX_BUF_SIZE - 1);
        if (next != uart2_rx_tail) {
            uart2_rx_buf[uart2_rx_head] = uart2_rx_byte;
            uart2_rx_head = next;
        }
        HAL_UART_Receive_IT(&huart2, &uart2_rx_byte, 1);
    }
}

/* ========================== USART2: Debug TX-only (PA2/PA3) ================ */
void BSP_USART2_Debug_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
    /* GPIO + NVIC handled by HAL_UART_MspInit callback */
    /* USART2 is TX-only; no RX IT */
}

void BSP_USART2_Send(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(&huart2, data, len, 100);
}

