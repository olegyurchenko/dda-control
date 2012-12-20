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
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define CASSETTE_UNKNOWN_POSITION (-1)
#define CASSETTE_NULL_POSITION 0
#define CASSETTE_1ST_CELL_POSITION 1

void reset_cassette_calibration();
int is_cassete_calibrated();
int cassette_position();
void cassette_calibration();
void cassete_goto_cell(int pos);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_CASSETTE_H_1355998352*/

