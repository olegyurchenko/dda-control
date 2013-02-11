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
#include <spin_edit.h>
#include <sys_timer.h>
#include <dda_protocol.h>
#include <dda_db.h>
#include <dda_config.h>
#include <dda_result_view.h>

#ifdef USE_CONSOLE
#include <console.h>
#define DEBUG
#endif

/*----------------------------------------------------------------------------*/
static MENU_ITEM root_itm, auto_itm, manual_itm;
static int work_handler(void *data, event_t evt, int param1, void *param2);
static int calibrarion_handler(void *data, event_t evt, int param1, void *param2);
static int done_handler(void *data, event_t evt, int param1, void *param2);
static int measuring_handler(void *data, event_t evt, int param1, void *param2);
static int mode_item_handler(void *data, event_t evt, int param1, void *param2);
static int work_item_handler(void *data, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
static work_state_t state = Idle;
static work_mode_t mode = UnknownMode;
static int particles = 1;
static int start_flag = 0;
static decimal32_t force = {0,0};
static decimal32_t result_force = {0,0};
static decimal32_t size = {0,0};
static unsigned measure_touch_position;
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
  mode = m;
  if(mode == UnknownMode)
    state = Idle;
}
/*----------------------------------------------------------------------------*/
void set_samples(int samples)
{
  particles = samples;
}
/*----------------------------------------------------------------------------*/
void start_work()
{
  set_event_handler(work_handler, 0);
  if(mode == UnknownMode)
  {
    state = Idle;
    //start_menu(&root_itm);
  }
  else
  {
    if(state == Idle)
    {
      state = Calibration;
      if(mode == AutoMode)
        start_flag = 1;
      else
        start_flag = 0;
     }
     else
      start_flag = 1;
  }
}
/*----------------------------------------------------------------------------*/
work_mode_t work_mode()
{
  return mode;
}
/*----------------------------------------------------------------------------*/
work_state_t work_state()
{
  return state;
}
/*----------------------------------------------------------------------------*/
void start_work_menu()
{
  clear_event_handler_stack();
  state = Idle;
  set_event_handler(work_handler, 0);
  start_menu(&root_itm);
}
/*----------------------------------------------------------------------------*/
static void display()
{
  char buffer[16], fbuf[16], sbuf[16];
  const char *state_msg;

  state_msg = get_text(STR_WORK_MODE);

  if(mode == AutoMode)
    state_msg = get_text(STR_AUTO_MODE);
  else
  if(mode == ManualMode)
    state_msg = get_text(STR_MANUAL_MODE);

  buffer[0] = '\0';
  switch(state)
  {
  case Idle:
    break;
  case Calibration:
    state_msg = get_text(STR_CALIBRATION);
    decimal32_str(&force, fbuf, sizeof(fbuf));
    snprintf(buffer, sizeof(buffer), "f:%-5s", fbuf);
    break;
  case Measuring:
    state_msg = get_text(STR_MEASURING);
    decimal32_str(&force, fbuf, sizeof(fbuf));
    decimal32_str(&size, sbuf, sizeof(sbuf));
    snprintf(buffer, sizeof(buffer), "f:%-5s s:%-6s", fbuf, sbuf);
    break;
  case NexCasseteWait:
    state_msg = get_text(STR_CASSETTE_WAIT);
    break;
  case Done:
    state_msg = get_text(STR_FINISHING);
    break;
  }
  lcd_put_line(0, state_msg, SCR_ALIGN_CENTER);
  if(buffer[0])
    lcd_put_line(1, buffer, SCR_ALIGN_LEFT);
}
/*----------------------------------------------------------------------------*/
static void set_zero()
{
  int value = 0;
  sys_adc_get_value(&value);
  set_zero_force(value);
}
/*----------------------------------------------------------------------------*/
static int touch_detect()
{
  int value = 0;
  sys_adc_get_value(&value);
  discrets2force(value, &force); //For display force
  umsize(touch_position(), plunger_position(), &size); //For display size
#ifdef DEBUG
  if(is_touch_force(value))
  {
    console_printf("tm:%u pos:%u val:%d\r\n", sys_tick_count() % 1000, plunger_position(), value);
  }
#endif
  return is_touch_force(value);
}
/*----------------------------------------------------------------------------*/
static int calibration_force_riched()
{
  int value = 0;
  sys_adc_get_value(&value);
  discrets2force(value, &force); //For display force
  if(decimal32_cmp(&force, get_calibration_force()) > 0)
    return 1;
  return 0;
}
/*----------------------------------------------------------------------------*/
static void calc_calibration_results() //Calculate calibration results
{
  int value = 0;
  sys_adc_get_value(&value);
  set_calibration_data(plunger_position() - touch_position(), value);
}
/*----------------------------------------------------------------------------*/
/**Return 0 if no particle*/
static int set_size_postion()
{
  static decimal32_t min_size, k;
  unsigned pos;

  pos = plunger_position();

  umsize(touch_position(), pos, &size);
  min_size = decimal32_init(mesh()->min, 0);
  k = decimal32_init(1, 0);
  decimal32_sub(&k, relative_size_deviation(), &k); //k = 1 - 0.25;
  decimal32_mul(&min_size, &k, &min_size);
  if(decimal32_cmp(&size, &min_size) <= 0) //size <= min_size
  {
    protocol_push_no_particle();
    return 0;
  }

  result_force = decimal32_init(0, 0);
  measure_touch_position = pos;
  protocol_push_grain_size(&size);
  return 1;
}
/*----------------------------------------------------------------------------*/
static int destruction_detect()
{
  int value = 0;
  sys_adc_get_value(&value);
  discrets2force(value, &force); //For display force
  protocol_push_current_force(&force);
  if(decimal32_cmp(&force, &result_force) > 0)
    result_force = force;

  if(decimal32_cmp(&force, get_max_force()) > 0)
    return 1;
  return is_destruction(touch_position() - measure_touch_position,
                        plunger_position() - measure_touch_position,
                        value
                        );
}
/*----------------------------------------------------------------------------*/
static void set_destruction_position()
{
  int value = 0;
  measure_t measure;

  sys_adc_get_value(&value);
  discrets2force(value, &force);

  measure.cell = cassette_position();
  measure.size = size;
  measure.strength = result_force;

  protocol_push_strength(db_measure_count(), measure.cell, &result_force);
  db_add_measure(&measure);
#ifdef DEBUG
  {
    char buffer[32];
    decimal32_str(&force, buffer, sizeof(buffer));
    console_printf("\r\ncur:%s result:", buffer);
    decimal32_str(&result_force, buffer, sizeof(buffer));
    console_printf("%s\r\n", buffer);
  }
#endif
  force = result_force;
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

    state = Calibration;
    set_event_handler(work_handler, 0);
    if(mode == AutoMode)
    {
      spin_edit_start(get_text(STR_NUMBER_OF_SAMPLES), &particles, cassete_max_cell(), 1, 1);
      start_flag = 1;
    }
    else
    {
      start_flag = 0;
    }
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
  static int samples = 0;

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
    return 0;


  case KEY_PRESS_EVENT:
    if(param1 == KEY_MENU)
    {
      if(state == Idle)
        start_root_menu();
    }
    else
    if(param1 == KEY_STOP)
    {
      start_flag = 0;
    }
    else
    if(param1 == KEY_START)
    {
      start_flag = 1;
    }
    break;

  default:
    break;
  }


  switch(state)
  {
  case Idle:
    start_work_menu();
    break;

  case Calibration:
    res = calibrarion_handler(data, evt, param1, param2);
    if(res == USER_BREAK)
    {
      state = Done;
    }
    else
    if(res < 0)
    {
      state = Idle;
      start_flag = 0;
    }
    else
    if(res == EVENT_HANDLER_DONE)
    {
      db_new_session(mesh_index(), particles);
      state = Measuring;
      samples = 0;
      start_flag = mode == AutoMode ? 1 : 0;
    }
    break;

  case Measuring:
    res = measuring_handler(data, evt, param1, param2);
    if(res == USER_BREAK)
    {
      state = Done;
    }
    else
    if(res < 0)
    {
      state = Idle;
      start_flag = 0;
    }
    else
    if(res == EVENT_HANDLER_DONE)
    {
      if(mode == AutoMode)
      {
        samples ++;
        if(samples >= particles)
        {
          state = Done;
        }
      }

      start_flag = mode == AutoMode ? 1 : 0;
    }
    break;

  case Done:
    res = done_handler(data, evt, param1, param2);
    if(res < 0 || res == EVENT_HANDLER_DONE)
    {
      state = Idle;
      start_flag = 0;
      if(db_measure_count())
        view_result();
    }
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
static int calibrarion_handler(void *data, event_t evt, int param1, void *param2)
{
  typedef enum
  {
    StartState,
    PlungerCatch,
    CassetteCatch,
    WaitTouch,
    CalibrationForce,
    Down
  } MICRO_STATE;

  static MICRO_STATE micro_state = StartState;
  static timeout_t timeout = {0, 0};
  static int cursor = 0;
  static int work_area;
  static int calibration_finish;
  static int slow_offset;
  static int touch_count = 0;
  int res;

  (void) data; //Prevent unused warning
  switch(micro_state)
  {
  case StartState:
    if(start_flag)
    {
      plunger_go_down();
      micro_state = PlungerCatch;
      calibration_finish = 0;
      slow_offset = um2steps(um_SLOW_OFFSET);
      touch_count = 0;
      work_area = 0;
    }
    else
    {
      if(evt == KEY_PRESS_EVENT && param1 == KEY_STOP)
      {
        micro_state = StartState;
        return USER_BREAK;
      }

      if(!timeout.timeout || timeout_riched(&timeout, sys_tick_count()))
      {
        cursor = !cursor;
        timeout_set(&timeout, 800, sys_tick_count());
      }
      lcd_put_line(1, cursor ? get_text(STR_PRESS_START) : " ", SCR_ALIGN_CENTER);
    }
    break;

  case PlungerCatch:
    lcd_put_line(1, get_text(STR_PLUNGER_GO_HOME), SCR_ALIGN_CENTER);
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      cassete_goto_position(CASSETTE_1ST_CELL);
      micro_state = CassetteCatch;
      set_zero();
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  case CassetteCatch:
    lcd_put_line(1, get_text(STR_CASSETTE_GO_HOME), SCR_ALIGN_CENTER);
    res = handler_call(&cassette_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      set_zero();
      plunger_go_up();
      micro_state = WaitTouch;
    }
    else
    if(res == CASSETTE_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_CASSETTE_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  case WaitTouch:
    lcd_put_line(1, get_text(STR_WAIT_TOUCH), SCR_ALIGN_CENTER);
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      //plunger_go_down();
      //micro_state = Down;
      //Undefined behavior: Inpossible state
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    {
      if(!work_area)
      {
        if((int)plunger_position() > slow_offset)
        {
          motor_change_rate(SLOW_RATE);
          set_zero();
          work_area = 1;
        }
      }
      else
      { //work_area
        if(touch_detect())
        {
          touch_count ++;
          if(touch_count >= TOUCH_COUNT)
          {
            set_touch_position();
            micro_state = CalibrationForce;
          }
        }
        else
          touch_count = 0;
      }
    }
    break;

  case CalibrationForce:
    //lcd_put_line(1, get_text(STR_WAIT_TOUCH), SCR_ALIGN_CENTER);
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      calc_calibration_results();
      plunger_go_down();
      micro_state = Down;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(!calibration_finish)
    {
      if(calibration_force_riched())
      {
        calibration_finish = 1;
        plunger_stop();
      }
    }
    break;

  case Down:
    lcd_put_line(1, get_text(STR_PLUNGER_GO_HOME), SCR_ALIGN_CENTER);
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      micro_state = StartState;
      return EVENT_HANDLER_DONE;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  default:
    break;
  }

  return 0;
}
/*----------------------------------------------------------------------------*/
static int measuring_handler(void *data, event_t evt, int param1, void *param2)
{

  typedef enum
  {
    StartState,
    PlungerCatch,
    CassetteCatch,
    WaitTouch,
    WaitDestruction,
    Down,
    WaitProtocol
  } MICRO_STATE;

  static MICRO_STATE micro_state = StartState;
  static timeout_t timeout = {0, 0};
  static int cursor = 0;
  static int detect;
  static int work_area;
  static int slow_offset;
  static int detect_count = 0;
  static int no_particle;
  int res;

  (void) data; //Prevent unused warning
  switch(micro_state)
  {
  case StartState:
    if(start_flag)
    {
      const mesh_t *current_mesh;
      plunger_go_down();
      micro_state = PlungerCatch;
      force = decimal32_init(0,0);
      size = decimal32_init(0,0);
      detect = 0;
      work_area = 0;
      //Calculate slow offset
      current_mesh = mesh();
      if(current_mesh == 0)
        slow_offset = MAX_INT_32;
      else
      {
        slow_offset = (int)touch_position() - um2steps(current_mesh->max) - 500;//(255 - SLOW_RATE);
      }
      detect_count = 0;
      no_particle = 0;
    }
    else
    {

      if(evt == KEY_PRESS_EVENT && param1 == KEY_STOP)
      {
        micro_state = StartState;
        return USER_BREAK;
      }

      if(!timeout.timeout || timeout_riched(&timeout, sys_tick_count()))
      {
        cursor = !cursor;
        timeout_set(&timeout, 800, sys_tick_count());
      }
      lcd_put_line(1, cursor ? get_text(STR_PRESS_START) : " ", SCR_ALIGN_CENTER);
    }
    break;

  case PlungerCatch:
    //lcd_put_line(1, get_text(STR_PLUNGER_GO_HOME), SCR_ALIGN_CENTER);
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      cassete_goto_position(cassette_position() + 1);
      micro_state = CassetteCatch;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  case CassetteCatch:
    //lcd_put_line(1, get_text(STR_CASSETTE_GO_HOME), SCR_ALIGN_CENTER);
    res = handler_call(&cassette_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      //set_zero();
      plunger_go_up();
      micro_state = WaitTouch;
    }
    else
    if(res == CASSETTE_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_CASSETTE_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  case WaitTouch:
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      plunger_go_down();
      micro_state = Down;
      break;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    {
      if(!work_area)
      {
        lcd_put_line(1, get_text(STR_WAIT_TOUCH), SCR_ALIGN_CENTER);
        if((int)plunger_position() >= slow_offset)
        {
          motor_change_rate(SLOW_RATE);
          set_zero();
          work_area = 1;
        }
      }
      else
      if(!no_particle) //if no particle - prevent continue detect
      { //work_area
        if(touch_detect())
        {
          detect_count ++;

          if(detect_count >= TOUCH_COUNT)
          {
            if(motor_rate() > SLOW_RATE)
              motor_change_rate(SLOW_RATE);
            if(set_size_postion())
            {
              micro_state = WaitDestruction;
              detect_count = 0;
            }
            else
            {
              plunger_stop();
              no_particle = 1;
            }
          }
        }
        else
        {
          detect_count = 0;
          if(plunger_position() > touch_position()) //No tablet
          {
            protocol_push_no_particle();
            plunger_stop();
            no_particle = 1;
          }
        }
      } //work_area
    }
    break;

  case WaitDestruction:
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      plunger_go_down();
      micro_state = Down;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(!detect)
    {
      if(destruction_detect())
      {
        detect_count ++;
        if(detect_count >= DESTRUCTION_DETECT_COUNT)
        {
          detect = 1;
          set_destruction_position();
          plunger_stop();
        }
      }
      else
        detect_count = 0;
    }
    break;

  case Down:
    //lcd_put_line(1, get_text(STR_PLUNGER_GO_HOME), SCR_ALIGN_CENTER);
    if(no_particle)
      lcd_put_line(1, get_text(STR_NO_PARTICLE), SCR_ALIGN_CENTER);

    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      micro_state = WaitProtocol;
      break;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  case WaitProtocol:
    lcd_put_line(1, get_text(STR_WAIT_PROTOCOL), SCR_ALIGN_CENTER);
    if(is_measure_data_transmitted())
    {
      micro_state = StartState;
      return EVENT_HANDLER_DONE;
    }
    break;

  default:
    break;
  }

  return 0;
}
/*----------------------------------------------------------------------------*/
static int done_handler(void *data, event_t evt, int param1, void *param2)
{
  typedef enum
  {
    StartState,
    PlungerCatch,
    CassetteCatch
  } MICRO_STATE;

  static MICRO_STATE micro_state = StartState;
  int res;

  (void) data; //Prevent unused warning
  switch(micro_state)
  {
  case StartState:
    plunger_go_down();
    micro_state = PlungerCatch;
    break;

  case PlungerCatch:
    lcd_put_line(1, get_text(STR_PLUNGER_GO_HOME), SCR_ALIGN_CENTER);
    res = handler_call(&plunger_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      cassete_goto_position(CASSETTE_NULL_POSITION);
      micro_state = CassetteCatch;
    }
    else
    if(res == PLUNGER_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == PLUNGER_END_POS_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_PLUNGER_END_KEY), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  case CassetteCatch:
    lcd_put_line(1, get_text(STR_CASSETTE_GO_HOME), SCR_ALIGN_CENTER);
    res = handler_call(&cassette_handler, evt, param1, param2);
    if(res == EVENT_HANDLER_DONE)
    {
      micro_state = StartState;
      return EVENT_HANDLER_DONE;
    }
    else
    if(res == CASSETTE_TIMEOUT_ERROR)
    {
      micro_state = StartState;
      show_message(get_text(STR_ERROR), get_text(STR_CASSETTE_TIMEOUT), 0);
      return EVENT_HANDLER_FAILED;
    }
    else
    if(res == USER_BREAK)
    {
      micro_state = StartState;
      show_message(get_text(STR_WARNING), get_text(STR_POS_OPERATION_BREAK), 0);
      return EVENT_HANDLER_FAILED;
    }
    break;

  default:
    break;
  }

  return 0;
}
/*----------------------------------------------------------------------------*/
