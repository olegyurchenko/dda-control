/*----------------------------------------------------------------------------*/
/**
* @pkg dda_config
*/
/**
* Configuration for device copy.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 14.01.2013 10:59:42<br>
* @pkgdoc dda_config
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_CONFIG_H_1358153982
#define DDA_CONFIG_H_1358153982
/*----------------------------------------------------------------------------*/
#define VERSION "0.1.a"
#define DEVICE_MODEL "DDA-133"
#define SENSOR_RATIO {1, 0} //1.0
#define STEP_RATIO {714286, 7} //0.0714286
#define CASSETTE_MAX_CELL 60
#define CASSETE_CELL_STEP 1
#define CASSETTE_TIMEOUT 10000 //10s
#define TOUCH_DISCRETS 5
#define MAX_FORCE {50,0} //50N
#define SLOW_RATE 128
#define um_SLOW_OFFSET 1500 //1.5mm
#define TOUCH_COUNT 3
#define PLUNGER_TIMEOUT 40000 //40s
#define SLOW_DOWN_POSITION 2000

/*----------------------------------------------------------------------------*/
/**dda_motors config*/
/*----------------------------------------------------------------------------*/
#define STP_MIN_PERIOD1 500//us
#define STP_MIN_PERIOD2 400 //us
#define STOPPAGE_TIME 30000 //us
#define STOPPAGE_COUNT 4
/*----------------------------------------------------------------------------*/
#define ACCELERATION_STEP1 1
#define DECELERATION_STEP1 16
#define CHANGE_RATE_STEP1 1
/*----------------------------------------------------------------------------*/
#define ACCELERATION_STEP2 1
#define DECELERATION_STEP2 4
#define CHANGE_RATE_STEP2 1
/*----------------------------------------------------------------------------*/


//#define USE_CONSOLE //Uncomment if ouy use console interface
#define NO_CASSETTE //Uncomment if your cassette mechanick in not work

/*----------------------------------------------------------------------------*/
#endif /*DDA_CONFIG_H_1358153982*/

