/*----------------------------------------------------------------------------*/
/**
* @pkg dda_protocol
*/
/**
* Serial protocol with PC.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 03.01.2013 10:31:16<br>
* @pkgdoc dda_protocol
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_PROTOCOL_H_1357201876
#define DDA_PROTOCOL_H_1357201876
/*----------------------------------------------------------------------------*/
#include <decimal.h>
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void protocol_init();
void protocol_handler();
void protocol_push_grain_size(const decimal32_t *size);
void protocol_push_current_force(const decimal32_t *force);
void protocol_push_strength(int index, int cell, const decimal32_t *strength);
int is_measure_data_transmitted();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_PROTOCOL_H_1357201876*/

