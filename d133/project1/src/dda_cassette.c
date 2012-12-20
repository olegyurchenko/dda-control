/*----------------------------------------------------------------------------*/
/**
* @pkg dda_cassette
*/
/**
* Cassette functions.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.12.2012 12:12:32<br>
* @pkgdoc dda_cassette
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_cassette.h"
#include <dda_motors.h>
#include <dda_key.h>
#include <event.h>
#include <dda_sensor.h>
/*----------------------------------------------------------------------------*/
static int position = CASSETTE_UNKNOWN_POSITION;
static int cassete0_handler(void*, event_t evt, int param1, void *param2);
static int cassete_next_handler(void*, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
void reset_cassette_calibration()
{
  position = CASSETTE_UNKNOWN_POSITION;
}
/*----------------------------------------------------------------------------*/
int is_cassete_calibrated()
{
  return position != CASSETTE_UNKNOWN_POSITION;
}
/*----------------------------------------------------------------------------*/
int cassette_position()
{
  return position;
}
/*----------------------------------------------------------------------------*/
void cassette_calibration()
{
  push_event_handler();
  set_event_handler(cassete0_handler, 0);
}
/*----------------------------------------------------------------------------*/
void cassete_goto_cell(int pos)
{
  push_event_handler();
  set_event_handler(cassete_next_handler, &pos);
  if(!is_cassete_calibrated())
  {
    push_event_handler();
    set_event_handler(cassete0_handler, 0);
  }
}
/*----------------------------------------------------------------------------*/
static int cassete0_handler(void *data, event_t evt, int param1, void *param2)
{
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
    sensors = sensors_state();
    if(sensors & CASSETE_0_SENSOR)
      state = WaitSensorOff;
    else
      state = WaitSensorOn;
    motor_start(CasseteMotor, state == WaitSensorOff ? PreityClockwise : Clockwise, state == WaitSensorOff ? 100 : 0);
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_STOP)
    {
      motor_stop();
      reset_cassette_calibration();
      pop_event_handler();
      return 1;
    }
    break;

  case SENSOR_ON_EVENT:
    if(param1 == CASSETE_0_SENSOR && state == WaitSensorOn)
    {
      state = WaitMotorOff;
      motor_deceleration();
    }
    break;

  case SENSOR_OFF_EVENT:
    if(param1 == CASSETE_0_SENSOR && state == WaitSensorOff)
      motor_deceleration();
    break;

  case MOTOR_OFF_EVENT:
    if(state == WaitSensorOff)
    {
      state = WaitSensorOn;
      motor_start(CasseteMotor, Clockwise, 100);
    }
    else
    {
      position = CASSETTE_NULL_POSITION;
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
static int cassete_next_handler(void *data, event_t evt, int param1, void *param2)
{
  typedef enum
  {
    WaitSensorOff,
    WaitSensorOn,
    WaitMotorOff
  } STATE;

  static STATE state;
  static int end_position, dir;

  int sensors;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(evt)
  {
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    end_position = *(int *)data;
    sensors = sensors_state();
    if(sensors & CASSETE_CELL_SENSOR)
      state = WaitSensorOff;
    else
      state = WaitSensorOn;
    if(end_position == position)
    {
      pop_event_handler();
      return 1;
    }
    else
    if(end_position > position)
      dir = PreityClockwise;
    else
      dir = Clockwise;

    motor_start(CasseteMotor, dir, 0);
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
    if(param1 == CASSETE_CELL_SENSOR && state == WaitSensorOn)
    {
      state = WaitMotorOff;
      if(dir == PreityClockwise)
      {
        position ++;
        if(position >= end_position)
          motor_deceleration();
        else
          state = WaitSensorOff;
      }
      else
      {
        position --;
        if(position <= end_position)
          motor_deceleration();
        else
          state = WaitSensorOff;
      }
    }
    break;

  case SENSOR_OFF_EVENT:
    if(param1 == CASSETE_CELL_SENSOR && state == WaitSensorOff)
      state = WaitSensorOn;
    break;

  case MOTOR_OFF_EVENT:
    pop_event_handler();
    return 1;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/

