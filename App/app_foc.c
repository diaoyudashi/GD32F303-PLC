#include "app_foc.h"
#include "bsp_tim.h"
#include "bsp_gpio.h"
#include "bsp_adc.h"
#include "app_system.h"
#include <math.h>

static uint8_t on=0;
static const uint16_t hall_angle[8]={0,54613,32768,43691,10923,0,21845,0};

static uint8_t read_hall(void){
    uint8_t h=0;
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6))h|=0x04;
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7))h|=0x01;
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))h|=0x02;
    return h&0x07;
}

void FOC_Init(void){on=0;}

void FOC_Run(void){
    if(!on){
        GPIO_InitTypeDef g={0};g.Mode=GPIO_MODE_AF_PP;g.Speed=GPIO_SPEED_FREQ_HIGH;
        g.Pin=LS_U_PIN;HAL_GPIO_Init(LS_U_PORT,&g);
        g.Pin=LS_V_PIN;HAL_GPIO_Init(LS_V_PORT,&g);
        g.Pin=LS_W_PIN;HAL_GPIO_Init(LS_W_PORT,&g);
        BSP_TIM1_PWM_StartFOC();on=1;
    }
    uint8_t h=read_hall();
    static uint8_t last_h=0xFF;
    static uint32_t last_t=0;
    static int32_t angle=0;
    uint32_t now=APP_System_GetTick();
    if(h!=last_h && last_h!=0xFF){
        uint32_t dt=now-last_t;
        if(dt>5&&dt<500){
            int32_t hp=(int32_t)hall_angle[h],err=hp-angle;
            if(err>32768)err-=65536; if(err<-32768)err+=65536;
            angle+=err/8;last_t=now;
        }
    }else if(last_h==0xFF){angle=(int32_t)hall_angle[h];last_t=now;}
    last_h=h;
    angle+=200; if(angle>65535)angle-=65536; if(angle<0)angle+=65536;
    uint32_t th=(uint32_t)(angle+16384)&0xFFFF;
    float w=(float)th*9.58738e-5f,p2=2.0943951f;

    uint16_t Im=BSP_ADC_GetChannel(0),v;
    v=BSP_ADC_GetChannel(1);if(v>Im)Im=v;v=BSP_ADC_GetChannel(2);if(v>Im)Im=v;
    static float i_int=0;
    float err=500.0f-(float)Im,A_raw=err*0.0002f+i_int;
    float A=(A_raw<0)?0:((A_raw>0.3f)?0.3f:A_raw);
    if(A==A_raw){i_int+=err*0.00005f;if(i_int<0)i_int=0;if(i_int>0.3f)i_int=0.3f;}

    uint16_t pr=htim1.Init.Period;
    htim1.Instance->CCR1=(uint16_t)((0.5f+0.5f*A*sinf(w))*pr);
    htim1.Instance->CCR2=(uint16_t)((0.5f+0.5f*A*sinf(w+p2))*pr);
    htim1.Instance->CCR3=(uint16_t)((0.5f+0.5f*A*sinf(w-p2))*pr);
}

void FOC_Stop(void){on=0;BSP_TIM1_PWM_Stop();}
