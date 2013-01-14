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
#include <sys_timer.h>
#include <dda_config.h>
/*----------------------------------------------------------------------------*/
static int position = CASSETTE_UNKNOWN_POSITION;
static int cassete_pos_handler(void*, event_t evt, int param1, void *param2);
static int dst_position, direction;
/*----------------------------------------------------------------------------*/
typedef enum
{
  Idle = 0,
  WaitNullSensor,
  WaitSensorOff,
  WaitSensorOn,
  WaitMotorOff
} STATE;

static STATE state = Idle;
/*----------------------------------------------------------------------------*/
handler_t cassete_handler = {cassete_pos_handler, 0};
static timeout_t timeout;
/*----------------------------------------------------------------------------*/
void reset_cassette_position()
{
  position = CASSETTE_UNKNOWN_POSITION;
}
/*----------------------------------------------------------------------------*/
int is_cassete_position_unknown()
{
  return position == CASSETTE_UNKNOWN_POSITION;
}
/*----------------------------------------------------------------------------*/
int cassette_position()
{
  return position == CASSETTE_UNKNOWN_POSITION ? CASSETTE_UNKNOWN_POSITION : position / CASSETE_CELL_STEP;
}
/*----------------------------------------------------------------------------*/
void cassete_goto_position(int pos)
{
  pos *= CASSETE_CELL_STEP;
  if(pos >= 0 && pos < CASSETTE_MAX_CELL && pos != position)
  {
    dst_position = pos;
    cassete_handler.handler = cassete_pos_handler;
    cassete_handler.data = 0;
    handler_call(&cassete_handler, MODE_SET_EVENT, 1, 0);
  }
}
/*----------------------------------------------------------------------------*/
static int cassete_pos_handler(void *data, event_t evt, int param1, void *param2)
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
#ifndef NO_CASSETTE
    sensors = sensors_real_state();
    state = Idle;
    if(is_cassete_position_unknown())
    {
      if(sensors & CASSETE_0_SENSOR)
        position = 0;
      else
      {
        state = WaitNullSensor;
        direction = Clockwise;
      }
    }

    if(state == Idle)
    {
      if(dst_position == position)
        return EVENT_HANDLER_DONE;

      if(sensors & CASSETE_CELL_SENSOR)
        state = WaitSensorOff;
      else
        state = WaitSensorOn;

      if(dst_position > position)
        direction = AntiClockwise;
      else
        direction = Clockwise;
    }
    motor_start(CasseteMotor, direction, 0);
    timeout_set(&timeout, CASSETTE_TIMEOUT, sys_tick_count());
#else //NO_CASSETTE
    position = dst_position;
    state = Idle;
#endif //NO_CASSETTE
    break;

  case IDLE_EVENT:
    if(state != Idle && timeout_riched(&timeout, sys_tick_count()))
    {
      motor_stop();
      reset_cassette_position();
      state = Idle;
      return CASSETTE_TIMEOUT_ERROR;
    }
    break;

  case SENSOR_ON_EVENT:
    if(param1 == CASSETE_0_SENSOR && state == WaitNullSensor)
    {
      timeout_set(&timeout, CASSETTE_TIMEOUT, sys_tick_count());
      motor_deceleration();
      break;
    }

    if(param1 == CASSETE_CELL_SENSOR && state == WaitSensorOn)
    {
      state = WaitMotorOff;
      if(direction == AntiClockwise)
      {
        position ++;
        if(position >= dst_position)
          motor_deceleration();
        else
          state = WaitSensorOff;
      }
      else
      {
        position --;
        if(position <= dst_position)
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
    if(state == WaitNullSensor)
    {
      position = 0;
      if(dst_position != position)
      {
        sensors = sensors_real_state();
        if(sensors & CASSETE_CELL_SENSOR)
          state = WaitSensorOff;
        else
          state = WaitSensorOn;

        direction = AntiClockwise;
        motor_start(CasseteMotor, direction, 0);
        timeout_set(&timeout, CASSETTE_TIMEOUT, sys_tick_count());
        break;
      }
    }
    state = Idle;
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_STOP)
    {
      motor_stop();
      reset_cassette_position();
      state = Idle;
      return USER_BREAK;
    }
    break;

  default:
    break;
  }

  if(state == Idle)
    return EVENT_HANDLER_DONE;

  return HANDLED_EVENTS;
}
/*----------------------------------------------------------------------------*/

