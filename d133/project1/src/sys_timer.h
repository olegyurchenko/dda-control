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
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#include <stdint.h>

/**Init sys timer*/
void sys_timer_init();

/**Get 1ms tick counter*/
uint32_t sys_tick_count();

/**ms sleep*/
void sys_sleep(uint32_t ms);
/**us sleep*/
void sys_usleep(uint32_t us);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*SYS_TIMER_H_1345709875*/

