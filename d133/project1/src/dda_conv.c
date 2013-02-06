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
static decimal32_t k_force_to_steps;
static decimal32_t k_discrets_to_steps;
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
  decimal32_t f, s;

  s = decimal32_init(steps, 0);
  k_force_to_steps = decimal32_init(1, 0); //Do not live it uninitialised
  k_discrets_to_steps = decimal32_init(1, 0); //Do not live it uninitialised
  discrets2force(discr - touch_discrets, &f);
  decimal32_div(&f, &s, &k_force_to_steps); //k_force_to_steps = force / steps
  f = decimal32_init(discr - touch_discrets, 0);
  decimal32_div(&f, &s, &k_discrets_to_steps); //k_discrets_to_steps = descr / steps

  /**
    XC := round(k_discrets_to_steps * size[grain] / ratio * relative_size_deviation);
    XC := round(k_discrets_to_steps * steps * relative_size_deviation);
  */
  s = decimal32_init(um2steps(mesh()->min), 0);
  decimal32_mul(&s, relative_size_deviation(), &s);
  decimal32_mul(&s, &k_discrets_to_steps, &s);
  decimal32_math_round(&s, 0, &s);
  decimal32_abs(&s, &s);
  delta_discrets = s.data; //XC

#ifdef DEBUG
  {
    char buffer[32];
    decimal32_str(&k_discrets_to_steps, buffer, sizeof(buffer));
    console_printf("\r\nk_discrets_to_steps=%s", buffer);
    console_printf("\r\ndelta_discrets=%d", delta_discrets);
    decimal32_str(&k_force_to_steps, buffer, sizeof(buffer));
    console_printf("\r\nk_force_to_steps=%s", buffer);
  }
#endif //DEBUG

}
/*----------------------------------------------------------------------------*/

