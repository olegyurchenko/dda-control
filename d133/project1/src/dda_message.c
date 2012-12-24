/*----------------------------------------------------------------------------*/
/**
* @pkg dda_message
*/
/**
* Output message to LCD and wait.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 24.12.2012 10:24:55<br>
* @pkgdoc dda_message
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_message.h"
#include <event.h>
#include <dda_lcd.h>
#include <sys_timer.h>
#include <dda_clib.h>
/*----------------------------------------------------------------------------*/
static int msg_handler(void *data, event_t evt, int param1, void *param2);
static timeout_t timeout;
static int tm_defined;
/*----------------------------------------------------------------------------*/
void show_message(const char *caption, const char *message, int time /*=0*/)
{
  if(time)
  {
    timeout_set(&timeout, time, sys_tick_count());
    tm_defined = 1;
  }
  else
    tm_defined = 0;

  push_event_handler();
  set_event_handler(msg_handler, 0);

  lcd_clear();
  if(strlen(caption) > lcd_width())
    lcd_add_scroll_text(0, 0, lcd_width(), caption);
  else
    lcd_put_line(0, caption, SCR_ALIGN_CENTER);

  if(strlen(message) > lcd_width())
    lcd_add_scroll_text(1, 0, lcd_width(), message);
  else
    lcd_put_line(1, message, SCR_ALIGN_CENTER);

}
/*----------------------------------------------------------------------------*/
static int msg_handler(void *data, event_t evt, int param1, void *param2)
{
  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;
  switch(evt)
  {
  case KEY_PRESS_EVENT:
    pop_event_handler();
    break;
  default:
    if(tm_defined && timeout_riched(&timeout, sys_tick_count()))
      pop_event_handler();
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
