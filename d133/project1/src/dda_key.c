/*----------------------------------------------------------------------------*/
/**
* @pkg dda_key
*/
/**
* DDA front panel key handle.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 09.12.2012 11:55:36<br>
* @pkgdoc dda_key
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_key.h"
#include <stm32f10x.h>
#include <sys_timer.h>
#include <sys_sheduler.h>
/*----------------------------------------------------------------------------*/
#define KEY_1    GPIO_Pin_10 //PC10
#define KEY_2    GPIO_Pin_11 //PC11
#define KEY_3    GPIO_Pin_12 //PC12
#define KEY_4    GPIO_Pin_5 //PB5
#define KEY_5    GPIO_Pin_6 //PB6
#define KEY_6    GPIO_Pin_7 //PB7
/*----------------------------------------------------------------------------*/
static uint32_t key_buffer[3];
static void key_timer(void *v);
/*----------------------------------------------------------------------------*/
void keys_init()
{
  GPIO_InitTypeDef gpio;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  gpio.GPIO_Pin = KEY_1 | KEY_2 | KEY_3;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  //gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gpio.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &gpio);

  gpio.GPIO_Pin = KEY_4 | KEY_5;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &gpio);

  gpio.GPIO_Pin = KEY_6;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &gpio);

  sheduler_add(key_timer, 0, 20, 30);
}
/*----------------------------------------------------------------------------*/
uint32_t keys_real_state()
{
  uint16_t port;
  uint32_t keys = 0;
  port = GPIO_ReadInputData(GPIOC);
  keys |= (port & KEY_1) ? 0 : 1;
  keys |= (port & KEY_2) ? 0 : 2;
  keys |= (port & KEY_3) ? 0 : 4;
  port = GPIO_ReadInputData(GPIOB);
  keys |= (port & KEY_4) ? 0 : 8;
  keys |= (port & KEY_5) ? 0 : 16;
  keys |= (port & KEY_6) ? 0 : 32;
  return keys;
}
/*----------------------------------------------------------------------------*/
uint32_t keys_state()
{
  uint32_t keys = 0;
  //Majority
  keys |= key_buffer[0] & key_buffer[1];
  keys |= key_buffer[0] & key_buffer[2];
  keys |= key_buffer[1] & key_buffer[2];
  return keys;
}
/*----------------------------------------------------------------------------*/
static void key_timer(void *v)
{
  (void)v; //unused warning
  key_buffer[0] = key_buffer[1];
  key_buffer[1] = key_buffer[2];
  key_buffer[2] = keys_real_state();
}
/*----------------------------------------------------------------------------*/

