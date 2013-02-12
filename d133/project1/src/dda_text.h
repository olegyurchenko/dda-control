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
#ifndef DDA_TEXT_H_1356419515
#define DDA_TEXT_H_1356419515
/*----------------------------------------------------------------------------*/
#define STR_ERROR 1 //"Error"
#define STR_WARNING 2 //"Warning"
#define STR_PLUNGER_TIMEOUT 3 //"Plunger timeout"
#define STR_PLUNGER_END_KEY 4 //"Plunger end key"
#define STR_CASSETTE_TIMEOUT 5 //"Cassette timeout"
#define STR_TEST 6 //"Test"
#define STR_CASSETTE_MOTOR_TEST 7 //"Cassette motor test"
#define STR_GOTO_0_CELL 8 //"Goto 0 cell"
#define STR_GOTO_NEXT_CELL 9 //"Goto next cell"
#define STR_PLUNGER_MOTOR_TEST 10 //"Plunger motor test"
#define STR_BOTTOM_POSITION 11 //"Bottom position"
#define STR_TOP_POSITION 12 //"Top position"
#define STR_ADC_TEST 13 //"ADC test"
#define STR_WORK 14 //"Work"
#define STR_AUTO 15 //"Auto"
#define STR_MANUAL 16 //"Manual"
#define STR_WORK_MODE 17 //"Work mode"
#define STR_AUTO_MODE 18 //"Auto mode"
#define STR_MANUAL_MODE 19 //"Manual mode"
#define STR_PLUNGER_GO_HOME 20 //"Plunger go home"
#define STR_CASSETTE_GO_HOME 21 //"Cassette go home"
#define STR_POS_OPERATION_BREAK 22 //"Positioning operation break !"
#define STR_NUMBER_OF_SAMPLES 23 //"Number of samples"
#define STR_CALIBRATION 24 //"Calibration"
#define STR_MEASURING 25 //"Measuring"
#define STR_WAIT_TOUCH 26 //"Wait touch"
#define STR_PRESS_START 27 //"Press START"
#define STR_FINISHING 28 //"Finishing"
#define STR_WAIT_PROTOCOL 29 //"Waiting protocol"
#define STR_GOTO_1_CELL 30 //"Goto 1 cell"
#define STR_CLOCKWISE 31 //"Clockwise"
#define STR_ANTICLOCKWISE 32 //"Anticlockwise"
#define STR_DEVICE 33 //"Device"
#define STR_GRIT 34 //"Grit"
#define STR_SAMPLES 35 //"Samples"
#define STR_AVG_STRENGTH 36 //"Avg. strength"
#define STR_ST_DEV_STRENGTH 37 //"St. dev. strength"
#define STR_MAX_STRENGTH 38 //"Max strength"
#define STR_MIN_STRENGTH 39 //"Min strength"
#define STR_AVG_SIZE 40 //"Avg. size"
#define STR_ST_DEV_SIZE 41 //"St. dev. size"
#define STR_MEASUREMENTS 42 //"Measurements"
#define STR_NO_PARTICLE 43 //"No particle"
#define STR_CASSETTE_WAIT 44 //"Please, insert next cassette"
#define STR_SESSION_RESULTS 45 //"Session results"
#define STR_PLUNGER_ERROR 46 //"Plunger error"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

const char *get_text(int id);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_TEXT_H_1356419515*/

