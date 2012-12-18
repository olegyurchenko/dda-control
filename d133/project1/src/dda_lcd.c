/*----------------------------------------------------------------------------*/
/**
* @pkg dda_lcd
*/
/**
* DDA functions to operate LCD.
*
* Controll LCD ВС1602Е compatible controller.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 06.12.2012  8:30:28<br>
* @pkgdoc dda_lcd
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_lcd.h"
#include <stm32f10x.h>
#include <sys_timer.h>
#include <sys_sheduler.h>
#include <dda_clib.h>
//#include <console.h>
/*----------------------------------------------------------------------------*/
#define LCD_RS    GPIO_Pin_1 //PA1
#define LCD_RW    GPIO_Pin_2 //PA2
#define LCD_E     GPIO_Pin_3 //PA3
#define LCD_D4    GPIO_Pin_4 //PA4
#define LCD_D5    GPIO_Pin_5 //PA5
#define LCD_D6    GPIO_Pin_6 //PA6
#define LCD_D7    GPIO_Pin_7 //PA7
/*----------------------------------------------------------------------------*/
#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 2
#define MAX_SCROLLTEXT_LENGTH 48
/*----------------------------------------------------------------------------*/
struct SCROLLED_TEXT
{
  char caption[MAX_SCROLLTEXT_LENGTH];
  int size;
  int hscroll;
  int line;
  int start_pos;
  int hsize;
  int times;
};
typedef struct SCROLLED_TEXT SCROLLED_TEXT;
/*----------------------------------------------------------------------------*/
#define MAX_SCROLL_COUNT 4
static SCROLLED_TEXT scrolls[MAX_SCROLL_COUNT];
static int scroll_count = 0;
#define HSCROLL_TIME 500
#define HSCROLL_TIMES 3
/*----------------------------------------------------------------------------*/
static char lcd_buffer[DISPLAY_HEIGHT * DISPLAY_WIDTH];
static char lcd_old[DISPLAY_HEIGHT * DISPLAY_WIDTH];
static void _hi_init();
static void lcd_timer(void *);
/*----------------------------------------------------------------------------*/
static void RS(int on)
{
  if(on)
    GPIO_SetBits(GPIOA, LCD_RS);
  else
    GPIO_ResetBits(GPIOA, LCD_RS);
}
/*----------------------------------------------------------------------------*/
static void RW(int on)
{
  if(on)
    GPIO_SetBits(GPIOA, LCD_RW);
  else
    GPIO_ResetBits(GPIOA, LCD_RW);
}
/*----------------------------------------------------------------------------*/
static void E(int on)
{
  if(on)
    GPIO_SetBits(GPIOA, LCD_E);
  else
    GPIO_ResetBits(GPIOA, LCD_E);
}
/*----------------------------------------------------------------------------*/
static void DATA(uint8_t d)
{
  uint32_t port;
  port = GPIO_ReadOutputData(GPIOA);
  port &= ~(LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7);
  port |= (d & 1) ? LCD_D4 : 0;
  port |= (d & 2) ? LCD_D5 : 0;
  port |= (d & 4) ? LCD_D6 : 0;
  port |= (d & 8) ? LCD_D7 : 0;
  GPIO_Write(GPIOA, port);
}
/*----------------------------------------------------------------------------*/
static void lcd_sb()
{
  E(1);
  sys_usleep(15);
  E(0);
  sys_usleep(5);
}
/*----------------------------------------------------------------------------*/
static void lcd_write8(uint8_t b)
{
  DATA(b >> 4);
  lcd_sb();
}
/*----------------------------------------------------------------------------*/
static void lcd_write4(uint8_t b)
{
  DATA(b >> 4);
  lcd_sb();
  DATA(b & 0x0f);
  lcd_sb();
  sys_usleep(100);
  DATA(0);
}
/*----------------------------------------------------------------------------*/
static void _init()
{
  RS(0);
  RW(0);
  DATA(0);
  lcd_write8(0x30); //Function set ( Interface is 8 bits long. )
  sys_sleep(10);    //Wait for more than 4.1 ms
  lcd_write8(0x30); //Function set ( Interface is 8 bits long. )
  sys_usleep(500);  //Wait for more than 100 μs
  lcd_write8(0x30); //Function set ( Interface is 8 bits long. )
  sys_usleep(500);
  lcd_write8(0x20); //Function set ( Set interface to be 4 bits long. ) Interface is 8 bits in length.
  lcd_write4(0x2c); // 0x28 - 5x8 font Function set ( Interface is 4 bits long. Specify
  //the number of display lines and character font. )
  //The number of display lines and character font can not be changed after this point.
  lcd_write4(0x08); //Display off
  lcd_write4(0x01); //Display clear
  sys_sleep(5);
  lcd_write4(0x06); //Entry mode set (moving direction -I, enable the shift of entire display (0)
  lcd_write4(0x0c);
}
/*----------------------------------------------------------------------------*/
void lcd_init()
{
  GPIO_InitTypeDef gpio;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  gpio.GPIO_Pin = LCD_RS | LCD_RW | LCD_E | LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &gpio);

  sys_sleep(800); /* Wait a bit after power-up */
  _init();
  _hi_init();
}
/*----------------------------------------------------------------------------*/
void _lcd_putch(char b)
{
  RS(1);
  RW(0);
  lcd_write4(b);
  RS(0);
}
/*----------------------------------------------------------------------------*/
void _lcd_home()
{
  lcd_write4(0x02);
  sys_sleep(5);
}
/*----------------------------------------------------------------------------*/
void _lcd_clear()
{
  lcd_write4(0x01);
  sys_sleep(5);
}
/*----------------------------------------------------------------------------*/
void _lcd_move(char n)
{
  lcd_write4(n | 0x80);
}
/*----------------------------------------------------------------------------*/
static void lcd_out(int pos, const char *text, int size)
{
  if(pos >= DISPLAY_WIDTH)
  {
    pos -= DISPLAY_WIDTH;
    pos += 0x40;
  }
  _lcd_move(pos);
  while(size)
  {
    _lcd_putch(*text);
    text ++;
    size --;
  }
}
/*----------------------------------------------------------------------------*/
//Hi level
/*----------------------------------------------------------------------------*/
static void _hi_init()
{
  memset(lcd_old, 0, sizeof(lcd_old));
  lcd_clear();
  sheduler_add(lcd_timer, 0, 100, 100);
}
/*----------------------------------------------------------------------------*/
static void intern_lcd_put_char(int x, int y, int ch)
{
  if(x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT && x >= 0 && y >= 0)
    lcd_buffer[y * DISPLAY_WIDTH + x] = (char)ch;
}
/*----------------------------------------------------------------------------*/
static void intern_lcd_put_line(int y, const char *txt, SCR_ALIGN align)
{
  int i, l, size;

  if(y < 0 || y >= DISPLAY_HEIGHT)
    return;
  size = strlen(txt);
  l = 0;
  if(align == SCR_ALIGN_CENTER)
    l = (DISPLAY_WIDTH - size) / 2;
  else
  if(align == SCR_ALIGN_RIGHT)
    l = DISPLAY_WIDTH - size;

  for(i = 0; i < l; i++)
    intern_lcd_put_char(i, y, ' ');
  for(; *txt && i < DISPLAY_WIDTH; i++)
    intern_lcd_put_char(i, y, *(txt ++));
  for(;i < DISPLAY_WIDTH; i++)
    intern_lcd_put_char(i, y, ' ');
}
/*----------------------------------------------------------------------------*/
int get_lcd_width()
{
  return DISPLAY_WIDTH;
}
/*----------------------------------------------------------------------------*/
int get_lcd_height()
{
  return DISPLAY_HEIGHT;
}
/*----------------------------------------------------------------------------*/
void lcd_put_line(int y, const char *txt, SCR_ALIGN align)
{
  intern_lcd_put_line(y, txt, align);
}
/*----------------------------------------------------------------------------*/
void lcd_add_line(const char *txt, SCR_ALIGN align)
{
  int i;
  for(i = DISPLAY_HEIGHT - 1; i > 0; i--)
    memcpy(&lcd_buffer[(i - 1) * DISPLAY_WIDTH], &lcd_buffer[i * DISPLAY_WIDTH], DISPLAY_WIDTH);

  for(i = 0; i < scroll_count; i++)
  {
    scrolls[i].line --;
    if(scrolls[i].line < 0)
    {
      int j;
      //Remove scroll
      for(j = i + 1; j < scroll_count; j++)
        scrolls[j - 1] = scrolls[j];
      scroll_count --;
      i --;
    }
  }

  intern_lcd_put_line(DISPLAY_HEIGHT - 1, txt, align);
}
/*----------------------------------------------------------------------------*/
void lcd_clear()
{
  memset(lcd_buffer, ' ', sizeof(lcd_buffer));
  scroll_count = 0;
}
/*----------------------------------------------------------------------------*/
void lcd_put_char(int x, int y, int ch)
{
  intern_lcd_put_char(x, y, ch);
}
/*----------------------------------------------------------------------------*/
int lcd_get_char(int x, int y)
{
  if(x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT && x >= 0 && y >= 0)
    return lcd_buffer[y * DISPLAY_WIDTH + x];
  return 0;
}
/*----------------------------------------------------------------------------*/
void lcd_update()
{
  int i;
  for(i = 0; i < DISPLAY_HEIGHT; i++)
  {
    if(memcmp(&lcd_buffer[i * DISPLAY_WIDTH], &lcd_old[i * DISPLAY_WIDTH], DISPLAY_WIDTH))
    {
      //char buff[DISPLAY_WIDTH + 1];
      //memcpy(buff, &lcd_buffer[i * DISPLAY_WIDTH], DISPLAY_WIDTH);
      //buff[DISPLAY_WIDTH] ='\0';
      //console_printf("buf='%s'\r\n", buff);
      lcd_out(i * DISPLAY_WIDTH, &lcd_buffer[i * DISPLAY_WIDTH], DISPLAY_WIDTH);
      memcpy(&lcd_old[i * DISPLAY_WIDTH], &lcd_buffer[i * DISPLAY_WIDTH], DISPLAY_WIDTH);
    }
  }
}
/*----------------------------------------------------------------------------*/
static void scrolled_text_init(const char *caption, SCROLLED_TEXT *s)
{
  int size;
  size = strlen(caption);
  if(size > MAX_SCROLLTEXT_LENGTH)
    size = MAX_SCROLLTEXT_LENGTH;

  s->size = 0;
  s->hscroll = 0;
  s->start_pos = 0;
  s->hsize = 0;
  s->times = 0;

  memcpy(s->caption, caption, size);
  s->size = size;
}
/*----------------------------------------------------------------------------*/
void lcd_add_scroll_text(int y, int x, int hsize, const char *txt)
{
  SCROLLED_TEXT *t = 0;
  int i;

  for(i = 0; i < scroll_count; i++)
  {
    if(scrolls[i].line == y && scrolls[i].start_pos == x)
    {
      t = &scrolls[i];
      break;
    }
  }

  if(t == 0)
  {
    if(scroll_count + 1 >= MAX_SCROLL_COUNT)
      return;

    t = &scrolls[scroll_count ++];
  }

  scrolled_text_init(txt, t);

  t->line = y;
  t->start_pos = x;
  t->hsize = hsize;

  for(i = 0; i < scroll_count; i++)
  {
    scrolls[i].times = 0;
    scrolls[i].hscroll = 0;
  }
}
/*----------------------------------------------------------------------------*/
static void lcd_hscroll_text(SCROLLED_TEXT *s)
{
  char *p;
  int size, x;
  if(s->size <= s->hsize)
    s->hscroll = 0;
  else
  if(s->hscroll > s->size - s->hsize)
  {
    s->times ++;
    if(s->times < HSCROLL_TIMES)
      return;
    s->hscroll = 0;
    s->times = 0;
  }

  p = &s->caption[s->hscroll];
  size = s->hsize;
  for(x = s->start_pos; x < DISPLAY_WIDTH && size; x++, size --)
    intern_lcd_put_char(x, s->line, *p ? *(p ++) : ' ');
  if(s->size <= s->hsize)
    return;

  if(!s->hscroll)
  {
    s->times ++;
    if(s->times < HSCROLL_TIMES)
      return;
  }

  s->times = 0;
  s->hscroll ++;
}
/*----------------------------------------------------------------------------*/
static void lcd_timer(void *v)
{
  static unsigned last_time = 0;
  unsigned t;
  (void)v; //Q_UNUSED
  t = sys_tick_count();
  if(last_time + HSCROLL_TIME < t || last_time > t)
  {
    int i;

    last_time = t;
    for(i = 0; i < scroll_count; i++)
      lcd_hscroll_text(&scrolls[i]);

  }
  lcd_update();
}
/*----------------------------------------------------------------------------*/
