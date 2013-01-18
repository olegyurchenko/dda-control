/*----------------------------------------------------------------------------*/
/**
* @pkg view
*/
/**
* Text view via LCD.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 18.01.2013  9:55:01<br>
* @pkgdoc view
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "view.h"
#include <event.h>
#include <sys_timer.h>
#include <dda_clib.h>
#include <dda_key.h>
#include <dda_work_mode.h>
/*----------------------------------------------------------------------------*/
#define UP_ARROW_CHR 0xd9
#define DOWN_ARROW_CHR 0xda
#define BLOCK_CHR 0xff
/*----------------------------------------------------------------------------*/
#define CURSOR_TIME 600
#define KEY_DELAY_TIME 800
#define KEY_REPEAT_TIME 300
/*----------------------------------------------------------------------------*/
static int view_handler(void*, event_t evt, int param1, void *param2);
/*----------------------------------------------------------------------------*/
typedef struct
{
  view_get_t get;
  void *data;
  int string_count;
  int index;
  int cursor_active;
  timeout_t timeout;
  int active_key;
  timeout_t key_timeout;
} view_data_t;
/*----------------------------------------------------------------------------*/
static view_data_t view_data;
/*----------------------------------------------------------------------------*/
void viewer(view_get_t get, void *data, int string_count)
{
  view_data.get = get;
  view_data.data = data;
  view_data.string_count = string_count;
  view_data.index = 0;
  view_data.cursor_active = 1;
  view_data.active_key = 0;
  timeout_set(&view_data.timeout, CURSOR_TIME, sys_tick_count());
  push_event_handler();
  set_event_handler(view_handler, 0);
}
/*----------------------------------------------------------------------------*/
static void draw_cursor()
{
#if 0
  lcd_put_char(0, 0, view_data.cursor_active ? UP_ARROW_CHR : ' ');
  lcd_put_char(0, 1, view_data.cursor_active ? DOWN_ARROW_CHR : ' ');
#else
  int index, y = 0, height;
  height = lcd_height();
  index = view_data.index;
  if((index >= view_data.string_count - height + 1) && (index >= height - 1))
  {
    index -= height - 1;
    y += view_data.index - index;
  }
  lcd_put_char(0, y, view_data.cursor_active ? BLOCK_CHR : ' ');
#endif
}
/*----------------------------------------------------------------------------*/
static void display()
{
  int width, height, y, index;
  width = lcd_width();
  height = lcd_height();
  lcd_clear();
  index = view_data.index;

  if((index >= view_data.string_count - height + 1) && (index >= height - 1))
    index -= height - 1;

  for(y = 0; y < height; y++)
  {
    char *str = 0;
    int size = 0, x = 1;
    SCR_ALIGN align = SCR_ALIGN_LEFT;
    if(index < view_data.string_count)
      view_data.get(view_data.data, index, &str, &align);
    if(str != 0)
    {
      size = strlen(str);
      if(align == SCR_ALIGN_CENTER)
        x = 1 + (width - size) / 2;
      else
        if(align == SCR_ALIGN_CENTER)
          x = 1 + width - size;
      if(size >= width && index == view_data.index)
      {
        lcd_add_scroll_text(y, 1, width - 1, str);
      }
      else
      {
        if(x < 1)
          x = 1;
        for(; x < width && *str; x++)
        {
          lcd_put_char(x, y, *str);
          str ++;
        }
      }
    }
    index ++;
  }

  draw_cursor();
}
/*----------------------------------------------------------------------------*/
static int view_handler(void *data, event_t evt, int param1, void *param2)
{
  (void) data; //Prevent unused warning
  (void) param1;
  (void) param2;
  switch(evt)
  {
  case IDLE_EVENT:
    if(timeout_riched(&view_data.timeout, sys_tick_count()))
    {
      timeout_set(&view_data.timeout, CURSOR_TIME, sys_tick_count());
      view_data.cursor_active = !view_data.cursor_active;
      draw_cursor();
    }

    if(view_data.active_key == KEY_DOWN && timeout_riched(&view_data.key_timeout, sys_tick_count()))
    {
      if(view_data.index >= view_data.string_count - 1)
        view_data.active_key = 0;
      else
      {
        view_data.index ++;
        timeout_set(&view_data.key_timeout, KEY_REPEAT_TIME, sys_tick_count());
        display();
      }
    }

    if(view_data.active_key == KEY_UP && timeout_riched(&view_data.key_timeout, sys_tick_count()))
    {
      if(!view_data.index)
        view_data.active_key = 0;
      else
      {
        view_data.index --;
        timeout_set(&view_data.key_timeout, KEY_REPEAT_TIME, sys_tick_count());
        display();
      }
    }
    break;

  case MODE_SET_EVENT:
    if(!param1) //Mode exit
      return 0;
    display();
    break;

  case KEY_PRESS_EVENT:
    if(param1 == KEY_DOWN)
    {
      if(view_data.index < view_data.string_count - 1)
      {
        view_data.active_key = param1;
        view_data.index ++;
        timeout_set(&view_data.key_timeout, KEY_DELAY_TIME, sys_tick_count());
        display();
      }
    }

    if(param1 == KEY_UP)
    {
      if(view_data.index)
      {
        view_data.active_key = param1;
        view_data.index --;
        timeout_set(&view_data.key_timeout, KEY_DELAY_TIME, sys_tick_count());
        display();
      }
    }

    if(param1 == KEY_STOP || param1 == KEY_MENU)
    {
      start_work_menu();
    }

    if(param1 == KEY_OK)
    {
      pop_event_handler();
    }
    break;

  case KEY_RELEASE_EVENT:
    view_data.active_key = 0;
    break;

  default:
    break;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
