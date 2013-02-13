/*----------------------------------------------------------------------------*/
/**
* @pkg dda_plunger
*/
/**
* Plunger functions.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.12.2012 12:20:06<br>
* @pkgdoc dda_plunger
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_PLUNGER_H_1355998806
#define DDA_PLUNGER_H_1355998806
/*----------------------------------------------------------------------------*/
#include <event.h>

extern handler_t plunger_handler;
#define PLUNGER_TIMEOUT_ERROR (-1)
#define PLUNGER_END_POS_ERROR (-2)


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

int is_plunger_down();
unsigned plunger_position();
void plunger_go_down();
void plunger_go_up();
void plunger_stop();
unsigned touch_position();
void set_touch_position();
void reset_plunger_position();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_PLUNGER_H_1355998806*/

