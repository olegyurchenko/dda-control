/*----------------------------------------------------------------------------*/
/**
* @pkg dda_work_mode
*/
/**
* DDA work modee.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.12.2012 10:27:00<br>
* @pkgdoc dda_work_mode
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_work_mode.h"
#include <dda_mode.h>
#include <event.h>
#include <dda_lcd.h>
#include <dda_key.h>
#include <sys_adc.h>
#include <dda_cassette.h>
#include <dda_plunger.h>
#include <dda_clib.h>
#include <dda_conv.h>
#include <dda_message.h>
#include <dda_motors.h>
#include <dda_text.h>
#include <dda_mesh.h>
/*----------------------------------------------------------------------------*/
static MENU_ITEM root_itm, auto_itm, manual_itm;
static int work_handler(void *data, event_t evt, int param1, void *param2);
static int mode_item_handler(void *data, event_t evt, int param1, void *param2);
static int work_item_handler(void *data, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
typedef enum
{
  Idle,
  SelMode,
  SelMesh,
  PlungerCatch,
  CasseteCatch
} STATE;

static STATE state = Idle;
static work_mode_t mode = UnknownMode;
/*----------------------------------------------------------------------------*/
void work_mode_init()
{
  menu_item_init(get_text(STR_WORK), work_item_handler, &root_itm);
  menu_item_add_child(root_menu,  &root_itm);
  menu_item_init(get_text(STR_AUTO), mode_item_handler, &auto_itm);
  menu_item_add_child(&root_itm,  &auto_itm);
  menu_item_init(get_text(STR_MANUAL), mode_item_handler, &manual_itm);
  menu_item_add_child(&root_itm,  &manual_itm);
}
/*----------------------------------------------------------------------------*/
void set_work_mode(work_mode_t m)
{
  state = Idle;
  mode = m;
  set_event_handler(work_handler, 0);
  if(mode == UnknownMode)
    start_menu(&root_itm);
}
/*----------------------------------------------------------------------------*/
work_mode_t work_mode()
{
  return mode;
}
/*----------------------------------------------------------------------------*/
static void display()
{
  char buffer[16], fbuf[16];
  decimal32_t force;
  int value = 0;
  const char *state_msg;

  state_msg = get_text(STR_WORK_MODE);

  if(mode == AutoMode)
    state_msg = get_text(STR_AUTO_MODE);
  else
  if(mode == ManualMode)
    state_msg = get_text(STR_MANUAL_MODE);

  switch(state)
  {
  case Idle:
    break;
  case PlungerCatch:
    state_msg = get_text(STR_PLUNGER_GO_HOME);
    break;
  case CasseteCatch:
    state_msg = get_text(STR_CASSETTE_GO_HOME);
    break;
  case SelMode:
    break;
  case SelMesh:
    break;
  }
  lcd_put_line(0, state_msg, SCR_ALIGN_CENTER);
  sys_adc_get_value(&value);
  discrets2force(value, &force);
  decimal32_str(&force, fbuf, sizeof(fbuf));
  snprintf(buffer, sizeof(buffer), "%8s", fbuf);
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
/*Use only for return menu position: Auto or Manual mode*/
static int work_item_handler(void *data, event_t evt, int param1, void *param2)
{
  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;
  switch(evt)
  {
  case MENU_EVENT:
    return MENU_CONTINUE;

  case MENU_GET_POSITION:
    switch(work_mode())
    {
    case AutoMode:
      *(int *)param2 = 0;
      return MENU_OK;
    case ManualMode:
      *(int *)param2 = 1;
      return MENU_OK;
    default:
      break;
    }
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static int mode_item_handler(void *data, event_t evt, int param1, void *param2)
{
  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;
  switch(evt)
  {
  case MENU_EVENT:
    if(param2 == &auto_itm)
    {
      mode = AutoMode;
      auto_itm.childs = 0;
      manual_itm.childs = 0;
      init_mesh_menu(&auto_itm);

    }
    else
    if(param2 == &manual_itm)
    {
      mode = ManualMode;
      auto_itm.childs = 0;
      manual_itm.childs = 0;
      init_mesh_menu(&manual_itm);
    }

    state = Idle;
    set_event_handler(work_handler, 0);
    return MENU_CONTINUE;

  case MENU_GET_POSITION:
    if(mesh_index() != INVALID_MESH_INDEX)
    {
      *(int *)param2 = mesh_index();
      return MENU_OK;
    }
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static int work_handler(void *data, event_t evt, int param1, void *param2)
{
  int res;

  (void) data; //Prevent unused warning

  switch(state)
  {
  case Idle:
    break;
  case PlungerCatch:
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      cassete_goto_position(CASSETTE_1ST_CELL);
      state = CasseteCatch;
      set_zero();
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      state = Idle;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return -1;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      state = Idle;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
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
      show_message(get_text(STR_ERROR), get_text(STR_CASSETTE_TIMEOUT), 0);
      return -1;
    }
    break;
  case SelMesh:
    plunger_go_down();
    state = PlungerCatch;
    break;
default:
    break;
  }

  switch(evt)
  {
  case IDLE_EVENT:
    display();
    break;

  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    lcd_clear();
    lcd_put_line(0, get_text(STR_WORK), SCR_ALIGN_CENTER);
    set_zero();
    break;


  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU && state == Idle)
    {
      start_root_menu();
    }

    if(param1 == KEY_STOP)
    {
      if(state == PlungerCatch
         || state == CasseteCatch)
      {
        motor_stop();
        show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
        state = Idle;
      }
    }

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/

