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
#include "dda_plunger.h"
#include <dda_motors.h>
#include <dda_key.h>
#include <event.h>
#include <dda_sensor.h>
#include <sys_timer.h>
#include <dda_motors.h>
/*----------------------------------------------------------------------------*/
#define PLUNGER_TIMEOUT 40000 //40s
typedef enum
{
  Idle,
  WaitSensorOff,
  WaitSensorOn,
  WaitMotorOff
} STATE;
/*----------------------------------------------------------------------------*/
static STATE state;
static int direction = PlungerDown;
static int position = -1;
static int plunger_go_handler(void *data, event_t evt, int param1, void *param2);
handler_t plunger_handler = {plunger_go_handler, 0};
static timeout_t timeout;
/*----------------------------------------------------------------------------*/
int is_plunger_down()
{
  return sensors_state() & DOWN_SENSOR;
}
/*----------------------------------------------------------------------------*/
int plunger_position()
{
  return position;
}
/*----------------------------------------------------------------------------*/
void plunger_go_down()
{
  plunger_handler.handler = plunger_go_handler;
  plunger_handler.data = 0;
  direction = PlungerDown;
  handler_call(&plunger_handler, MODE_SET_EVENT, 1, 0);
}
/*----------------------------------------------------------------------------*/
static int plunger_go_handler(void *data, event_t evt, int param1, void *param2)
{
  int sensors;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(evt)
  {
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    state = Idle;
    sensors = sensors_state();
    if(   (direction == PlungerDown && (sensors & DOWN_SENSOR))
       || (direction == PlungerUp && (sensors & UP_SENSOR)) )
      break;

    state = WaitSensorOn;
    motor_start(PlungerMotor, direction, 0);
    timeout_set(&timeout, PLUNGER_TIMEOUT, sys_tick_count());
    break;

  case IDLE_EVENT:
    if(state != Idle && timeout_riched(&timeout, sys_tick_count()))
    {
      motor_stop();
      state = Idle;
      return PLUNGER_TIMEOUT_ERROR;
    }
    break;

  case SENSOR_ON_EVENT:
    if(   (param1 == DOWN_SENSOR && state == WaitSensorOn && direction == PlungerDown)
       || (param1 == UP_SENSOR && state == WaitSensorOn && direction == PlungerUp) )
    {
      state = WaitMotorOff;
      //motor_deceleration(); !!!!!!!
      motor_stop();
    }
    break;

  case MOTOR_OFF_EVENT:
    state = Idle;
    break;

  default:
    break;
  }

  if(state == Idle)
  {
    sensors = sensors_state();
    return (sensors & UP_SENSOR) ? PLUNGER_END_POS_ERROR : EVENT_HANDLER_DONE;
  }

  return HANDLED_EVENTS;
}
/*----------------------------------------------------------------------------*/
