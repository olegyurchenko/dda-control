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
#define USE_CONSOLE //!!!!
#ifdef USE_CONSOLE
#include <console.h>
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
typedef enum
{
  Idle = -1,
  KeyTest = 0,
  AdcTest = 1,
  MotorTest = 2,
  TestCount
} TestMode;

TestMode mode = Idle;
static int sheduler_id = -1;
static int test_times;
static int old_key_state = 0;
static int motor = 0;
static int direction = 0;
/*----------------------------------------------------------------------------*/
static void set_mode(TestMode m);
static void test_timer(void *);
static void key_test();
static void adc_test();
static void motor_test();
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
static CONSOLE_CMD test_data;
static int test_cmd(int argc, char **argv)
{
  int test = 1;
  if(argc > 1)
    test = str2int(argv[1]);

  test --;
  if(test < TestCount)
    set_mode((TestMode) test);
  return CONSOLE_OK;
}
#endif //USE_CONSOLE
/*----------------------------------------------------------------------------*/
void test_mode_init()
{
  set_mode(KeyTest);
#ifdef USE_CONSOLE
  console_cmd_init(&test_data);
  test_data.cmd = "test";
  test_data.help = "test [mode 0-off, 1,2,..] - Test mode set";
  test_data.handler = test_cmd;
  console_add_cmd(&test_data);
#endif //USE_CONSOLE
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
    lcd_clear();
    lcd_update();
  }
  else
  {
    if(sheduler_id == -1)
      sheduler_id = sheduler_add(test_timer, 0, 100, 100);
  }
  test_times = 0;
  old_key_state = 0;
  mode = m;
}
/*----------------------------------------------------------------------------*/
static void test_timer(void *v)
{
  int key;
  (void)v; //unused warning
  key = keys_state();
  if((old_key_state & KEY_MENU) && !(key & KEY_MENU))
  {
    int m = mode;
    m ++;
    if(m >= TestCount)
      m = KeyTest;
    set_mode((TestMode)m);
  }

  old_key_state &= ~KEY_MENU;
  old_key_state |= key & KEY_MENU;

  switch(mode)
  {
  case KeyTest:
    key_test();
    break;
  case AdcTest:
    adc_test();
    break;
  case MotorTest:
    motor_test();
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
  sys_adc_start_conversion();
  while(!sys_adc_get_value(&value))
  {
  }

  //value = 0xfed; //!!!
  snprintf(buffer, sizeof(buffer), "(0x%03x) %4d", value, value);
  lcd_put_line(1, buffer, SCR_ALIGN_RIGHT);
  lcd_update();
}
/*----------------------------------------------------------------------------*/
static void motor_test()
{
  int key;
  char buffer[20];

  key = keys_state();
  if(!test_times)
  {
    lcd_clear();
    lcd_put_line(0, "Motor test", SCR_ALIGN_CENTER);
  }

  if((old_key_state & KEY_START) && !(key & KEY_START))
    motor_start(motor, direction, 0);

  if((old_key_state & KEY_STOP) && !(key & KEY_STOP))
    motor_deceleration();

  if((old_key_state & KEY_UP) && !(key & KEY_UP))
  {
    motor ++;
    if(motor > 1)
      motor = 0;
  }

  if((old_key_state & KEY_DOWN) && !(key & KEY_DOWN))
  {
    motor --;
    if(motor < 0)
      motor = 1;
  }

  if((old_key_state & KEY_OK) && !(key & KEY_OK))
  {
    direction ++;
    if(direction > 1)
      direction = 0;
  }

  //snprintf(buffer, sizeof(buffer), "%04x %04x", key, old_key_state); //!!!!
  old_key_state &= ~(KEY_START | KEY_STOP | KEY_UP | KEY_DOWN | KEY_OK);
  old_key_state |= key & (KEY_START | KEY_STOP | KEY_UP | KEY_DOWN | KEY_OK);

  snprintf(buffer, sizeof(buffer), "m:%d d:%d %08u", motor, direction, motor_step_count());
  lcd_put_line(1, buffer, SCR_ALIGN_LEFT);
  lcd_update();
}
/*----------------------------------------------------------------------------*/
