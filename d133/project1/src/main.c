#include <stm32f10x.h>
//#include <system_stm32f10x.h>
#include <sys_timer.h>
#include <dda_uart.h>
#include <console.h>
#include <sys_adc.h>
#include <dda_motors.h>
/*----------------------------------------------------------------------------*/
static void init();
//static void ledInit();
//static void ledOn(int on);
/*----------------------------------------------------------------------------*/
int main()
{
  int led = 0, sz;
  uint32_t t;

  init();
  t = sys_tick_count() + 500;
  while(1)
	{
    if(t < sys_tick_count())
    {
      //const char buf[] = "12";
      led = !led;
      //ledOn(led);
      t = sys_tick_count() + 500;
      //sys_uart_write(0, &buf[0], 1);
      //sys_uart_write(1, &buf[1], 1);

      //USART_SendData(USART1, 0x31);
      //USART_SendData(USART2, 0x32);
      //USART_SendData(USART3, 0x33);
    }

    console_handler();
  }
}
/*----------------------------------------------------------------------------*/
static void init()
{
  SystemInit();
  SystemCoreClockUpdate();
  
  sys_timer_init();
  uart_init();
  console_init(1);
  //sys_dac_init();
  sys_adc_init();
  //audio_init();
  //ledInit();
  motors_init();
}
/*----------------------------------------------------------------------------*/
#if 0
static void ledOn(int on)
{
  if(on)
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
  else
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
}
/*----------------------------------------------------------------------------*/
static void ledInit()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitTypeDef gpio;

  gpio.GPIO_Pin = GPIO_Pin_9;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &gpio);
}
/*----------------------------------------------------------------------------*/
#endif
