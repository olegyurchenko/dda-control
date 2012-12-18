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
#include "event.h"
#include <dda_key.h>
#include <dda_sensor.h>
#include <dda_motors.h>
/*----------------------------------------------------------------------------*/
#define EVENT_STACK_SIZE 8
typedef struct
{
  event_handler_t handler;
  void *data;
} EVT_HANDLER;
/*----------------------------------------------------------------------------*/
static int handler_shack_point = 0;
/*----------------------------------------------------------------------------*/
static EVT_HANDLER event_stack[EVENT_STACK_SIZE];
static EVT_HANDLER current_handler = {0, 0};
/*----------------------------------------------------------------------------*/
void set_event_handler(event_handler_t handler, void *data)
{
  if(current_handler.handler != handler || current_handler.data != data)
  {
    handle_event(MODE_SET_EVENT, 0, 0);
    current_handler.handler = handler;
    current_handler.data = data;
    handle_event(MODE_SET_EVENT, 1, 0);
  }
}
/*----------------------------------------------------------------------------*/
int handle_event(event_t evt, int param1, void *param2)
{
  if(current_handler.handler != 0)
    return current_handler.handler(current_handler.data, evt, param1, param2);
  return 0;
}
/*----------------------------------------------------------------------------*/
int push_event_handler(void)
{
  if(handler_shack_point < EVENT_STACK_SIZE - 1)
  {
    event_stack[handler_shack_point ++] = current_handler;
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
int pop_event_handler(void)
{
  if(handler_shack_point)
  {
    set_event_handler(event_stack[handler_shack_point].handler, event_stack[handler_shack_point].data);
    handler_shack_point --;
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static int key_handler(int state, int old_state)
{
  int i, result = 0;
  for(i = 0; i < 6; i++)
  {
    if((state ^ old_state) & (1 << i))
    {
      if(state & (1 << i))
        result += handle_event(KEY_PRESS_EVENT, 1 << i, 0);
      else
        result += handle_event(KEY_RELEASE_EVENT, 1 << i, 0);
    }
  }
  return result;
}
/*----------------------------------------------------------------------------*/
static int sensor_handler(int state, int old_state)
{
  int i, result = 0;
  for(i = 0; i < 4; i++)
  {
    if((state ^ old_state) & (1 << i))
    {
      if(state & (1 << i))
        result += handle_event(SENSOR_ON_EVENT, 1 << i, 0);
      else
        result += handle_event(SENSOR_OFF_EVENT, 1 << i, 0);
    }
  }
  return result;
}
/*----------------------------------------------------------------------------*/
static int motor_handler(int state, int old_state)
{
  if(state)
    return handle_event(MOTOR_ON_EVENT, 1, 0);
  if(old_state)
    return handle_event(MOTOR_OFF_EVENT, 0, 0);
  return 0;
}
/*----------------------------------------------------------------------------*/
int process_events(void)
{
  static int key_state = 0;
  static int sensor_state = 0;
  static int motor_state = 0;
  int state, result = 0;

  state = keys_state();
  if(state != key_state)
    result += key_handler(state, key_state);
  key_state = state;
  state = sensors_state();
  if(state != sensor_state)
    result += sensor_handler(state, sensor_state);
  sensor_state = state;
  state = !is_motor_idle();
  if(state != motor_state)
    result += motor_handler(state, motor_state);
  motor_state = state;
  if(!result)
    result += handle_event(IDLE_EVENT, 0, 0);
  return result;
}
/*----------------------------------------------------------------------------*/
