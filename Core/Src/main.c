#include "gd32f30x.h"

volatile uint32_t g_tick = 0;
void SysTick_Handler(void) { g_tick++; }
static const char Ascll[] = "0123456789ABCDEF";

static void tx_byte(uint8_t d) {
    usart_data_transmit(USART0, d);
    while (usart_flag_get(USART0, USART_FLAG_TBE) == RESET);
}
static void ack(void) { tx_byte(0x06); }
static void nak(void) { tx_byte(0x15); }

static void reply_frame(uint8_t *data, uint16_t len) {
    uint8_t s = 0; tx_byte(0x02);
    for (uint16_t i = 0; i < len; i++) { tx_byte(data[i]); s += data[i]; }
    tx_byte(0x03); s += 0x03;
    tx_byte(Ascll[(s>>4)&0xF]); tx_byte(Ascll[s&0xF]);
}

int main(void) {
    SysTick_Config(SystemCoreClock/1000);
    rcu_periph_clock_enable(RCU_GPIOF);
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9|GPIO_PIN_10);
    gpio_bit_set(GPIOF, GPIO_PIN_9|GPIO_PIN_10);
    rcu_periph_clock_enable(RCU_GPIOA); rcu_periph_clock_enable(RCU_USART0); rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    usart_deinit(USART0); usart_baudrate_set(USART0,19200);
    usart_word_length_set(USART0,USART_WL_8BIT); usart_stop_bit_set(USART0,USART_STB_1BIT);
    usart_parity_config(USART0,USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
    usart_receive_config(USART0,USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
    usart_enable(USART0);

    uint8_t rx[256]; int16_t n=0;
    uint32_t lb=0; uint8_t led=1;

    while(1) {
        while(usart_flag_get(USART0,USART_FLAG_RBNE)==SET) {
            uint8_t d=usart_data_receive(USART0)&0x7F;
            if(d==0x05){ack();}
            else if(d==0x02){n=1;rx[0]=0x02;}
            else if(n>0){if(n<250){rx[n]=d;n++;}
                if(n>=5&&rx[n-3]==0x03){
                    gpio_bit_reset(GPIOF,GPIO_PIN_9);
                    uint8_t cmd = rx[1]; /* 命令在 rx[1] (对齐参考rx_data[3]) */

                    switch(cmd) {
                    case '0': /* 0x30 读字元件 -> 回 PLC 型号 */
                        { uint16_t v=0x5EF6; uint8_t lo=v&0xFF,hi=(v>>8)&0xFF;
                          uint8_t r[4]={Ascll[(lo>>4)&0xF],Ascll[lo&0xF],Ascll[(hi>>4)&0xF],Ascll[hi&0xF]};
                          reply_frame(r,4); } break;
                    case '1': /* 0x31 写字元件 */ ack(); break;
                    case '4': /* 0x34 find_end */ ack(); break;
                    case '7': /* 0x37 强制ON */ ack(); break;
                    case '8': /* 0x38 强制OFF */ ack(); break;
                    case 'B': /* 0x42 锁定 */ ack(); break;
                    case 'E': /* 0x45 扩展指令 -> 回型号 */ { uint16_t v=0x5EF6; uint8_t lo=v&0xFF,hi=(v>>8)&0xFF; uint8_t r[4]={Ascll[(lo>>4)&0xF],Ascll[lo&0xF],Ascll[(hi>>4)&0xF],Ascll[hi&0xF]}; reply_frame(r,4); } break;
                    default:  ack(); break;
                    }
                    n=0;
                }
            }
        }
        if((g_tick-lb)>=500){lb=g_tick;
            if(led){gpio_bit_reset(GPIOF,GPIO_PIN_10);led=0;}
            else{gpio_bit_set(GPIOF,GPIO_PIN_10);led=1;}
        }
    }
}
