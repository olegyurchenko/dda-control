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
/*----------------------------------------------------------------------------*/
typedef struct
{
  int *value;
  int min;
  int max;
  int increment;
  int x;
  int y;
  int w;
} spin_data_t;
/*----------------------------------------------------------------------------*/
static spin_data_t spin_data;
static int spin_handler(void*, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
void spin_edit_start(int y, int x, int w, int *value, int max, int min, int increment)
{
  spin_data.value = value;
  spin_data.x = x;
  spin_data.y = y;
  spin_data.w = w;
  spin_data.min = min;
  spin_data.max = max;
  push_event_handler();
  set_event_handler(spin_handler, 0);
}
/*----------------------------------------------------------------------------*/
static int spin_handler(void *data, event_t evt, int param1, void *param2)
{
  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;
  return 0;
}
/*----------------------------------------------------------------------------*/
