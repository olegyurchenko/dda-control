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
#ifndef MENU_H_1355822680
#define MENU_H_1355822680
/*----------------------------------------------------------------------------*/
#include <event.h>

#define MENU_CURSOR_TIME 600
#define MENU_HSCROLL_TIME 500
#define MENU_HSCROLL_PAUSE 1300

#define SELECTION_CHAR_COUNT 2
#define MENU_ITEM_INDENT 2
/*----------------------------------------------------------------------------*/
#define MENU_CAPTION_SIZE 32
/*----------------------------------------------------------------------------*/
struct MENU_ITEM
{
  int menu_index;
  const char *caption;
  event_handler_t handler;
  void *data;
  short flag;
  short caption_size;
  short hstart;
  short hscroll;
  struct MENU_ITEM *parent;
  struct MENU_ITEM *next;
  struct MENU_ITEM *prev;
  struct MENU_ITEM *childs;
};
typedef struct MENU_ITEM MENU_ITEM;
 /*----------------------------------------------------------------------------*/
struct MENU_STATE
{
  MENU_ITEM *root;
  int size;
  int pos;
  int index_width;
};

typedef struct MENU_STATE MENU_STATE;
/*----------------------------------------------------------------------------*/
#define MENU_OK 1
#define MENU_CONTINUE 0
#define MENU_CANCEL (-1)
#define MENU_ABORT (-2)
/*----------------------------------------------------------------------------*/
#define MENU_SCROLL_UP_EVENT (event_t)(USER_EVENT + 100)
#define MENU_SCROLL_DOWN_EVENT (event_t)(USER_EVENT + 101)
#define MENU_EDIT_OK_EVENT (event_t)(USER_EVENT + 102)
#define MENU_OK_EVENT (event_t)(USER_EVENT + 103)
#define MENU_WAIT_ITEMS_EVENT (event_t)(USER_EVENT + 104)
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void menu_item_init(const char *caption, event_handler_t handler, MENU_ITEM *dst);

int menu_item_add(MENU_ITEM *m, MENU_ITEM *n);
int menu_item_add_child(MENU_ITEM *m, MENU_ITEM *n);

void start_menu(MENU_ITEM *m);
void set_menu_pos(int index);
void set_menu_pos_by_index(int index);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*MENU_H_1355822680*/

