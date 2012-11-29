/*----------------------------------------------------------------------------*/
/**
* @pkg sys_timer
*/
/**
* System timer for ST32 processors.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 23.08.2012 11:17:55<br>
* @pkgdoc sys_timer
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "sys_timer.h"
#include <stm32f10x.h>
/*----------------------------------------------------------------------------*/
static volatile uint32_t counter = 0;

/**Init sys timer*/
void sys_timer_init()
{
  SysTick_Config(SystemCoreClock / 1000);
}
/*----------------------------------------------------------------------------*/
/**Get 1ms tick counter*/
uint32_t sys_tick_count()
{
  return counter;
}
/*----------------------------------------------------------------------------*/
void sys_sleep(uint32_t ms)
{
  uint32_t t1, t2;
  t1 = sys_tick_count();
  t2 = t1 + ms;
  if(t2 <= t1)
    return; //Overload
  while(t2 > sys_tick_count())
  {
  }
}
/*----------------------------------------------------------------------------*/
void SysTick_Handler()
{
  counter ++;
}
/*----------------------------------------------------------------------------*/
