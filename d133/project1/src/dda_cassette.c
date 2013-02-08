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
#include <dda_settings.h>
#include <dda_clib.h>
/*----------------------------------------------------------------------------*/
static int position = CASSETTE_UNKNOWN_POSITION;
static int cassette_pos_handler(void*, event_t evt, int param1, void *param2);
static int dst_position, direction;
static int is_init = 0;
static int max_cell = CASSETTE_MAX_CELL;
static int cassete_cell_step = CASSETE_CELL_STEP;
static unsigned cassette_timeout = CASSETTE_TIMEOUT;
/*----------------------------------------------------------------------------*/
//#define CASSETTE_0_DIRECTION AntiClockwise
//#define CASSETTE_DIRECTION AntiClockwise
#define CASSETTE_0_DIRECTION Clockwise
#define CASSETTE_DIRECTION Clockwise
/*----------------------------------------------------------------------------*/
typedef enum
{
  Idle = 0,
  WaitNullSensorOn,
  WaitNullSensorOff,
  WaitSensorOff,
  WaitSensorOn,
  WaitMotorOff
} STATE;

static STATE state = Idle;
/*----------------------------------------------------------------------------*/
handler_t cassette_handler = {cassette_pos_handler, 0};
static timeout_t timeout;
/*----------------------------------------------------------------------------*/
static void init()
{
  if(!is_init)
  {
    const char *v;

    v = setting_get(S_CELL_STEP);
    if(v && *v)
    {
      cassete_cell_step = atoi(v);
      if(!cassete_cell_step)
        cassete_cell_step = 1;
      max_cell = CASSETTE_MAX_CELL/cassete_cell_step;
    }

    v = setting_get(S_C_TIMEOUT);
    if(v && *v)
      cassette_timeout = atoi(v);

    is_init = 1;
  }
}
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
  init();
  return position == CASSETTE_UNKNOWN_POSITION ? CASSETTE_UNKNOWN_POSITION : position / cassete_cell_step;
}
/*----------------------------------------------------------------------------*/
int cassete_max_cell()
{
  init();
  return CASSETTE_MAX_CELL/cassete_cell_step;
}
/*----------------------------------------------------------------------------*/
void cassete_goto_position(int pos)
{
  init();
  pos *= cassete_cell_step;
  if(pos >= 0 && pos < CASSETTE_MAX_CELL && pos != position)
  {
    dst_position = pos;
    cassette_handler.handler = cassette_pos_handler;
    cassette_handler.data = 0;
    handler_call(&cassette_handler, MODE_SET_EVENT, 1, 0);
  }
}
/*----------------------------------------------------------------------------*/
static int cassette_pos_handler(void *data, event_t evt, int param1, void *param2)
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
    if(is_cassete_position_unknown()
      || (dst_position != position && dst_position == CASSETTE_NULL_POSITION))
    {
      state = WaitNullSensorOn;
      direction = CASSETTE_0_DIRECTION;
      if(sensors & CASSETE_0_SENSOR)
      {
        direction = ! CASSETTE_0_DIRECTION;
        state = WaitNullSensorOff;
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
        direction = CASSETTE_DIRECTION;
      else
        direction = !CASSETTE_DIRECTION;
    }
    motor_start(CasseteMotor, direction, 0);
    timeout_set(&timeout, cassette_timeout, sys_tick_count());
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
    if(param1 == CASSETE_0_SENSOR && state == WaitNullSensorOn)
    {
      timeout_set(&timeout, cassette_timeout, sys_tick_count());
      motor_deceleration();
      break;
    }

    if(param1 == CASSETE_CELL_SENSOR && state == WaitSensorOn)
    {
      state = WaitMotorOff;
      if(direction == CASSETTE_DIRECTION)
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
    else
    if(param1 == CASSETE_0_SENSOR && state == WaitNullSensorOff)
    {
      timeout_set(&timeout, cassette_timeout, sys_tick_count());
      motor_deceleration();
      break;
    }
    break;

  case MOTOR_OFF_EVENT:
    if(state == WaitNullSensorOn)
    {
      position = 0;
      if(dst_position != position)
      {
        sensors = sensors_real_state();
        if(sensors & CASSETE_CELL_SENSOR)
          state = WaitSensorOff;
        else
          state = WaitSensorOn;

        direction = CASSETTE_DIRECTION;
        motor_start(CasseteMotor, direction, 0);
        timeout_set(&timeout, cassette_timeout, sys_tick_count());
        break;
      }
    }

    if(state == WaitNullSensorOff)
    {
      direction = CASSETTE_0_DIRECTION;
      state = WaitNullSensorOn;
      motor_start(CasseteMotor, direction, 0);
      timeout_set(&timeout, cassette_timeout, sys_tick_count());
      break;
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

