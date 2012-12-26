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
, {STR_CASSETTE_MOTOR_TEST, "Cassette motor test"}
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
, {STR_PLUNGER_GO_HOME, "Plunger go home"}
, {STR_CASSETTE_GO_HOME, "Cassette go home"}
, {STR_POS_OPERATION_BREAK, "Positioning operation break !"}
, {STR_NUMBER_OF_SAMPLES, "Samples count"}
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

