/*----------------------------------------------------------------------------*/
/**
* @pkg dda_sensor
*/
/**
* DDA Sensors.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 09.12.2012 11:56:03<br>
* @pkgdoc dda_sensor
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_sensor.h"
#include <sys_sheduler.h>
#include <stm32f10x.h>

#define NO_BUFFERED_SENSORS
/*----------------------------------------------------------------------------*/
#define SENS_1    GPIO_Pin_4 //PC4
#define SENS_2    GPIO_Pin_5 //PC5
#define SENS_3    GPIO_Pin_0 //PB0
#define SENS_4    GPIO_Pin_1 //PB1
/*----------------------------------------------------------------------------*/
#ifndef NO_BUFFERED_SENSORS
static uint32_t sensors_buffer[3];
static void sensor_timer(void *v);
#endif //NO_BUFFERED_SENSORS
/*----------------------------------------------------------------------------*/
void sensors_init()
{
  GPIO_InitTypeDef gpio;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  gpio.GPIO_Pin = SENS_1 | SENS_2;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  //gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gpio.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &gpio);

  gpio.GPIO_Pin = SENS_3 | SENS_4;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &gpio);

#ifndef NO_BUFFERED_SENSORS
  sheduler_add(sensor_timer, 0, 1, 10);
#endif //NO_BUFFERED_SENSORS
}
/*----------------------------------------------------------------------------*/
uint32_t sensors_real_state()
{
  uint16_t port;
  uint32_t state = 0;
  port = GPIO_ReadInputData(GPIOC);
  state |= (port & SENS_1) ? 0 : 1;
  state |= (port & SENS_2) ? 0 : 2;
  port = GPIO_ReadInputData(GPIOB);
  state |= (port & SENS_3) ? 0 : 4;
  state |= (port & SENS_4) ? 0 : 8;
  return state;
}
/*----------------------------------------------------------------------------*/
uint32_t sensors_state()
{
#ifndef NO_BUFFERED_SENSORS
  uint32_t sensors = 0;
  //Majority
  sensors |= sensors_buffer[0] & sensors_buffer[1];
  sensors |= sensors_buffer[0] & sensors_buffer[2];
  sensors |= sensors_buffer[1] & sensors_buffer[2];
  return sensors;
#else //NO_BUFFERED_SENSORS
  return sensors_real_state();
#endif //NO_BUFFERED_SENSORS
}
/*----------------------------------------------------------------------------*/
#ifndef NO_BUFFERED_SENSORS
static void sensor_timer(void *v)
{
  (void)v; //unused warning
  sensors_buffer[0] = sensors_buffer[1];
  sensors_buffer[1] = sensors_buffer[2];
  sensors_buffer[2] = sensors_real_state();
}
#endif //NO_BUFFERED_SENSORS
/*----------------------------------------------------------------------------*/
