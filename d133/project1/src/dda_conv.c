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
#include <dda_config.h>
/*----------------------------------------------------------------------------*/
//This value must set for every force sensor !!!
//const decimal32_t force_k = {1, 0}; //1.
//#define EMPTY
static decimal64_t force_k = SENSOR_RATIO;
static decimal64_t zero_force = {0, 0}; //0
static decimal64_t step_ratio = STEP_RATIO;
/*----------------------------------------------------------------------------*/
void discrets2force(int discr, decimal32_t* dst)
{
  decimal64_t src, f;

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
  src = decimal64_init(discr, 0);
  touch = decimal64_init(TOUCH_DISCRETS, 0);
  decimal64_sub(&src, &zero_force, &src);
  return decimal64_cmp(&src, &touch) > 0;
}
/*----------------------------------------------------------------------------*/


