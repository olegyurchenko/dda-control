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
#ifndef SYS_TIMER_H_1345709875
#define SYS_TIMER_H_1345709875
/*----------------------------------------------------------------------------*/
#include <stdint.h>

typedef struct
{
  uint32_t timeout;
  uint32_t start;
} timeout_t;
/*-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**Init sys timer*/
void sys_timer_init();

/**Get 1ms tick counter*/
uint32_t sys_tick_count();

/**ms sleep*/
void sys_sleep(uint32_t ms);
/**us sleep*/
void sys_usleep(uint32_t us);

/**Timeout handle routines*/
void timeout_set(timeout_t *dst, uint32_t timeout, uint32_t time);
int timeout_riched(timeout_t *tm, uint32_t time);


#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*SYS_TIMER_H_1345709875*/

