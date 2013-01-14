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

//#define DEVICE_SERIAL 2202
#define DEVICE_SERIAL 2203

#define DEVICE_MODEL "DDA-133"
#define SENSOR_RATIO {12195122, 8} //0.12195122
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
//#define USE_CONSOLE //Uncomment if ouy use console interface
#define NO_CASSETTE //Unkomment if your cassette mechanick in not work


/*----------------------------------------------------------------------------*/
#if (DEVICE_SERIAL == 2202)
#undef SENSOR_RATIO
#define SENSOR_RATIO {575, 3} //0.575
#undef CASSETTE_MAX_CELL
#define CASSETTE_MAX_CELL 30
#undef CASSETE_CELL_STEP
#define CASSETE_CELL_STEP 2

#endif //2202
/*----------------------------------------------------------------------------*/
#if (DEVICE_SERIAL == 2203)
#undef SENSOR_RATIO
#define SENSOR_RATIO {72, 3} //0.072
#endif //2203
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
#endif /*DDA_CONFIG_H_1358153982*/

