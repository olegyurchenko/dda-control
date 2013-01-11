#include <stm32f10x.h>
#include <sys_timer.h>
#include <dda_uart.h>
#include <sys_adc.h>
#include <dda_motors.h>
#include <sys_sheduler.h>
#include <dda_lcd.h>
#include <dda_key.h>
#include <dda_sensor.h>
#include <dda_mode.h>
#include <event.h>
#include <dda_protocol.h>

#ifdef USE_CONSOLE
#include <console.h>
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
static void init();
/*----------------------------------------------------------------------------*/
int main()
{
  init();
  lcd_put_line(0, "Test", SCR_ALIGN_CENTER);
  lcd_add_scroll_text(1, 0, 16, "DDA is part of a software-hardware system");
  lcd_put_line(1, "1234567890123456", SCR_ALIGN_LEFT);
  lcd_update();

  //!!!!!!!!!!!!!!!!!!!!!!
  mode_init(); //STart of work
  //!!!!!!!!!!!!!!!!!!!!!!


  while(1)
	{
#ifdef USE_CONSOLE
    console_handler();
#else //USE_CONSOLE
    protocol_handler();
#endif //USE_CONSOLE
    sheduler_handler();
    process_events();
  }
}
/*----------------------------------------------------------------------------*/
static void init()
{
  SystemInit();
  SystemCoreClockUpdate();
  
  /* Configure the Priority Group to 4 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  sys_timer_init();
  sheduler_init();
  uart_init();
#ifdef USE_CONSOLE
  console_init();
#else //USE_CONSOLE
  protocol_init();
#endif //USE_CONSOLE
  sys_adc_init();
  //ledInit();
  motors_init();
  lcd_init();
  keys_init();
  sensors_init();
}
/*----------------------------------------------------------------------------*/
