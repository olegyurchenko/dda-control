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
#ifndef DDA_CONV_H_1356079362
#define DDA_CONV_H_1356079362
/*----------------------------------------------------------------------------*/
#include <decimal.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void discrets2force(int discr, decimal32_t* dst);
void steps2um(unsigned step, decimal32_t* dst);
int um2steps(int um);
void umsize(unsigned empty_touch, unsigned touch, decimal32_t* dst);

void set_zero_force(int discr);
int is_touch_force(int discr);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_CONV_H_1356079362*/

