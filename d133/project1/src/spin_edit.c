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
/*----------------------------------------------------------------------------*/
#ifndef ABS
#define ABS(a)              (((a) < 0) ? (- (a)) : (a))
#endif
/*----------------------------------------------------------------------------*/
#define UP_ARROW_CHR 0xd9
#define DOWN_ARROW_CHR 0xda
#define BORDER_CHR 0xff
/*----------------------------------------------------------------------------*/
typedef struct
{
  int *value;
  int min;
  int max;
  int increment;
  int size;
  const char *caption;
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

  push_event_handler();
  display();
  set_event_handler(spin_handler, 0);
}
/*----------------------------------------------------------------------------*/
void display()
{
  int width;
  width = lcd_width();
  lcd_clear();
  if(strlen(spin_data.caption) > width)
    lcd_add_scroll_text(0, 0, width, spin_data.caption);
  else
    lcd_put_line(0, spin_data.caption, SCR_ALIGN_CENTER);
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
    break;

  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    break;


  case KEY_PRESS_EVENT:
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
