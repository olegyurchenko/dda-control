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
/*----------------------------------------------------------------------------*/
static int position = -1;
static int plunger_go_handler(void *data, event_t evt, int param1, void *param2);
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
  int dir = PlungerDown;
  push_event_handler();
  set_event_handler(plunger_go_handler, &dir);
}
/*----------------------------------------------------------------------------*/
static int plunger_go_handler(void *data, event_t evt, int param1, void *param2)
{
  static int dir = PlungerDown;
  typedef enum
  {
    WaitSensorOff,
    WaitSensorOn,
    WaitMotorOff
  } STATE;

  static STATE state;
  int sensors;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(evt)
  {
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    dir = *(int *)data;
    sensors = sensors_state();
    if(   (dir == PlungerDown && (sensors & DOWN_SENSOR))
       || (dir == PlungerUp && (sensors & UP_SENSOR)) )
    {
      state = WaitSensorOff;
      motor_start(PlungerMotor, dir == PlungerDown ? PlungerUp : PlungerDown, 50);
    }
    else
    {
      state = WaitSensorOn;
      motor_start(PlungerMotor, dir, 0);
    }
    break;


  case KEY_PRESS_EVENT:
    if(param1 == KEY_STOP)
    {
      motor_stop();
      pop_event_handler();
      return 1;
    }
    break;

  case SENSOR_ON_EVENT:
    if(   (param1 == DOWN_SENSOR && state == WaitSensorOn && dir == PlungerDown)
       || (param1 == UP_SENSOR && state == WaitSensorOn && dir == PlungerUp) )
    {
      state = WaitMotorOff;
      //motor_deceleration(); !!!!!!!
      motor_stop();
    }
    break;

  case SENSOR_OFF_EVENT:
    if(   (param1 == DOWN_SENSOR && state == WaitSensorOff && dir == PlungerDown)
       || (param1 == UP_SENSOR && state == WaitSensorOff && dir == PlungerUp) )
      motor_deceleration();
    break;

  case MOTOR_OFF_EVENT:
    if(state == WaitSensorOff)
    {
      state = WaitSensorOn;
      motor_start(PlungerMotor, dir, 50);
    }
    else
    {
      pop_event_handler();
      return 1;
    }
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
