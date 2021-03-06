/*----------------------------------------------------------------------------*/
/**
* @pkg menu
*/
/**
* DDA menu module.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 18.12.2012 11:24:40<br>
* @pkgdoc menu
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "menu.h"
#include <dda_key.h>
#include <dda_lcd.h>
#include <dda_clib.h>
#include <sys_timer.h>
/*----------------------------------------------------------------------------*/
#define DEBUG
#ifdef DEBUG
#include <console.h>
#endif
/*----------------------------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif
/*----------------------------------------------------------------------------*/
static MENU_ITEM root_item;

#define SELECTION_CHAR_LEFT '>'
#define SELECTION_CHAR_RIGHT '<'

#define MENU_CHECK_ITEM 1
#define MENU_RADIO_ITEM 2
/*----------------------------------------------------------------------------*/
static struct MENU_STATE state;
static int event_handler(void*, event_t evt, int param1, void *param2);
void draw_menu_item(int y, int index_width, MENU_ITEM *m);
void draw_menu_selection(int y, int index_width, MENU_ITEM *m, int sel);
int hscroll_menu_item(int y, int index_width, MENU_ITEM *n);
/*----------------------------------------------------------------------------*/
typedef struct
{
  const char *caption;
  int code_width;
  int menu_code_width;
  MENU_ITEM *items;
  event_handler_t handler;

} CODE_SELECT_STATE;
/*----------------------------------------------------------------------------*/
void menu_item_init(const char *caption, event_handler_t handler, MENU_ITEM *dst)
{
  dst->flag = 0;
  dst->caption = "";
  dst->handler = handler;
  dst->data = NULL;
  dst->parent = NULL;
  dst->next = NULL;
  dst->prev = NULL;
  dst->childs = NULL;
  dst->hstart = 0;
  dst->hscroll = 0;
  dst->caption_size = 0;
  if(caption != NULL && *caption)
  {
    dst->caption = caption;
    dst->caption_size = strlen(caption);
  }
}
/*----------------------------------------------------------------------------*/
int menu_item_add(MENU_ITEM *m, MENU_ITEM *n)
{
  int index = 1;
  while(m->next != NULL)
  {
    index ++;
    m = m->next;
  }
  n->prev = m;
  n->next = NULL;
  m->next = n;
  n->parent = m->parent;
  n->menu_index = index + 1;
  return index;
}
/*----------------------------------------------------------------------------*/
int menu_item_add_child(MENU_ITEM *m, MENU_ITEM *n)
{
  if(m->childs == NULL)
  {
    m->childs = n;
    n->next = NULL;
    n->prev = NULL;
    n->parent = m;
    n->menu_index = 1;
    return 0;
  }

  return menu_item_add(m->childs, n);
}
/*----------------------------------------------------------------------------*/
static void init_menu(MENU_ITEM *m)
{
  int i = 0, max_index = 0;
  MENU_ITEM *n;
  char buf[16];

  if(m == NULL)
    m = &root_item;


  n = m->childs;

  while(n != NULL)
  {
    if(n->menu_index > max_index)
      max_index = n->menu_index;
    n = n->next;
    i ++;
  }

  state.root = m;
  state.pos = 0;
  state.size = i;

  state.index_width = snprintf(buf, sizeof(buf), "%d ", max_index);
}
/*----------------------------------------------------------------------------*/
void start_menu(MENU_ITEM *m)
{
  init_menu(m);
  push_event_handler();
  set_event_handler(event_handler, NULL);
}
/*----------------------------------------------------------------------------*/
static void draw_selection(int active)
{
  int i, index = -1, j = 0;
  MENU_ITEM *n;
  int height;

  height = lcd_height();

  if(active)
  {
    index = state.pos;

    if(state.pos >= height)
      index = height - 1;
  }

  if(state.pos >= height)
    j = state.pos - height + 1;

  n = state.root->childs;
  while(n != NULL && j)
  {
    n = n->next;
    j --;
  }

  for(i = 0; i < height && n != NULL; i++)
  {
    draw_menu_selection(i, state.index_width, n, i == index);
    n = n->next;
  }
}
/*----------------------------------------------------------------------------*/
static int hscroll_menu()
{
  int y, j = 0;
  MENU_ITEM *n;
  int height;

  height = lcd_height();

  y = state.pos;

  if(state.pos >= height)
    y = height - 1;

  j = state.pos;

  n = state.root->childs;
  while(n != NULL && j)
  {
    n = n->next;
    j --;
  }

  if(n != NULL)
    return hscroll_menu_item(y, state.index_width, n);

  return 0;
}
/*----------------------------------------------------------------------------*/
static void draw_menu()
{
  int i = 0;
  MENU_ITEM *n;
  int height;

  height = lcd_height();
  lcd_clear();

  if(state.pos >= height)
    i = state.pos - height + 1;

  n = state.root->childs;
  while(n != NULL && i)
  {
    n = n->next;
    i --;
  }

  for(i = 0; i < height && n != NULL; i++)
  {
    draw_menu_item(i, state.index_width, n);
    n = n->next;
  }

  draw_selection(1);
}
/*----------------------------------------------------------------------------*/
void set_menu_pos(int index)
{
  if(index < 0 || index >= state.size || index == state.pos)
    return;
  state.pos = index;
  draw_menu();
}
/*----------------------------------------------------------------------------*/
static void menu_click()
{
  int i;
  MENU_ITEM *n;
  int result = MENU_CONTINUE;

  i = state.pos;
  n = state.root->childs;
  while(n != NULL && i)
  {
    n = n->next;
    i --;
  }

  pop_event_handler();
  if(n->handler != NULL)
    result = n->handler(n->data, MENU_EVENT, n->menu_index, n);

  if(result == MENU_OK)
    return;


  push_event_handler();
  set_event_handler(event_handler, NULL);
  if(result == MENU_CANCEL)
    return;

  init_menu(n);
  if(n->handler != NULL)
  {
    int position = -1;
    n->handler(n->data, MENU_GET_POSITION, 0, &position);
    if(position >= 0)
      set_menu_pos(position);
  }

  draw_menu();
}
/*----------------------------------------------------------------------------*/
static int find_childs(MENU_ITEM *parent, int index)
{
  MENU_ITEM *n;
  int i = 0;

  n = parent->childs;
  while(n != NULL)
  {
    if(n->menu_index == index)
      return i;
    n = n->next;
    i ++;
  }

  return i;
}
/*----------------------------------------------------------------------------*/
void set_menu_pos_by_index(int index)
{
  if(state.root != NULL)
    set_menu_pos(find_childs(state.root, index));
}
/*----------------------------------------------------------------------------*/
void draw_menu_item(int y, int index_width, MENU_ITEM *n)
{
  char buffer[16], fmt[16];
  const char *p;
  int j;
  int width;

  width =  lcd_width();

  buffer[0] = '\0';
  if(index_width)
  {
    sprintf(fmt,"%%-%ud", index_width);
    sprintf(buffer, fmt, n->menu_index);
    //console_printf("`%s` `%s`\r\n", fmt, buffer); //!!!!!!!!!!!!!!!!!!!!!!!
  }
  p = buffer;
  for(j = MENU_ITEM_INDENT; j < width-SELECTION_CHAR_COUNT && *p; j++, p++)
    lcd_put_char(j, y, *p);

  p = n->caption;
  for(; j < width-SELECTION_CHAR_COUNT && *p; j++, p++)
    lcd_put_char(j, y, *p);
  n->hscroll = 0;
}
/*----------------------------------------------------------------------------*/
int hscroll_menu_item(int y, int index_width, MENU_ITEM *n)
{
  const char *p;
  int j;
  int width;

  width =  lcd_width();
  if(n->caption_size <= width - SELECTION_CHAR_COUNT - MENU_ITEM_INDENT - index_width)
    return 0;

  if(n->hscroll < 0)
    n->hscroll = 0;

  if(n->hscroll > n->caption_size - (width - SELECTION_CHAR_COUNT - MENU_ITEM_INDENT - index_width - n->hstart - 1))
    n->hscroll = 0;

  p = &n->caption[n->hscroll + n->hstart];

  for(j = MENU_ITEM_INDENT + index_width + n->hstart; j < width - SELECTION_CHAR_COUNT; j++)
    lcd_put_char(j, y, *p ? *(p++) : ' ');

  if(!n->hscroll ++)
    return 2;
  return 1;
}
/*----------------------------------------------------------------------------*/
void draw_menu_selection(int y, int index_width, MENU_ITEM *n, int sel)
{
  int caption_size, j;
  int width;

  width =  lcd_width();

  caption_size = n->caption_size;
  if(caption_size > width - SELECTION_CHAR_COUNT - MENU_ITEM_INDENT - index_width)
    caption_size = width - SELECTION_CHAR_COUNT - MENU_ITEM_INDENT - index_width;

  for(j = 0; j < MENU_ITEM_INDENT; j++)
    lcd_put_char(j, y, ' ');

  for(j = MENU_ITEM_INDENT + index_width + caption_size; j < width; j++)
    lcd_put_char(j, y, ' ');

  if(sel)
  {
    int m = 0;
    for(j = MENU_ITEM_INDENT - SELECTION_CHAR_COUNT; j < MENU_ITEM_INDENT; j++)
      lcd_put_char(j, y, SELECTION_CHAR_LEFT);
    for(j = MENU_ITEM_INDENT + index_width + caption_size; j < width && m < SELECTION_CHAR_COUNT; j++, m++)
      lcd_put_char(j, y, SELECTION_CHAR_RIGHT);
  }
}
/*----------------------------------------------------------------------------*/
static int event_handler(void* m, event_t evt, int param1, void *param2)
{
  static unsigned iddle_time;
  static unsigned selection_time;

  int index = -1;
  static int cursor;

  (void) m;   //Prevent unused warning
  (void) param2;

  switch(evt)
  {
  case MODE_SET_EVENT:
    if(param1)
    {
      draw_menu();
      iddle_time = sys_tick_count();
    }
    break;

  case IDLE_EVENT:
    if(iddle_time + MENU_CURSOR_TIME < sys_tick_count())
    {
      cursor = !cursor;
      draw_selection(cursor);
      iddle_time = sys_tick_count();
    }

    if(selection_time + MENU_HSCROLL_TIME < sys_tick_count())
    {
      switch(hscroll_menu())
      {
      case 0:
        selection_time = sys_tick_count();
        break;
      case 1:
        selection_time = sys_tick_count();
        break;
      case 2:
        selection_time = sys_tick_count() + MENU_HSCROLL_PAUSE;
        break;
      }
    }
    break;

  case KEY_PRESS_EVENT:
    switch(param1)
    {
    case KEY_OK:
      menu_click();
      break;

    case KEY_STOP: //Cancel
      pop_event_handler();
      break;

    case KEY_UP:
      if(state.pos)
      {
        state.pos --;
        draw_menu();
        cursor = 1;
        selection_time = sys_tick_count() + MENU_HSCROLL_PAUSE - MENU_HSCROLL_TIME;
      }
      break;

    case KEY_DOWN:
      if(state.pos < state.size - 1)
      {
        state.pos ++;
        draw_menu();
        cursor = 1;
        selection_time = sys_tick_count() + MENU_HSCROLL_PAUSE - MENU_HSCROLL_TIME;
      }
      break;

    case KEY_MENU:
      if(state.root->parent != NULL)
      {
        MENU_ITEM *n;
        index = state.pos;
        n = state.root->childs;
        while(n != NULL && index)
        {
          n = n->next;
          index --;
        }

        n = n->parent;
        index = 0;
        while(n->prev != NULL)
        {
          n = n->prev;
          index ++;
        }

        init_menu(state.root->parent);
        if(state.size > index)
          state.pos = index;
        draw_menu();
        index = -1;
      }
      break;

    default:
      break;
    }

  default:
    break;
  }

  return 0;
}
/*----------------------------------------------------------------------------*/

