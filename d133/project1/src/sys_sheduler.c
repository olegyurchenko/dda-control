/*----------------------------------------------------------------------------*/
/**
* @pkg sys_sheduler
*/
/**
* Short description of sys_sheduler.
*
* Long description of sys_sheduler.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 06.12.2012 10:37:50<br>
* @pkgdoc sys_sheduler
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "sys_sheduler.h"
#include <sys_timer.h>
//#define DEBUG

#ifdef DEBUG
#include <console.h>
#endif
/*----------------------------------------------------------------------------*/
typedef struct
{
  sh_callback cb;
  void *data;
  timeout_t tm;
  uint32_t period;
} SHEDULER_DATA;
/*----------------------------------------------------------------------------*/
#define MAX_JOB_COUNT 16
/*----------------------------------------------------------------------------*/
static SHEDULER_DATA sheduler_data[MAX_JOB_COUNT];
#ifdef DEBUG
static void debug_handler(void *);
#endif
/*----------------------------------------------------------------------------*/
void sheduler_init()
{
  int i;
  for(i = 0; i < MAX_JOB_COUNT; i++)
  {
    sheduler_data[i].cb = 0;
    sheduler_data[i].data = 0;
    sheduler_data[i].period = 0;
  }
#ifdef DEBUG
  sheduler_add(debug_handler, 0, 100, 1000);
#endif
}
/*----------------------------------------------------------------------------*/
int sheduler_add(sh_callback cb, void *user_data, uint32_t delay, uint32_t period)
{
  int i;
  for(i = 0; i < MAX_JOB_COUNT; i++)
  {
    if(sheduler_data[i].cb == 0)
    {
      sheduler_data[i].cb = cb;
      sheduler_data[i].data = user_data;
      timeout_set(&sheduler_data[i].tm, delay, sys_tick_count());
      sheduler_data[i].period = period;
      return i;
    }
  }
  return -1;
}
/*----------------------------------------------------------------------------*/
void sheduler_remove(int i)
{
  if(i >= 0 && i < MAX_JOB_COUNT)
    sheduler_data[i].cb = 0;
}
/*----------------------------------------------------------------------------*/
void sheduler_handler()
{
  int i;
  for(i = 0; i < MAX_JOB_COUNT; i++)
  {
    if(sheduler_data[i].cb != 0 && timeout_riched(&sheduler_data[i].tm, sys_tick_count()))
    {
      sheduler_data[i].cb(sheduler_data[i].data);
      if(sheduler_data[i].period)
        timeout_set(&sheduler_data[i].tm, sheduler_data[i].period, sys_tick_count());
      else
        sheduler_data[i].cb = 0;
    }
  }
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
static void debug_handler(void *data)
{
  static unsigned t;
  volatile int64_t test = 0;
  int i;
  (void) data;
  console_printf("Sheduler: %u\r\n", sys_tick_count() - t);
  for(int i = 0; i < 64; i++)
    test /= 10;
  t = sys_tick_count();
  console_printf("Calc: %u\r\n", sys_tick_count() - t);
  t = sys_tick_count();
}

#endif
