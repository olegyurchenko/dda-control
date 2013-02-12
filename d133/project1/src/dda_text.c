/*----------------------------------------------------------------------------*/
/**
* @pkg dda_text
*/
/**
* Text store.
*
* Use for internationalisation (in future).<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 25.12.2012  9:11:55<br>
* @pkgdoc dda_text
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_text.h"
/*----------------------------------------------------------------------------*/
typedef struct
{
  int id;
  const char *text;
} text_t;
/*----------------------------------------------------------------------------*/
const text_t text_en[] =
{
  {-1, "?text?"}
, {STR_ERROR, "Error"}
, {STR_WARNING, "Warning"}
, {STR_PLUNGER_TIMEOUT, "Plunger timeout"}
, {STR_PLUNGER_END_KEY, "Plunger end key"}
, {STR_CASSETTE_TIMEOUT, "Cassette timeout"}
, {STR_TEST, "Test"}
, {STR_CASSETTE_MOTOR_TEST, "Cassette rotation"}
, {STR_GOTO_0_CELL, "Goto 0 cell"}
, {STR_GOTO_NEXT_CELL, "Goto next cell"}
, {STR_PLUNGER_MOTOR_TEST, "Plunger motor test"}
, {STR_BOTTOM_POSITION, "Bottom position"}
, {STR_TOP_POSITION, "Top position"}
, {STR_ADC_TEST, "ADC test"}
, {STR_WORK, "Work"}
, {STR_AUTO, "Auto"}
, {STR_MANUAL, "Manual"}
, {STR_WORK_MODE, "Work mode"}
, {STR_AUTO_MODE, "Auto mode"}
, {STR_MANUAL_MODE, "Manual mode"}
, {STR_PLUNGER_GO_HOME, "Plunger return"}
, {STR_CASSETTE_GO_HOME, "Cassette return"}
, {STR_POS_OPERATION_BREAK, "Positioning operation break !"}
, {STR_NUMBER_OF_SAMPLES, "Samples count"}
, {STR_CALIBRATION, "Calibration"}
, {STR_MEASURING, "Measuring"}
, {STR_WAIT_TOUCH, "Wait touch"}
, {STR_PRESS_START, "Press START"}
, {STR_FINISHING, "Finishing"}
, {STR_WAIT_PROTOCOL, "Waiting protocol"}
, {STR_GOTO_1_CELL, "Goto 1 cell"}
, {STR_CLOCKWISE, "Clockwise"}
, {STR_ANTICLOCKWISE, "Anticlockwise"}
, {STR_DEVICE , "Device"}
, {STR_GRIT, "Grit"}
, {STR_SAMPLES, "Samples"}
, {STR_AVG_STRENGTH, "P avg"}
, {STR_ST_DEV_STRENGTH, "P dev"}
, {STR_MAX_STRENGTH, "P max"}
, {STR_MIN_STRENGTH, "P min"}
, {STR_AVG_SIZE, "S avg"}
, {STR_ST_DEV_SIZE, "S dev"}
, {STR_MEASUREMENTS, "Measurements"}
, {STR_NO_PARTICLE, "No particle"}
, {STR_CASSETTE_WAIT, "Cassette wait"}
, {STR_SESSION_RESULTS, "Session results"}
, {STR_PLUNGER_ERROR, "Plunger error"}

};
/*----------------------------------------------------------------------------*/
const char *get_text(int id)
{
  const char *txt;
  unsigned i;

  txt = text_en[0].text;
  for(i = 1; i < sizeof(text_en)/sizeof(text_en[0]); i++)
  {
    if(text_en[i].id == id)
    {
      txt = text_en[i].text;
      break;
    }
  }
  return txt;
}
/*----------------------------------------------------------------------------*/

