/*----------------------------------------------------------------------------*/
/**
* @pkg audio
*/
/**
* Audio functions.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 29.08.2012 13:11:38<br>
* @pkgdoc audio
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "audio.h"
#include <stm32f10x.h>
/*----------------------------------------------------------------------------*/
#define DAC_DHR12RD_Address      0x40007420
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

uint32_t DualSine12bit[32];
/*----------------------------------------------------------------------------*/
DAC_InitTypeDef            DAC_InitStructure;
DMA_InitTypeDef            DMA_InitStructure;
TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
uint32_t Idx = 0;
/*----------------------------------------------------------------------------*/
void audio_init()
{
  /* System Clocks Configuration */
  RCC_Configuration();

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
     connected to the DAC converter. In order to avoid parasitic consumption,
     the GPIO pin should be configured in analog */
  GPIO_Configuration();

  /* TIM2 Configuration */
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 32 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = HSE_VALUE / (600 * 32) ;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* DAC channel2 Configuration */
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* Fill Sine32bit table */
  for (Idx = 0; Idx < 32; Idx++)
  {
    DualSine12bit[Idx] = (Sine12bit[Idx] << 16) + (Sine12bit[Idx]);
  }

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
  /* DMA2 channel4 configuration */
  DMA_DeInit(DMA2_Channel4);
#else
  /* DMA1 channel4 configuration */
  DMA_DeInit(DMA1_Channel4);
#endif
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSine12bit;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 32;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
  DMA_Init(DMA2_Channel4, &DMA_InitStructure);
  /* Enable DMA2 Channel4 */
  DMA_Cmd(DMA2_Channel4, ENABLE);
#else
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  /* Enable DMA1 Channel4 */
  DMA_Cmd(DMA1_Channel4, ENABLE);
#endif

  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
     automatically connected to the DAC converter. */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is
     automatically connected to the DAC converter. */
  //DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Enable DMA for DAC Channel2 */
  DAC_DMACmd(DAC_Channel_2, ENABLE);
  //DAC_DMACmd(DAC_Channel_1, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);

}
/*----------------------------------------------------------------------------*/
static void RCC_Configuration(void)
{
  /* Enable peripheral clocks ------------------------------------------------*/
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
  /* DMA2 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
#else
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /*By Oleg*/
  /* GPIOC Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}
/*----------------------------------------------------------------------------*/
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
     connected to the DAC converter. In order to avoid parasitic consumption,
     the GPIO pin should be configured in analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4/* | GPIO_Pin_5*/;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*By Oleg*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//  GPIO_SetBits(GPIOC, GPIO_Pin_13);

}
/*----------------------------------------------------------------------------*/
