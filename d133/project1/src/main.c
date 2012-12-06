#include <stm32f10x.h>
#include <sys_timer.h>
#include <dda_uart.h>
#include <sys_adc.h>
#include <dda_motors.h>
#include <sys_sheduler.h>
#include <dda_lcd.h>

#define USE_CONSOLE //!!!!!!!!!!!


#ifdef USE_CONSOLE
#include <console.h>
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
static void init();
/*----------------------------------------------------------------------------*/
int main()
{
  init();
  while(1)
	{
#ifdef USE_CONSOLE
    console_handler();
#endif //USE_CONSOLE
    sheduler_handler();
  }
}
/*----------------------------------------------------------------------------*/
static void init()
{
  SystemInit();
  SystemCoreClockUpdate();
  
  sys_timer_init();
  sheduler_init();
  uart_init();
#ifdef USE_CONSOLE
  console_init();
#endif //USE_CONSOLE
  sys_adc_init();
  //ledInit();
  motors_init();
  lcd_init();
}
/*----------------------------------------------------------------------------*/
