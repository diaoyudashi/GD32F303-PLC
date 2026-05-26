#include "bsp_adc.h"
#include "app_config.h"

ADC_HandleTypeDef hadc1 = {0};
DMA_HandleTypeDef hdma_adc1 = {0};

volatile uint32_t adc_dma_buffer[ADC_BUF_SIZE] = {0};
volatile uint8_t  adc_dma_complete = 0;

void BSP_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef ch = {0};
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Analog inputs: PC0(IN10), PC1(IN11), PC2(IN12), PB0(IN8), PA0(IN0) */
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;

    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    HAL_GPIO_Init(GPIOC, &gpio);
    gpio.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOB, &gpio);
    gpio.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* DMA1 Channel1 for ADC1 */
    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_adc1);

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

    /* ADC1: 12-bit, scan mode, continuous, triggered by TIM1_TRGO */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;           /* trigger-driven */
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = ADC_NUM_CHANNELS;
    HAL_ADC_Init(&hadc1);

    /* Channel config: sampling order = rank, sample time = 28.5 cycles */
    ch.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;

    ch.Rank = 1; ch.Channel = ADC_CHANNEL_10;  /* PC0: I_U */
    HAL_ADC_ConfigChannel(&hadc1, &ch);
    ch.Rank = 2; ch.Channel = ADC_CHANNEL_11;  /* PC1: I_V */
    HAL_ADC_ConfigChannel(&hadc1, &ch);
    ch.Rank = 3; ch.Channel = ADC_CHANNEL_12;  /* PC2: I_W */
    HAL_ADC_ConfigChannel(&hadc1, &ch);
    ch.Rank = 4; ch.Channel = ADC_CHANNEL_8;   /* PB0: photo1 */
    HAL_ADC_ConfigChannel(&hadc1, &ch);
    ch.Rank = 5; ch.Channel = ADC_CHANNEL_0;   /* PA0: photo2 */
    HAL_ADC_ConfigChannel(&hadc1, &ch);

    /* NVIC for DMA */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, NVIC_PRIO_ADC_DMA, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void BSP_ADC1_Start_DMA(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, ADC_BUF_SIZE);
}

uint32_t BSP_ADC_GetChannel(uint8_t ch)
{
    if (ch < ADC_BUF_SIZE) {
        return adc_dma_buffer[ch];
    }
    return 0;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    adc_dma_complete = 1;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    adc_dma_complete = 1;
}
