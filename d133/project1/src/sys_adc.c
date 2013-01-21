/*----------------------------------------------------------------------------*/
/**
* @pkg sys_adc
*/
/**
* st32f107 ADC functions.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 05.09.2012  8:53:48<br>
* @pkgdoc sys_adc
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "sys_adc.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <dda_clib.h>
#include <dda_config.h>
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
#include <console.h>
#include <sys_timer.h>

static CONSOLE_CMD adc_console1;
static int adc_cmd1(int argc, char **argv);
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
#ifndef ABS
#define ABS(a)              (((a) < 0) ? (- (a)) : (a))
#endif
/*----------------------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ADC_BUFFER_SIZE 100
static volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
/*----------------------------------------------------------------------------*/
void sys_adc_init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  ADC_InitTypeDef  ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

#define DIV RCC_PCLK2_Div2
//#define DIV RCC_PCLK2_Div4
//#define DIV RCC_PCLK2_Div6
//#define DIV RCC_PCLK2_Div8

  RCC_ADCCLKConfig(DIV);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* GPIO */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_Cmd(ADC1, DISABLE);
  ADC_DeInit(ADC1);
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel12 configuration */

#define SAMPLE_TIME ADC_SampleTime_1Cycles5
//#define SAMPLE_TIME ADC_SampleTime_7Cycles5
//#define SAMPLE_TIME ADC_SampleTime_13Cycles5
//#define SAMPLE_TIME ADC_SampleTime_28Cycles5
//#define SAMPLE_TIME ADC_SampleTime_41Cycles5
//#define SAMPLE_TIME ADC_SampleTime_55Cycles5
//#define SAMPLE_TIME ADC_SampleTime_71Cycles5
//#define SAMPLE_TIME ADC_SampleTime_239Cycles5

  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, SAMPLE_TIME);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  sys_sleep(2);
  /* Enable ADC1 reset calibration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

#ifdef USE_CONSOLE
  console_cmd_init(&adc_console1);
  adc_console1.cmd = "adc";
  adc_console1.help = "adc - ADC read ADC DMA statistic";
  adc_console1.handler = adc_cmd1;
  console_add_cmd(&adc_console1);
#endif //USE_CONSOLE
}
/*----------------------------------------------------------------------------*/
void sys_adc_start_conversion()
{
}
/*----------------------------------------------------------------------------*/
int sys_adc_get_value(int *dst)
{
  int32_t dirty_avg = 0, avg = 0, deviation, avg_deviation = 0;
  int i, count = 0;

  for(i = 0; i < ADC_BUFFER_SIZE; i++)
    dirty_avg += adc_buffer[i];
  dirty_avg /= ADC_BUFFER_SIZE;

  for(i = 0; i < ADC_BUFFER_SIZE; i++)
    avg_deviation += ABS((int32_t)adc_buffer[i] - dirty_avg);
  avg_deviation /= ADC_BUFFER_SIZE;

  for(i = 0; i < ADC_BUFFER_SIZE; i++)
  {
    deviation = ABS((int32_t)adc_buffer[i] - dirty_avg);
    if(deviation < avg_deviation)
    {
      avg += adc_buffer[i];
      count ++;
    }
  }

  if(count)
    *dst = avg / count;
  else
    *dst = dirty_avg;

  return 1;
}
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
static int adc_cmd1(int argc, char **argv)
{
  uint32_t avg = 0;
  uint16_t min = 0xffff;
  uint16_t max = 0;
  int i;

  (void) argc;
  (void) argv;

  for(i = 0; i < ADC_BUFFER_SIZE; i++)
  {
    if(min > adc_buffer[i])
      min = adc_buffer[i];
    if(max < adc_buffer[i])
      max = adc_buffer[i];
    avg += adc_buffer[i];
  }
  avg /= ADC_BUFFER_SIZE;
  console_printf("\r\nAvg:%4d (0x%03x)\r\n",avg, avg);
  console_printf("Min:%4d (0x%03x) Max:%4d (0x%03x)\r\n",min, min, max, max);
  avg = (max + min) / 2;
  console_printf("Center:%4d (0x%03x)",avg, avg);
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
#endif //USE_CONSOLE
