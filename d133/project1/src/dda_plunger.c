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
#include <dda_clib.h>
#include <dda_motors.h>
#include <dda_key.h>
#include <event.h>
#include <dda_sensor.h>
#include <sys_timer.h>
#include <dda_motors.h>
#include <dda_config.h>
#include <dda_settings.h>
#ifdef USE_CONSOLE
#include <console.h>
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
typedef enum
{
  Idle,
  WaitSensorOff,
  WaitSensorOn,
  WaitMotorOff,
  Mileage,
  EndMileage
} STATE;
/*----------------------------------------------------------------------------*/
static STATE state;
static int direction = PlungerDown;
static int plunger_go_handler(void *data, event_t evt, int param1, void *param2);
handler_t plunger_handler = {plunger_go_handler, 0};
static timeout_t timeout;
static unsigned m_touch_position = 0;
static unsigned top_position = 0;
static unsigned zero_step = 0; //Motor step count when DOWN_SENSEOR OFF
static int is_init = 0;
static unsigned plunger_timeout = PLUNGER_TIMEOUT;
static uint8_t slow_rate = SLOW_RATE;
static int unknown_position = 1;
/*----------------------------------------------------------------------------*/
#define MILEAGE_STEPS 2000
/*----------------------------------------------------------------------------*/
static void init()
{
  if(!is_init)
  {
    const char *v;
    v = setting_get(S_PLUNGER_TIMEOUT);
    if(v && *v)
      plunger_timeout = atoi(v);
    v = setting_get(S_SLOW_RATE);
    if(v && *v)
      slow_rate = atoi(v);

    is_init = 1;
  }
}
/*----------------------------------------------------------------------------*/
int is_plunger_down()
{
  return sensors_real_state() & DOWN_SENSOR;
}
/*----------------------------------------------------------------------------*/
unsigned plunger_position()
{
  unsigned count;
  count = motor_step_count();

  if(direction == PlungerDown)
  {
    if(count > top_position)
      return 0;
    return top_position - count;
  }
  return  count /*- zero_step*/;
}
/*----------------------------------------------------------------------------*/
unsigned touch_position()
{
  return m_touch_position;
}
/*----------------------------------------------------------------------------*/
void set_touch_position()
{
  m_touch_position = plunger_position();
}
/*----------------------------------------------------------------------------*/
void reset_plunger_position()
{
  unknown_position = 1;
}
/*----------------------------------------------------------------------------*/
void plunger_go_down()
{
  init();
  plunger_handler.handler = plunger_go_handler;
  plunger_handler.data = 0;
  direction = PlungerDown;
  handler_call(&plunger_handler, MODE_SET_EVENT, 1, 0);
}
/*----------------------------------------------------------------------------*/
void plunger_go_up()
{
  init();
  plunger_handler.handler = plunger_go_handler;
  plunger_handler.data = 0;
  direction = PlungerUp;
  handler_call(&plunger_handler, MODE_SET_EVENT, 1, 0);
}
/*----------------------------------------------------------------------------*/
void plunger_stop()
{
  handler_call(&plunger_handler, MODE_SET_EVENT, 0, 0);
}
/*----------------------------------------------------------------------------*/
static int plunger_go_handler(void *data, event_t evt, int param1, void *param2)
{
  static int ret_code;
  int sensors;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(evt)
  {
  case MODE_SET_EVENT:
    if(!param1) //Stop
    {
      if(state != Idle)
      {
        state = WaitMotorOff;
        motor_deceleration();
        //motor_stop();
        ret_code = EVENT_HANDLER_DONE;
      }
      return 0;
    }
    state = Idle;
    ret_code = EVENT_HANDLER_DONE;
    //sensors = sensors_real_state();
    sensors = sensors_real_state();
    if(   (direction == PlungerDown && (sensors & DOWN_SENSOR))
       || (direction == PlungerUp && (sensors & UP_SENSOR)) )
    {
      if(!unknown_position)
        break;
      state = Mileage;
      motor_start(PlungerMotor, !direction, 0);
    }
    else
    {
      state = WaitSensorOn;
      motor_start(PlungerMotor, direction, 0);
    }
    timeout_set(&timeout, plunger_timeout, sys_tick_count());
    break;

  case IDLE_EVENT:
    if(state != Idle && timeout_riched(&timeout, sys_tick_count()))
    {
      state = WaitMotorOff;
      motor_deceleration();
      ret_code = PLUNGER_TIMEOUT_ERROR;
    }

    if(direction == PlungerDown && top_position)
    {
      if(motor_rate() > slow_rate && plunger_position() <= SLOW_DOWN_POSITION)
        motor_change_rate(slow_rate);
    }

    if(state == Mileage && motor_step_count() >= MILEAGE_STEPS)
    {
      motor_deceleration();
      state = EndMileage;
    }
    break;

  case SENSOR_ON_EVENT:
    if(   (param1 == DOWN_SENSOR && state == WaitSensorOn && direction == PlungerDown)
       || (param1 == UP_SENSOR && state == WaitSensorOn && direction == PlungerUp) )
    {
      state = WaitMotorOff;
      motor_deceleration(); //!!!!!!!
      //motor_stop();
#ifdef USE_CONSOLE
      console_printf("\r\ntop: %u", motor_step_count());
#endif
    }
    break;

  case SENSOR_OFF_EVENT:
    if(param1 == DOWN_SENSOR && direction == PlungerUp)
    {
      zero_step = motor_step_count();
#ifdef USE_CONSOLE
      console_printf("\r\nzero_step: %u", zero_step);
#endif
    }
    break;

  case MOTOR_OFF_EVENT:
    if(state == EndMileage)
    {
      top_position = 0;
      state = WaitSensorOn;
      motor_start(PlungerMotor, direction, slow_rate);
      break;
    }

    if(direction == PlungerUp && !(sensors_real_state() & UP_SENSOR))
      top_position = motor_step_count();
    else
      top_position = 0;
    state = Idle;
    unknown_position = 0;
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_STOP)
    {
      state = WaitMotorOff;
      motor_deceleration();
      ret_code = USER_BREAK;
    }
    break;

  default:
    break;
  }

  if(state == Idle)
  {
    sensors = sensors_real_state();
    return (sensors & UP_SENSOR) ? PLUNGER_END_POS_ERROR : ret_code;
  }

  return HANDLED_EVENTS;
}
/*----------------------------------------------------------------------------*/
