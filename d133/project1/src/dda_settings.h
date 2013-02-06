/*----------------------------------------------------------------------------*/
/**
* @pkg dda_settings
*/
/**
* Device settings for store in flash.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 24.01.2013  9:19:08<br>
* @pkgdoc dda_settings
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_SETTINGS_H_1359011948
#define DDA_SETTINGS_H_1359011948
/*----------------------------------------------------------------------------*/
typedef struct
{
  unsigned hname;
  char name[12];
  char value[16];
} DDA_SETTING;

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void settings_init();
void settings_clear();
int settings_read();
int settings_write();

const char* setting_get(const char *name);
void setting_set(const char *name, const char *value);
int setting_count();
const DDA_SETTING *setting(int index);
int setting_modified();
void setting_set_modified(int m);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/

/**Settings constans*/
#define S_SERIAL "serial"
#define S_MODEL "model"
#define S_SENSOR_RATIO "sensRatio"
#define S_STEP_RATIO "stepRatio"
#define S_CELL_STEP "cellStep"
#define S_C_TIMEOUT "cTimeout"
#define S_TOUCH_DISCRETS "dTouch"
#define S_CALIBRATION_FORCE "fCalibr"
#define S_MAX_FORCE "maxForce"
#define S_SLOW_RATE "slowRate"
#define S_um_SLOW_OFFSET "umSlow"
#define S_TOUCH_COUNT "touchCount"
#define S_PLUNGER_TIMEOUT "pTimeout"
#define S_SLOW_DOWN_POSITION "slowDown"
#define S_STP_MIN_PERIOD1 "minPeriod1"
#define S_STP_MIN_PERIOD2 "minPeriod2"
#define S_STOPPAGE_COUNT  "stopCount"
#define S_ACCELERATION_STEP1 "accStep1"
#define S_DECELERATION_STEP1 "decStep1"
#define S_CHANGE_RATE_STEP1 "chStep1"
#define S_ACCELERATION_STEP2 "accStep2"
#define S_DECELERATION_STEP2 "decStep2"
#define S_CHANGE_RATE_STEP2 "chStep2"


/*----------------------------------------------------------------------------*/
#endif /*DDA_SETTINGS_H_1359011948*/

