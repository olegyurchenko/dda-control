/*----------------------------------------------------------------------------*/
/**
* @pkg event
*/
/**
* Event manegement.
*
* Managemet for event driven programming.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 18.12.2012  9:38:27<br>
* @pkgdoc event
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef EVENT_H_1355816307
#define EVENT_H_1355816307
/*----------------------------------------------------------------------------*/
typedef enum
{
  IDLE_EVENT = 0,
  MODE_SET_EVENT,
  KEY_PRESS_EVENT,
  KEY_RELEASE_EVENT,
  SENSOR_ON_EVENT,
  SENSOR_OFF_EVENT,
  MOTOR_ON_EVENT,
  MOTOR_OFF_EVENT,
  TIMER_EVENT,
  MENU_EVENT,
  USER_EVENT = 1000
} event_t;
/*----------------------------------------------------------------------------*/
typedef int (*event_handler_t)(void*, event_t evt, int param1, void *param2);
typedef struct
{
  event_handler_t handler;
  void *data;
} handler_t;
/*----------------------------------------------------------------------------*/
#define NO_HANDLED_EVENTS 0
#define HANDLED_EVENTS 1
#define EVENT_HANDLER_DONE 2
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
void set_event_handler(event_handler_t handler, void *data);
int handle_event(event_t evt, int param1, void *param2);
int push_event_handler(void);
int pop_event_handler(void);
void clear_event_handler_stack();
int process_events(void);
int handler_call(handler_t *h, event_t evt, int param1, void *param2);
#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*EVENT_H_1355816307*/

