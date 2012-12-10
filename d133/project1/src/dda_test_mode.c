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
/*----------------------------------------------------------------------------*/
typedef enum
{
  Idle = -1,
  KeyTest,
  AdcTest
} TestMode;

TestMode mode = Idle;
static int sheduler_id = -1;
static int test_times;
/*----------------------------------------------------------------------------*/
static void set_mode(TestMode m);
static void test_timer(void *);
static void key_test();
static void adc_test();
/*----------------------------------------------------------------------------*/
void test_mode_init()
{
  set_mode(AdcTest);
}
/*----------------------------------------------------------------------------*/
void test_mode_handler()
{
}
/*----------------------------------------------------------------------------*/
static void set_mode(TestMode m)
{
  if(m == Idle)
  {
    if(sheduler_id != -1)
      sheduler_remove(sheduler_id);
    sheduler_id = -1;
  }
  else
  {
    if(sheduler_id == -1)
      sheduler_id = sheduler_add(test_timer, 0, 100, 100);
  }
  test_times = 0;
  mode = m;
}
/*----------------------------------------------------------------------------*/
static void test_timer(void *v)
{
  (void)v; //unused warning
  switch(mode)
  {
  case KeyTest:
    key_test();
    break;
  case AdcTest:
    adc_test();
    break;
  default:
    break;
  }
  test_times ++;
}
/*----------------------------------------------------------------------------*/
static void key_test()
{
  uint32_t keys;
  int i;
  if(!test_times)
  {
    lcd_clear();
    lcd_put_line(0, "Key test", SCR_ALIGN_LEFT);
    lcd_add_scroll_text(0, 10, 6, "Press any key");
  }
  keys = keys_real_state();
  for(i = 0; i < 6; i++)
  {
    lcd_put_char(5 - i, 1, (keys & (1 << i)) ? '1' : '0');
  }
  lcd_update();
}
/*----------------------------------------------------------------------------*/
static void adc_test()
{
  char buffer[16];
  int value = 0;
  if(!test_times)
  {
    lcd_clear();
    lcd_put_line(0, "ADC test", SCR_ALIGN_CENTER);
  }
  if(sys_adc_get_value(&value))
  {
    //value = 0xfed; //!!!
    snprintf(buffer, sizeof(buffer), "(0x%03x) %4d", value, value);
    lcd_put_line(1, buffer, SCR_ALIGN_RIGHT);
  }
  sys_adc_start_conversion();
  lcd_update();
}
/*----------------------------------------------------------------------------*/
