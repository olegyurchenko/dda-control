/*----------------------------------------------------------------------------*/
/**
* @pkg spin_edit
*/
/**
* Spin edit. Enable input integer values.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 24.12.2012 12:29:29<br>
* @pkgdoc spin_edit
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "spin_edit.h"
#include <event.h>
#include <dda_lcd.h>
#include <dda_key.h>
#include <dda_clib.h>
#include <sys_timer.h>
#include <dda_work_mode.h>
/*----------------------------------------------------------------------------*/
#ifndef ABS
#define ABS(a)              (((a) < 0) ? (- (a)) : (a))
#endif
/*----------------------------------------------------------------------------*/
#define UP_ARROW_CHR 0xd9
#define DOWN_ARROW_CHR 0xda
#define BORDER_CHR 0xff
/*----------------------------------------------------------------------------*/
#define CURSOR_TIME 600
#define KEY_DELAY_TIME 800
#define KEY_REPEAT_TIME 300
/*----------------------------------------------------------------------------*/
typedef struct
{
  int *value;
  int min;
  int max;
  int increment;
  int size;
  const char *caption;
  int cursor_active;
  timeout_t timeout;
  int active_key;
  timeout_t key_timeout;
} spin_data_t;
/*----------------------------------------------------------------------------*/
static spin_data_t spin_data;
static int spin_handler(void*, event_t evt, int param1, void *param2);
static int value = 0;
static void display();
/*----------------------------------------------------------------------------*/
void spin_edit_start(const char *caption, int *val, int max, int min, int increment)
{
  int size;
  char buffer[16];

  spin_data.value = val;
  spin_data.caption = caption;
  spin_data.min = min;
  spin_data.max = max;
  spin_data.increment = increment;

  spin_data.size = sprintf(buffer, "%d", min);
  size = sprintf(buffer, "%d", max);
  if(spin_data.size < size)
    spin_data.size = size;

  value = *val;
  if(value < min)
    value = min;
  if(value > max)
    value = max;

  spin_data.cursor_active = 1;
  timeout_set(&spin_data.timeout, CURSOR_TIME, sys_tick_count());
  push_event_handler();
  set_event_handler(spin_handler, 0);
}
/*----------------------------------------------------------------------------*/
static void draw_cursor()
{
  int width, x;
  width = lcd_width();
  x = (width - spin_data.size) / 2 - 1;
  lcd_put_char(x, 1, spin_data.cursor_active ? UP_ARROW_CHR : ' ');
  x += spin_data.size + 1;
  lcd_put_char(x, 1, spin_data.cursor_active ? DOWN_ARROW_CHR : ' ');

}
/*----------------------------------------------------------------------------*/
static void display()
{
  int width, x;
  char buffer[16], fmt[16], *p;
  width = lcd_width();
  lcd_clear();
  if(strlen(spin_data.caption) > width)
    lcd_add_scroll_text(0, 0, width, spin_data.caption);
  else
    lcd_put_line(0, spin_data.caption, SCR_ALIGN_CENTER);

  snprintf(fmt, sizeof(fmt), "%%-%dd", spin_data.size);
  snprintf(buffer, sizeof(buffer), fmt, value);
  x = (width - spin_data.size) / 2;
  p = buffer;
  while(*p && x < width)
  {
    lcd_put_char(x, 1, *p);
    x ++; p ++;
  }
  draw_cursor();
}
/*----------------------------------------------------------------------------*/
static int increment_value()
{
  if(value <= spin_data.max - spin_data.increment)
  {
    value += spin_data.increment;
    display();
  }
  return value <= spin_data.max - spin_data.increment;
}
/*----------------------------------------------------------------------------*/
static int decrement_value()
{
  if(value >= spin_data.min + spin_data.increment)
  {
    value -= spin_data.increment;
    display();
  }
  return value >= spin_data.min + spin_data.increment;
}
/*----------------------------------------------------------------------------*/
static int spin_handler(void *data, event_t evt, int param1, void *param2)
{
  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;
  switch(evt)
  {
  case IDLE_EVENT:
    if(timeout_riched(&spin_data.timeout, sys_tick_count()))
    {
      timeout_set(&spin_data.timeout, CURSOR_TIME, sys_tick_count());
      spin_data.cursor_active = !spin_data.cursor_active;
      draw_cursor();
    }

    if(spin_data.active_key == KEY_UP && timeout_riched(&spin_data.key_timeout, sys_tick_count()))
    {
      if(!increment_value())
        spin_data.active_key = 0;
      else
        timeout_set(&spin_data.key_timeout, KEY_REPEAT_TIME, sys_tick_count());
    }

    if(spin_data.active_key == KEY_DOWN && timeout_riched(&spin_data.key_timeout, sys_tick_count()))
    {
      if(!decrement_value())
        spin_data.active_key = 0;
      else
        timeout_set(&spin_data.key_timeout, KEY_REPEAT_TIME, sys_tick_count());
    }
    break;

  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    display();
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_UP)
    {
      if(increment_value())
      {
        spin_data.active_key = KEY_UP;
        timeout_set(&spin_data.key_timeout, KEY_DELAY_TIME, sys_tick_count());
      }
    }

    if(param1 == KEY_DOWN)
    {
      if(decrement_value())
      {
        spin_data.active_key = KEY_DOWN;
        timeout_set(&spin_data.key_timeout, KEY_DELAY_TIME, sys_tick_count());
      }
    }

    if(param1 == KEY_STOP || param1 == KEY_MENU)
    {
      start_work_menu();
    }

    if(param1 == KEY_OK)
    {
      *spin_data.value = value;
      pop_event_handler();
    }
    break;

  case KEY_RELEASE_EVENT:
    spin_data.active_key = 0;
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
