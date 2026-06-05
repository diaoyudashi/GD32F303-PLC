#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define ADC_NUM_CHANNELS    6
#define ADC_BUF_SIZE        6

/* Channel order in DMA buffer */
#define ADC_CH_CURRENT_U    0  /* PC0: ADC_IN10 */
#define ADC_CH_CURRENT_V    1  /* PC1: ADC_IN11 */
#define ADC_CH_CURRENT_W    2  /* PC2: ADC_IN12 */
#define ADC_CH_PHOTO1       3  /* PB0: ADC_IN8  */
#define ADC_CH_PHOTO2       4  /* PA0: ADC_IN0  */
#define ADC_CH_HALL_TRIG    5  /* PB1: ADC_IN9 — linear Hall for motor trigger */

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern volatile uint32_t adc_dma_buffer[ADC_BUF_SIZE];
extern volatile uint8_t  adc_dma_complete;

void BSP_ADC1_Init(void);
void BSP_ADC1_Start_DMA(void);
uint32_t BSP_ADC_GetChannel(uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ADC_H */
