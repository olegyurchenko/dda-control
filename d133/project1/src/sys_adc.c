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
/*----------------------------------------------------------------------------*/
#define USE_CONSOLE //!!!!
#ifdef USE_CONSOLE
#include <console.h>
#include <sys_timer.h>

static CONSOLE_CMD adc_console1, adc_console2;
static int adc_cmd1(int argc, char **argv);
static int adc_state_handler();
static int adc_cmd2(int argc, char **argv);
static struct
{
  int pos;
  int samles;
  uint8_t sample_time;
  uint32_t avg;
  uint16_t min;
  uint16_t max;
  uint32_t start;
} adc_data;
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
void sys_adc_init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  ADC_InitTypeDef  ADC_InitStructure;

  RCC_ADCCLKConfig(RCC_PCLK2_Div2);
  //RCC_ADCCLKConfig(RCC_PCLK2_Div8);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  ADC_Cmd(ADC1, DISABLE);
  ADC_DeInit(ADC1);

  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;

  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_Cmd(ADC1, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  sys_sleep(2);
  ADC_ResetCalibration(ADC1);
  while (ADC_GetResetCalibrationStatus(ADC1)) { };
  ADC_StartCalibration(ADC1);
  while (ADC_GetCalibrationStatus(ADC1)) { };

#ifdef USE_CONSOLE
  console_cmd_init(&adc_console1);
  adc_console1.cmd = "adc";
  adc_console1.help = "adc <samples> - ADC conversion <samples> times";
  adc_console1.handler = adc_cmd1;
  adc_console1.state_handler = adc_state_handler;
  console_add_cmd(&adc_console1);

  adc_data.sample_time = ADC_SampleTime_13Cycles5;

  console_cmd_init(&adc_console2);
  adc_console2.cmd = "adt";
  adc_console2.help = "adt [<ADC sample time>] - Set ADC samle time [0-7]";
  adc_console2.handler = adc_cmd2;
  console_add_cmd(&adc_console2);
#endif //USE_CONSOLE
}
/*----------------------------------------------------------------------------*/
void sys_adc_start_conversion()
{
  ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, adc_data.sample_time /*ADC_SampleTime_13Cycles5*/);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
/*----------------------------------------------------------------------------*/
int sys_adc_get_value(int *dst)
{
  if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
    return 0;
  *dst = ADC_GetConversionValue(ADC1);
  ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
  return 1;
}
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
static int adc_cmd1(int argc, char **argv)
{
  adc_data.pos = 0;
  adc_data.samles = 1;
  adc_data.avg = 0;
  adc_data.max = 0;
  adc_data.min = 0xffff;
  if(argc > 1)
    adc_data.samles = str2int(argv[1]);
  sys_adc_start_conversion();
  adc_data.start = sys_tick_count();
  return adc_data.samles > 0 ? 0 : -1;
}
/*----------------------------------------------------------------------------*/
static int adc_state_handler()
{
  int value = 0;
  if(sys_adc_get_value(&value))
  {
    //console_printf("\r\n%4d (0x%03x)", value, value);
    adc_data.avg += value;
    adc_data.pos ++;
    if(adc_data.min > value)
      adc_data.min = value;
    if(adc_data.max < value)
      adc_data.max = value;
    if(adc_data.pos >= adc_data.samles)
    {
      adc_data.avg /= adc_data.samles;
      console_printf("\r\nAvg:%4d (0x%03x) Time:%dms\r\n",adc_data.avg, adc_data.avg, sys_tick_count() - adc_data.start);
      console_printf("Min:%4d (0x%03x) Max:%4d (0x%03x)\r\n",adc_data.min, adc_data.min, adc_data.max, adc_data.max);
      adc_data.avg = (adc_data.max + adc_data.min) / 2;
      console_printf("Center:%4d (0x%03x)",adc_data.avg, adc_data.avg);
      return -1;
    }
    sys_adc_start_conversion();
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static int adc_cmd2(int argc, char **argv)
{
  if(argc > 1)
    adc_data.sample_time = 0x7 & str2int(argv[1]);
  console_printf("\r\nADC_sampling_time = %d", adc_data.sample_time);
  return 0;
}
/*----------------------------------------------------------------------------*/
#endif //USE_CONSOLE
