/*
DO NOT EDIT MANUALLY THIS FILE.
This file was generated form src/dda_text.c.utf8
*/
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
#include <dda_settings.h>
#include <dda_clib.h>
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
const text_t text_ru[] =
{
  {-1, "?text?"}
, {STR_ERROR, "O\xc1""\xb8""\xb2""\xba""a"/*Ошибка*/}
, {STR_WARNING, "B\xbd""\xb8""\xbc""a\xbd""\xb8""e"/*Внимание*/}
, {STR_PLUNGER_TIMEOUT, "Timeout \xbe""\xbb""y\xbd""\xb6""epa"/*Timeout плунжера*/}
, {STR_PLUNGER_END_KEY, "\xa8""\xbb""y\xbd""\xb6""ep \xb3""\xb3""epxy"/*Плунжер вверху*/}
, {STR_CASSETTE_TIMEOUT, "Timeout \xba""acce\xbf""\xc3"""/*Timeout кассеты*/}
, {STR_TEST, "\xa8""po\xb3""ep\xba""a"/*Проверка*/}
, {STR_CASSETTE_MOTOR_TEST, "Bpa\xe6""e\xbd""\xb8""e \xba""acce\xbf""\xc3"""/*Вращение кассеты*/}
, {STR_GOTO_0_CELL, "Ha 0 \xc7""\xc0""e\xb9""\xba""y"/*На 0 ячейку*/}
, {STR_GOTO_NEXT_CELL, "C\xbb""e\xe3""y\xc6""\xe6""a\xc7"" \xc7""\xc0""e\xb9""\xba""a"/*Следующая ячейка*/}
, {STR_PLUNGER_MOTOR_TEST, "\xa8""po\xb3""ep\xba""a \xbe""\xbb""y\xbd""\xb6""epa"/*Проверка плунжера*/}
, {STR_BOTTOM_POSITION, "B\xbd""\xb8""\xb7"""/*Вниз*/}
, {STR_TOP_POSITION, "B\xb3""epx"/*Вверх*/}
, {STR_ADC_TEST, "\xa8""po\xb3""ep\xba""a A\xe1""\xa8"""/*Проверка АЦП*/}
, {STR_WORK, "Pa\xb2""o\xbf""a"/*Работа*/}
, {STR_AUTO, "A\xb3""\xbf""o"/*Авто*/}
, {STR_MANUAL, "Py\xc0""\xbd""o\xb9"""/*Ручной*/}
, {STR_WORK_MODE, "Pa\xb2""o\xbf""a"/*Работа*/}
, {STR_AUTO_MODE, "A\xb3""\xbf""o"/*Авто*/}
, {STR_MANUAL_MODE, "Py\xc0""\xbd""o\xb9"""/*Ручной*/}
, {STR_PLUNGER_GO_HOME, "Bo\xb7""\xb3""pa\xbf"" \xbe""\xbb""y\xbd""\xb6""epa"/*Возврат плунжера*/}
, {STR_CASSETTE_GO_HOME, "Bo\xb7""\xb3""pa\xbf"" \xba""acce\xbf""\xc3"""/*Возврат кассеты*/}
, {STR_POS_OPERATION_BREAK, "\xa8""pep\xb3""a\xbd""o \xbe""o\xb7""\xb8""\xe5""\xb8""o\xbd""\xb8""po\xb3""a\xbd""\xb8""e !"/*Прервано позиционирование !*/}
, {STR_NUMBER_OF_SAMPLES, "\xab""\xb8""c\xbb""o \xbe""po\xb2"""/*Число проб*/}
, {STR_CALIBRATION, "Ka\xbb""\xb8""\xb2""po\xb3""\xba""a"/*Калибровка*/}
, {STR_MEASURING, "\xa5""\xb7""\xbc""epe\xbd""\xb8""e"/*Измерение*/}
, {STR_WAIT_TOUCH, "O\xb6""\xb8""\xe3""a\xbd""\xb8""e \xba""aca\xbd""\xb8""\xc7"""/*Ожидание касания*/}
, {STR_PRESS_START, "Ha\xb6""\xbc""\xb8"" START"/*Нажми START*/}
, {STR_FINISHING, "O\xba""o\xbd""\xc0""a\xbd""\xb8""e"/*Окончание*/}
, {STR_WAIT_PROTOCOL, "O\xb6""\xb8""\xe3""a\xbd""\xb8""e o\xb2""\xbc""e\xbd""a"/*Ожидание обмена*/}
, {STR_GOTO_1_CELL, "Ha 1-\xc6"" \xc7""\xc0""e\xb9""\xba""y"/*На 1-ю ячейку*/}
, {STR_CLOCKWISE, "\xa8""o \xc0""aco\xb3""o\xb9"""/*По часовой*/}
, {STR_ANTICLOCKWISE, "\xa8""po\xbf""\xb8""\xb3"" \xc0""aco\xb3""o\xb9"""/*Против часовой*/}
, {STR_DEVICE , "\xa8""p\xb8""\xb2""op"/*Прибор*/}
, {STR_GRIT, "\xa4""ep\xbd""o"/*Зерно*/}
, {STR_SAMPLES, "\xa5""\xb7""\xbc""epe\xbd""\xb8""\xb9"""/*Измерений*/}
, {STR_AVG_STRENGTH, "P cp"/*P ср*/}
, {STR_ST_DEV_STRENGTH, "P c\xba""o"/*P ско*/}
, {STR_MAX_STRENGTH, "P max"}
, {STR_MIN_STRENGTH, "P min"}
, {STR_AVG_SIZE, "S cp"/*S ср*/}
, {STR_ST_DEV_SIZE, "S c\xba""o"/*S ско*/}
, {STR_MEASUREMENTS, "\xa5""\xb7""\xbc""epe\xbd""\xb8""\xc7"""/*Измерения*/}
, {STR_NO_PARTICLE, "He\xbf"" \xb7""ep\xbd""a"/*Нет зерна*/}
, {STR_CASSETTE_WAIT, "O\xb6""\xb8""\xe3""a\xbd""\xb8""e \xba""acce\xbf""\xc3"""/*Ожидание кассеты*/}
, {STR_SESSION_RESULTS, "Pe\xb7""y\xbb""\xc4""\xbf""a\xbf"" \xb7""a\xbc""epo\xb3"""/*Результат замеров*/}
, {STR_PLUNGER_ERROR, "C\xb2""o\xb9"" \xbe""\xbb""y\xbd""\xb6""epa"/*Сбой плунжера*/}

};
/*----------------------------------------------------------------------------*/
static const text_t *text = 0;
/*----------------------------------------------------------------------------*/
const char *get_text(int id)
{
  const char *txt;
  unsigned i;

  if(text == 0)
  {
    const char *v;
    int lang = 0;
    v = setting_get(S_LANG);
    if(v != 0 && *v)
      lang = atoi(v);

    switch(lang)
    {
    case 0:
    default:
      text = text_en;
      break;
    case 1:
      text = text_ru;
      break;
    }
  }

  txt = text[0].text;
  for(i = 1; i < sizeof(text_en)/sizeof(text_en[0]); i++)
  {
    if(text[i].id == id)
    {
      txt = text[i].text;
      break;
    }
  }
  return txt;
}
/*----------------------------------------------------------------------------*/

