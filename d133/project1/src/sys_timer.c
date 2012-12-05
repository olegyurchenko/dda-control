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
// Use DWT's Cycle Count Register (DWT registers are described in the ARMv7M Architecture Reference Manual)
/*----------------------------------------------------------------------------*/
static uint32_t dwt(void)
{
  return *DWT_CYCCNT;
}
/*----------------------------------------------------------------------------*/
void sys_usleep(uint32_t us)
{
  int32_t stop;

  stop = dwt() + us * (SystemCoreClock/1000000);
  while(((int32_t)dwt() - stop) < 0)
  {
  }
}
/*----------------------------------------------------------------------------*/
