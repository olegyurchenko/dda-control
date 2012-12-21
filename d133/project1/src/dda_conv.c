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
/*----------------------------------------------------------------------------*/
//This value must set for every force sensor !!!
//const decimal32_t force_k = {1, 0}; //1.
//#define EMPTY
//static decimal32_t force_k = {1, 0}; //1.
static decimal64_t force_k = {12195122, 8};//0.12195122
static decimal64_t zero_force = {0, 0}; //0
static decimal64_t step_ratio = {714286, 7}; //0.0714286
/*----------------------------------------------------------------------------*/
void discrets2force(int discr, decimal32_t* dst)
{
  decimal64_t src, f;

  /*dst = (discr - zero_force) * force_k */
  *dst = decimal32_init(discr, 0);
  src = decimal64_init(discr, 0);
  decimal64_sub(&src, &zero_force, &f);
  decimal64_mul(&f, &force_k, &f);
  decimal64_math_round(&f, 2, &f);
  decimal64_abs(&f, &src);
  if(src.data > 10000) /*if(abs(f) > 100.00)*/
    decimal64_math_round(&f, 1, &f);
  decimal64_32(&f, dst);
}
/*----------------------------------------------------------------------------*/
void steps2um(unsigned step, decimal32_t* dst)
{
  decimal64_t src, um;
  src = decimal64_init(step, 0);
  decimal64_mul(&src, &step_ratio, &um);
  decimal64_math_round(&um, 2, &um);
  decimal64_32(&um, dst);
}
/*----------------------------------------------------------------------------*/
void set_zero_force(int discr)
{
  zero_force = decimal64_init(discr, 0);
}
/*----------------------------------------------------------------------------*/

