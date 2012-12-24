/*----------------------------------------------------------------------------*/
/**
* @pkg dda_cassette
*/
/**
* Cassette functions.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.12.2012 12:12:32<br>
* @pkgdoc dda_cassette
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_CASSETTE_H_1355998352
#define DDA_CASSETTE_H_1355998352
/*----------------------------------------------------------------------------*/
#include <event.h>
#define CASSETTE_UNKNOWN_POSITION (-1)
#define CASSETTE_NULL_POSITION 0
#define CASSETTE_1ST_CELL 1
#define CASSETTE_MAX_CELL 60

extern handler_t cassete_handler;

#define CASSETTE_TIMEOUT_ERROR (-1)

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


void reset_cassette_position();
int is_cassete_position_unknown();
int cassette_position();
void cassete_goto_position(int pos);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_CASSETTE_H_1355998352*/

