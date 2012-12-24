/*----------------------------------------------------------------------------*/
/**
* @pkg dda_mode
*/
/**
* DDA main module.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.12.2012 10:26:55<br>
* @pkgdoc dda_mode
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_mode.h"
#include <event.h>
#include <dda_test_mode.h>
#include <dda_work_mode.h>
#include <dda_lcd.h>
#include <dda_key.h>
#include <sys_timer.h>
#include <dda_clib.h>
#include <dda_cassette.h>
#include <dda_plunger.h>
#include <dda_message.h>
#include <dda_motors.h>
/*----------------------------------------------------------------------------*/
MENU_ITEM *root_menu = 0;
static MENU_ITEM root_itm;
static int root_handler(void *data, event_t evt, int param1, void *param2);
static int splash_handler(void *data, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
#define VERSION "0.1.a"
#define DEVICE_SERIAL "00001"
#define DEVICE_MODEL "DDA-133"
/*----------------------------------------------------------------------------*/
const char *version_str()
{
  static char buffer[32];
  snprintf(buffer, sizeof(buffer), "v%s (build %s)",VERSION, __DATE__);
  return buffer;
}
/*----------------------------------------------------------------------------*/
const char *device_model_str()
{
  return DEVICE_MODEL;
}
/*----------------------------------------------------------------------------*/
const char *device_serial_str()
{
  return DEVICE_SERIAL;
}
/*----------------------------------------------------------------------------*/
void mode_init()
{
  menu_item_init("/", 0, &root_itm);
  root_menu = &root_itm;
  work_mode_init();
  test_mode_init();
  set_event_handler(splash_handler, 0);
}
/*----------------------------------------------------------------------------*/
void set_root_mode()
{
  clear_event_handler_stack();
  set_event_handler(root_handler, 0);
}
/*----------------------------------------------------------------------------*/
void start_root_menu()
{
  set_root_mode();
  start_menu(root_menu);
}
/*----------------------------------------------------------------------------*/
static int root_handler(void *data, event_t evt, int param1, void *param2)
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
    lcd_put_line(0, "root", SCR_ALIGN_CENTER);
    break;
  case MENU_EVENT:
    set_event_handler(root_handler, 0);
    return MENU_OK;
    break;
  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU)
    {
      start_root_menu();
      return 1;
    }
  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
#define SPLASH_TIME 3000 //3s
/*----------------------------------------------------------------------------*/
static void draw_splash_screen()
{
  char buffer[32];
  lcd_clear();
  snprintf(buffer, sizeof(buffer), "%-7s %8s", device_model_str(), device_serial_str());
  lcd_put_line(0, buffer, SCR_ALIGN_LEFT);
  lcd_add_scroll_text(1, 1, lcd_width(), version_str());
}
/*----------------------------------------------------------------------------*/
static int splash_handler(void *data, event_t evt, int param1, void *param2)
{
  static timeout_t timeout;
  typedef enum
  {
    Idle,
    PlungerCatch,
    CasseteCatch
  } STATE;

  static STATE state = PlungerCatch;
  int res;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(state)
  {
  case Idle:
    break;
  case PlungerCatch:
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      cassete_goto_position(CASSETTE_NULL_POSITION);
      state = CasseteCatch;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      state = Idle;
      show_message("Error", "Plunger timeout", 0);
      return -1;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      state = Idle;
      show_message("Error", "Plunger end key", 0);
      return -1;
    }
    break;
  case CasseteCatch:
    res = handler_call(&cassete_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      state = Idle;
    }
    else
    if(res == CASSETTE_TIMEOUT_ERROR)
    {
      state = Idle;
      show_message("Error", "Cassette timeout", 0);
      return -1;
    }
    break;
  }

  switch(evt)
  {
  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    draw_splash_screen();
    timeout_set(&timeout, SPLASH_TIME, sys_tick_count());
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU)
    {
      motor_stop();
      start_root_menu();
      return 1;
    }

  default:
    if(state == Idle && timeout_riched(&timeout, sys_tick_count()))
    {
      set_work_mode(UnknownMode);
      return 1;
    }
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
