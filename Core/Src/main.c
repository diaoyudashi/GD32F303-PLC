#include "main.h"

/* BSP layer */
#include "bsp_clock.h"
#include "bsp_gpio.h"
#include "bsp_tim.h"
#include "bsp_adc.h"
#include "bsp_uart.h"
#include "bsp_can.h"

/* App layer */
#include "app_system.h"
#include "app_led.h"
#include "app_motor.h"
#include "app_sensor.h"
#include "app_rs485.h"
#include "app_can.h"
#include "app_eeprom.h"

int main(void)
{
    /* ---- Hardware Init ---- */
    HAL_Init();

    BSP_Clock_Init();
    BSP_GPIO_Init();

    BSP_TIM1_PWM_Init();          /* BLDC 6-step PWM */
    BSP_TIM2_PWMDAC_Init();       /* Photo threshold DAC */
    BSP_TIM3_Encoder_Init();      /* Encoder */
    BSP_Hall_EXTI_Init();         /* Hall sensor interrupts */

    BSP_ADC1_Init();              /* Current + photo ADC */
    BSP_ADC1_Start_DMA();

    /* BSP_USART1_RS485_Init called by APP_RS485_Init */
    /* BSP_CAN1_Init called by APP_CAN_Init */

    /* ---- Application Init ---- */
    APP_System_Init();
    APP_LED_Init();
    APP_EEPROM_Init();
    APP_Sensor_Init();
    APP_RS485_Init();
    APP_CAN_Init();
    APP_Motor_Init();

    /* ---- Load parameters from EEPROM ---- */
    APP_EEPROM_Load();
    if (!APP_EEPROM_IsValid()) {
        APP_EEPROM_LoadDefaults();
        APP_EEPROM_Save();
    }

    /* ---- System Ready ---- */
    APP_System_SetState(SYS_IDLE);
    APP_LED_Set(0, LED_SLOW_BLINK);

    /* ---- Super-Loop ---- */
    while (1)
    {
        APP_System_Run();
        APP_LED_Run();
        APP_Sensor_Run();                  /* update ADC filter values */
        APP_Motor_UpdateFromSensors();     /* sensor → speed mapping */
        APP_Motor_Run();                  /* commutation + current */
        APP_RS485_Run();
        APP_CAN_Run();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    APP_LED_Set(0, LED_FAST_BLINK);
    APP_LED_Set(1, LED_FAST_BLINK);
    while (1) {
        APP_LED_Run();
    }
}
