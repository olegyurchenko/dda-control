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
/*----------------------------------------------------------------------------*/
static MENU_ITEM root_itm;
static int work_handler(void *data, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
void work_mode_init()
{
  menu_item_init("Work", work_handler, &root_itm);
  menu_item_add_child(root_menu,  &root_itm);
}
/*----------------------------------------------------------------------------*/
void set_work_mode()
{
  set_event_handler(work_handler, 0);
}
/*----------------------------------------------------------------------------*/
static void display()
{
  char buffer[16];
  int value = 0;

  sys_adc_get_value(&value);
  snprintf(buffer, sizeof(buffer), "(0x%03x) %4d", value, value);
  lcd_put_line(1, buffer, SCR_ALIGN_RIGHT);
//  lcd_update();
}
/*----------------------------------------------------------------------------*/
static int work_handler(void *data, event_t evt, int param1, void *param2)
{
  typedef enum
  {
    PlungerCatch,
    CasseteCatch,
    SelMode,
    SelMesh
  } STATE;

  static STATE state = PlungerCatch;

  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;

  switch(evt)
  {
  case IDLE_EVENT:
    display();
    switch(state)
    {
    case PlungerCatch:
      if(!is_plunger_down())
      {
        plunger_go_down();
        return 1;
      }
      state = CasseteCatch;
      break;

    case CasseteCatch:
      if(!is_cassete_calibrated())
      {
        cassette_calibration();
        return 1;
      }
        cassete_goto_cell(CASSETTE_1ST_CELL_POSITION);
      state = SelMode;
      break;
    case SelMode:
      break;
    case SelMesh:
      break;
    default:
      break;
    }
    break;

  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    lcd_clear();
    lcd_put_line(0, "Work", SCR_ALIGN_CENTER);
    break;

  case MENU_EVENT:
    set_event_handler(work_handler, 0);
    state = PlungerCatch;
    return MENU_OK;

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

