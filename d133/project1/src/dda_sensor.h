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
#ifndef DDA_SENSOR_H_1355046963
#define DDA_SENSOR_H_1355046963
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#define DOWN_SENSOR 4
#define UP_SENSOR 8
#define CASSETE_0_SENSOR 1
#define CASSETE_CELL_SENSOR 2


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void sensors_init();
uint32_t sensors_real_state();
uint32_t sensors_state();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_SENSOR_H_1355046963*/

