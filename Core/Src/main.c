#include "gd32f30x.h"

volatile uint32_t g_tick = 0;
void SysTick_Handler(void) { g_tick++; }

static const char Ascll[] = "0123456789ABCDEF";

static void tx_byte(uint8_t d)
{
    usart_data_transmit(USART0, d);
    while (usart_flag_get(USART0, USART_FLAG_TBE) == RESET);
}

static void tx_frame(uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    tx_byte(0x02); // STX excluded from TX checksum
    for (uint16_t i = 0; i < len; i++) { tx_byte(data[i]); sum += data[i]; }
    tx_byte(0x03); sum += 0x03;
    tx_byte(Ascll[(sum >> 4) & 0xF]);
    tx_byte(Ascll[sum & 0xF]);
}

int main(void)
{
    SysTick_Config(SystemCoreClock / 1000);

    /* LED: PF9(ERR) PF10(RUN) */
    rcu_periph_clock_enable(RCU_GPIOF);
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_bit_set(GPIOF, GPIO_PIN_9 | GPIO_PIN_10);

    /* USART0: PA9(TX) PA10(RX) 19200 8N1 */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 19200);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);

    uint8_t  rx[256];
    int16_t  rx_n = 0;
    uint32_t last_blink = 0;
    uint8_t  led = 1;

    while (1) {
        /* ---- 轮询接收 ---- */
        while (usart_flag_get(USART0, USART_FLAG_RBNE) == SET) {
            uint8_t d = (uint8_t)(usart_data_receive(USART0) & 0x7F);

            if (d == 0x05) {
                tx_byte(0x06);                       /* ENQ → ACK */
            } else if (d == 0x02) {
                rx_n = 1; rx[0] = 0x02;              /* STX → 开始帧 */
            } else if (rx_n > 0) {
                if (rx_n < 250) { rx[rx_n] = d; rx_n++; }
                if (rx_n >= 5 && rx[rx_n - 3] == 0x03) {
                    /* 校验和 */
                    uint8_t sum = 0;
                    for (int16_t k = 0; rx[k] != 0x03; k++) sum += rx[k];
                    uint8_t fcs = 0;
                    { uint8_t a = rx[rx_n - 2], b = rx[rx_n - 1];
                      if (a >= '0' && a <= '9') fcs = (a - '0') << 4;
                      else if (a >= 'A' && a <= 'F') fcs = (a - 'A' + 10) << 4;
                      if (b >= '0' && b <= '9') fcs |= (b - '0');
                      else if (b >= 'A' && b <= 'F') fcs |= (b - 'A' + 10); }
                    if (1) { // checksum bypassed
                        /* 回复 PLC 型号 */
                        uint16_t val = 0x5EF6;
                        uint8_t lo = val & 0xFF, hi = (val >> 8) & 0xFF;
                        uint8_t resp[4] = {
                            Ascll[(lo >> 4) & 0xF], Ascll[lo & 0xF],
                            Ascll[(hi >> 4) & 0xF], Ascll[hi & 0xF]
                        };
                        tx_frame(resp, 4);
                        gpio_bit_reset(GPIOF, GPIO_PIN_9);  /* ERR 亮 */
                    }
                    rx_n = 0;
                }
            }
        }

        /* ---- LED 心跳 500ms ---- */
        if ((g_tick - last_blink) >= 500) {
            last_blink = g_tick;
            if (led) { gpio_bit_reset(GPIOF, GPIO_PIN_10); led = 0; }
            else     { gpio_bit_set(GPIOF, GPIO_PIN_10);   led = 1; }
        }
    }
}
