/*----------------------------------------------------------------------------*/
/**
* @pkg dda_test_mode
*/
/**
* DDA test mode.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 09.12.2012 11:55:42<br>
* @pkgdoc dda_test_mode
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_test_mode.h"
#include <sys_sheduler.h>
#include <dda_key.h>
#include <dda_lcd.h>
#include <sys_adc.h>
#include <dda_clib.h>
#include <dda_motors.h>
#include <dda_sensor.h>
#include <event.h>
#include <menu.h>
#include <dda_mode.h>
#include <dda_cassette.h>
#include <dda_conv.h>

#define USE_CONSOLE //!!!!
#ifdef USE_CONSOLE
#include <console.h>
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
static int test_handler(void*, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
static int cassete0_handler(void*, event_t evt, int param1, void *param2);
static int cassete_next_handler(void*, event_t evt, int param1, void *param2);
static int top_bottom_handler(void*, event_t evt, int param1, void *param2);
static int adc_handler(void*, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
static MENU_ITEM root_itm,
cassete_motor_itm, cassete0_itm, cassete_next_itm,
rod_motor_itm, fulldown_itm, fullup_itm,
adc_itm;
/*----------------------------------------------------------------------------*/
void test_mode_init()
{
  menu_item_init("Test", 0, &root_itm);
  menu_item_add_child(root_menu,  &root_itm);

  menu_item_init("Cassete motor test", 0, &cassete_motor_itm);
  menu_item_add_child(&root_itm,  &cassete_motor_itm);
  menu_item_init("Goto 0 cell", cassete0_handler, &cassete0_itm);
  menu_item_add_child(&cassete_motor_itm, &cassete0_itm);
  menu_item_init("Goto next cell", cassete_next_handler, &cassete_next_itm);
  menu_item_add_child(&cassete_motor_itm, &cassete_next_itm);


  menu_item_init("Plunger motor test", 0, &rod_motor_itm);
  menu_item_add_child(&root_itm,  &rod_motor_itm);
  menu_item_init("Bottom position", top_bottom_handler, &fulldown_itm);
  fulldown_itm.data = (void *)PlungerDown;
  menu_item_add_child(&rod_motor_itm,  &fulldown_itm);
  menu_item_init("Top position", top_bottom_handler, &fullup_itm);
  menu_item_add_child(&rod_motor_itm,  &fullup_itm);
  fullup_itm.data = (void *)PlungerUp;


  menu_item_init("ADC test", adc_handler, &adc_itm);
  menu_item_add_child(&root_itm,  &adc_itm);

  //set_event_handler(test_handler, 0);
}
/*----------------------------------------------------------------------------*/
static void start_test_menu()
{
  start_menu(&root_itm);
}
/*----------------------------------------------------------------------------*/
static int test_handler(void *data, event_t evt, int param1, void *param2)
{
  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;
  switch(evt)
  {
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    lcd_clear();
    lcd_put_line(0, "Test", SCR_ALIGN_CENTER);
    break;
  case MENU_EVENT:
    set_event_handler(test_handler, 0);
    return MENU_OK;
    break;
  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU)
    {
      start_test_menu();
      return 1;
    }
  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static void dispay_sensors()
{
  int sensors, i;
  sensors = sensors_state();
  for(i = 0; i < 4; i++)
  {
    lcd_put_char(3 - i, 0, (sensors & (1 << i)) ? '1' : '0');
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
  case IDLE_EVENT:
    dispay_sensors();
    break;
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    lcd_clear();
    sensors = sensors_state();
    if(sensors & CASSETE_0_SENSOR)
      state = WaitSensorOff;
    else
      state = WaitSensorOn;
    motor_start(CasseteMotor, state == WaitSensorOff ? PreityClockwise : Clockwise, state == WaitSensorOff ? 100 : 0);
    reset_cassette_calibration();
    break;

  case MENU_EVENT:
    set_event_handler(cassete0_handler, 0);
    return MENU_OK;
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU || param1 == KEY_STOP)
    {
      motor_stop();
      set_event_handler(test_handler, 0);
      start_menu(&cassete_motor_itm);
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
      set_event_handler(test_handler, 0);
      start_menu(&cassete_motor_itm);
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
  int sensors;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(evt)
  {
  case IDLE_EVENT:
    dispay_sensors();
    break;
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    lcd_clear();
    sensors = sensors_state();
    if(sensors & CASSETE_CELL_SENSOR)
      state = WaitSensorOff;
    else
      state = WaitSensorOn;
    motor_start(CasseteMotor, PreityClockwise, 0);
    reset_cassette_calibration();
    break;

  case MENU_EVENT:
    set_event_handler(cassete_next_handler, 0);
    return MENU_OK;
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU || param1 == KEY_STOP)
    {
      motor_stop();
      set_event_handler(test_handler, 0);
      start_menu(&cassete_motor_itm);
      set_menu_pos(1);
      return 1;
    }
    break;
  case SENSOR_ON_EVENT:
    if(param1 == CASSETE_CELL_SENSOR && state == WaitSensorOn)
    {
      state = WaitMotorOff;
      motor_deceleration();
    }
    break;

  case SENSOR_OFF_EVENT:
    if(param1 == CASSETE_CELL_SENSOR && state == WaitSensorOff)
      state = WaitSensorOn;
    break;

  case MOTOR_OFF_EVENT:
    set_event_handler(test_handler, 0);
    start_menu(&cassete_motor_itm);
    set_menu_pos(1);
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static int top_bottom_handler(void *data, event_t evt, int param1, void *param2)
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
  case IDLE_EVENT:
    dispay_sensors();
    break;
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    lcd_clear();
    dir = (int)data;
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

  case MENU_EVENT:
    set_event_handler(top_bottom_handler, data);
    return MENU_OK;
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU || param1 == KEY_STOP)
    {
      motor_stop();
      set_event_handler(test_handler, 0);
      start_menu(&rod_motor_itm);
      set_menu_pos(dir == PlungerDown ? 0 : 1);
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
      set_event_handler(test_handler, 0);
      start_menu(&rod_motor_itm);
      set_menu_pos(dir == PlungerDown ? 0 : 1);
      return 1;
    }
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static void display_adc()
{
  char buffer[16], fbuf[16];
  decimal32_t force;
  int value = 0;

  sys_adc_get_value(&value);
  discrets2force(value, &force);
  decimal32_str(&force, fbuf, sizeof(fbuf));
  snprintf(buffer, sizeof(buffer), "%5s (%03xH %d)", fbuf, value, value);
  lcd_put_line(1, buffer, SCR_ALIGN_LEFT);
//  lcd_update();
}
/*----------------------------------------------------------------------------*/
static void set_zero()
{
  int value = 0;
  sys_adc_get_value(&value);
  set_zero_force(value);
}
/*----------------------------------------------------------------------------*/
static int adc_handler(void *data, event_t evt, int param1, void *param2)
{
  int sensors;
  static int dir;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(evt)
  {
  case IDLE_EVENT:
    dispay_sensors();
    display_adc();
    break;
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    lcd_clear();
    set_zero();
    break;

  case MENU_EVENT:
    set_event_handler(adc_handler, 0);
    return MENU_OK;
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU || param1 == KEY_STOP)
    {
      motor_stop();
      set_event_handler(test_handler, 0);
      start_menu(&root_itm);
      set_menu_pos(2);
      return 1;
    }
    else
    if(param1 == KEY_UP)
    {
      dir = PlungerUp;
      sensors = sensors_state();
      if(!(sensors & UP_SENSOR))
         motor_start(PlungerMotor, dir, 0);
    }
    else
    if(param1 == KEY_DOWN)
    {
      dir = PlungerDown;
      sensors = sensors_state();
      if(!(sensors & DOWN_SENSOR))
        motor_start(PlungerMotor, dir, 0);
    }
    break;

  case KEY_RELEASE_EVENT:
    if(param1 == KEY_UP || param1 == KEY_DOWN)
    {
      motor_deceleration();
    }
    break;

  case SENSOR_ON_EVENT:
    if(   (param1 == DOWN_SENSOR && dir == PlungerDown)
       || (param1 == UP_SENSOR && dir == PlungerUp) )
    {
      //motor_deceleration(); !!!!!!!
      motor_stop();
    }
    break;


  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
