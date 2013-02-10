/*----------------------------------------------------------------------------*/
/**
* @pkg dda_conv
*/
/**
* Convertion discrets values to phisics values.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 21.12.2012 10:42:42<br>
* @pkgdoc dda_conv
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_conv.h"
#include <dda_clib.h>
#include <dda_config.h>
#include <dda_settings.h>
#include <dda_mesh.h>
#define DEBUG

#ifdef DEBUG
#include <console.h>
#include <dda_uart.h>
#endif
/*----------------------------------------------------------------------------*/
//This value must set for every force sensor !!!
//const decimal32_t force_k = {1, 0}; //1.
//#define EMPTY
static decimal64_t force_k = SENSOR_RATIO;
static decimal64_t zero_force = {0, 0}; //0
static decimal64_t step_ratio = STEP_RATIO;
static decimal32_t max_force = MAX_FORCE;
static decimal32_t calibration_force = CALIBRATION_FORCE;
static int is_init = 0;
static int touch_discrets = TOUCH_DISCRETS;
/*----------------------------------------------------------------------------*/
static decimal64_t k_discrets_to_steps;
static int delta_discrets;
/*----------------------------------------------------------------------------*/
static void init()
{
  if(!is_init)
  {
    const char *value;

    value = setting_get(S_SENSOR_RATIO);
    if(value != 0 && *value)
      str_decimal64(value, &force_k);

    value = setting_get(S_STEP_RATIO);
    if(value != 0 && *value)
      str_decimal64(value, &step_ratio);

    value = setting_get(S_MAX_FORCE);
    if(value != 0 && *value)
      str_decimal32(value, &max_force);

    value = setting_get(S_TOUCH_DISCRETS);
    if(value != 0 && *value)
      touch_discrets = atoi(value);

    value = setting_get(S_CALIBRATION_FORCE);
    if(value != 0 && *value)
      str_decimal32(value, &calibration_force);

    is_init = 1;
  }
}
/*----------------------------------------------------------------------------*/
void discrets2force(int discr, decimal32_t* dst)
{
  decimal64_t src, f;

  init();
  /*dst = (discr - zero_force) * force_k */
  //set_fast_division(1); //!!!!!!!!!!!!
  *dst = decimal32_init(discr, 0);
  src = decimal64_init(discr, 0);
  decimal64_sub(&src, &zero_force, &f);
  decimal64_mul(&f, &force_k, &f);
  decimal64_math_round(&f, 2, &f);
  decimal64_abs(&f, &src);
  if(src.data > 10000) /*if(abs(f) > 100.00)*/
    decimal64_math_round(&f, 1, &f);
  decimal64_32(&f, dst);
  //set_fast_division(0); //!!!!!!!!!!!!
}
/*----------------------------------------------------------------------------*/
void steps2um(unsigned step, decimal32_t* dst)
{
  decimal64_t src, um;

  init();
  //set_fast_division(1); //!!!!!!!!!!!!
  src = decimal64_init(step, 0);
  decimal64_mul(&src, &step_ratio, &um);
  decimal64_math_round(&um, 2, &um);
  decimal64_32(&um, dst);
  //set_fast_division(0); //!!!!!!!!!!!!
}
/*----------------------------------------------------------------------------*/
int um2steps(int um)
{
  decimal64_t src, stp;
  decimal32_t dst;

  init();
  src = decimal64_init(um, 0);
  decimal64_div(&src, &step_ratio, &stp);
  decimal64_math_round(&stp, 0, &stp);
  if(decimal64_32(&stp,  &dst))
    return MAX_INT_32;
  return dst.data;
}
/*----------------------------------------------------------------------------*/
void umsize(unsigned empty_touch, unsigned touch, decimal32_t* dst)
{
  decimal64_t d1, d2;

  init();
  d1 = decimal64_init(touch, 0);
  d2 = decimal64_init(empty_touch, 0);
  decimal64_sub(&d2, &d1, &d1);
  decimal64_mul(&d1, &step_ratio, &d1);
  decimal64_math_round(&d1, 1, &d1);
  decimal64_32(&d1, dst);
}
/*----------------------------------------------------------------------------*/
void set_zero_force(int discr)
{
  zero_force = decimal64_init(discr, 0);
}
/*----------------------------------------------------------------------------*/
int get_zero_force(void)
{
  return (int)zero_force.data;
}
/*----------------------------------------------------------------------------*/
int is_touch_force(int discr)
{
  decimal64_t src, touch;

  init();
  src = decimal64_init(discr, 0);
  touch = decimal64_init(touch_discrets, 0);
  decimal64_sub(&src, &zero_force, &src);
  return decimal64_cmp(&src, &touch) > 0;
}
/*----------------------------------------------------------------------------*/
const decimal32_t* get_max_force()
{
  init();
  return &max_force;
}
/*----------------------------------------------------------------------------*/
const decimal32_t* get_calibration_force()
{
  init();
  return &calibration_force;
}
/*----------------------------------------------------------------------------*/
void set_calibration_data(int steps, int discr)
{
  decimal64_t f, s;

  k_discrets_to_steps = decimal64_init(1, 0); //Do not live it uninitialised

  s = decimal64_init(steps, 0);

  f = decimal64_init(discr - touch_discrets, 0);
  decimal64_sub(&f, &zero_force, &f);
  decimal64_div(&f, &s, &k_discrets_to_steps); //k_discrets_to_steps = descr / steps
  decimal64_math_round(&k_discrets_to_steps, 8, &k_discrets_to_steps);

  delta_discrets = 0;

#ifdef DEBUG
  {
    char buffer[32];
    decimal64_str(&k_discrets_to_steps, buffer, sizeof(buffer));
    console_printf("\r\nk_discrets_to_steps=%s", buffer);
  }
#endif //DEBUG

}
/*----------------------------------------------------------------------------*/
int is_destruction(int size, int steps, int discr)
{
  decimal64_t calc_discrets, discrets, delta;

  if(!delta_discrets)
  {
    decimal64_t f, s;

    s = decimal64_init(size, 0);
    decimal32_64(relative_size_deviation(), &f);
    decimal64_mul(&s, &f, &s);
    decimal64_mul(&s, &k_discrets_to_steps, &s);
    decimal64_math_round(&s, 0, &s);
    delta_discrets = (int32_t) s.data; //XXC
#ifdef DEBUG
    console_printf("\r\nsize=%u delta_discrets=%d", size, delta_discrets);
#endif
  }

  calc_discrets = decimal64_init(steps, 0);
  decimal64_mul(&calc_discrets, &k_discrets_to_steps, &calc_discrets);
  decimal64_math_round(&calc_discrets, 0, &calc_discrets);

  discrets = decimal64_init(discr, 0);
  decimal64_sub(&discrets, &zero_force, &discrets);

  delta = decimal64_init(delta_discrets, 0);
/*
#ifdef DEBUG
  if(!uart_tx_size())
  {
    console_printf("\r\nsteps=%d calc_discrets=%d discrets=%d", steps, (int)calc_discrets.data, (int)discrets.data);
  }
#endif //DEBUG
*/
  decimal64_sub(&calc_discrets, &discrets, &discrets);

  return decimal64_cmp(&discrets, &delta) > 0;
}
/*----------------------------------------------------------------------------*/

