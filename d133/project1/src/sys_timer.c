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
//#define DWT_CYCCNT    *(volatile uint32_t *)0xE0001004
//#define DWT_CONTROL   *(volatile uint32_t *)0xE0001000
//#define SCB_DEMCR     *(volatile uint32_t *)0xE000EDFC
/*----------------------------------------------------------------------------*/
volatile uint32_t *DWT_CYCCNT = (volatile uint32_t *)0xE0001004; //address of the register
volatile uint32_t *DWT_CONTROL = (volatile uint32_t *)0xE0001000; //address of the register
volatile uint32_t *SCB_DEMCR = (volatile uint32_t *)0xE000EDFC; //address of the register
/*----------------------------------------------------------------------------*/
static volatile uint32_t counter = 0;
/*----------------------------------------------------------------------------*/
void timeout_set(timeout_t *dst, uint32_t timeout, uint32_t t)
{
  dst->start = t;
  dst->timeout = t + timeout;
}
/*----------------------------------------------------------------------------*/
int timeout_riched(timeout_t *tm, uint32_t t)
{
  if(tm->start <= tm->timeout)
  {
    if(tm->timeout < t || tm->start > t)
      return 1;
  }
  else
  {
    if(tm->timeout < t && tm->start > t)
      return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
/**Init sys timer*/
void sys_timer_init()
{
  SysTick_Config(SystemCoreClock / 1000);

  // DWT's Cycle Count Register (0xE0001004) for timing measurement

  //Enable TRCENA bit in DEMCR (0xE000EDFC, bit 24) before using DWT
  *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
  *DWT_CYCCNT = 0; // reset the counter
  *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
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
  timeout_t tm;
  timeout_set(&tm, ms, sys_tick_count());
  while(!timeout_riched(&tm, sys_tick_count()))
  {
  }
}
/*----------------------------------------------------------------------------*/
void SysTick_Handler()
{
  counter ++;
}
/*----------------------------------------------------------------------------*/
// Use DWT's Cycle Count Register (DWT registers are described in the ARMv7M Architecture Reference Manual)
/*----------------------------------------------------------------------------*/
static uint32_t dwt(void)
{
  return *DWT_CYCCNT;
}
/*----------------------------------------------------------------------------*/
static void _usleep(uint32_t us)
{
  timeout_t tm;
  timeout_set(&tm, us * (SystemCoreClock/1000000), dwt());
  while(!timeout_riched(&tm, dwt()))
  {
  }
}
/*----------------------------------------------------------------------------*/
void sys_usleep(uint32_t us)
{
  uint32_t ms;
  ms = us / 1000;
  if(ms)
    sys_sleep(ms);
  us -= ms * 1000;
  if(us)
    _usleep(us);
}
