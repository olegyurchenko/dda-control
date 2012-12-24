/*----------------------------------------------------------------------------*/
/**
* @pkg dda_message
*/
/**
* Output message to LCD and wait.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 24.12.2012 10:24:55<br>
* @pkgdoc dda_message
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_MESSAGE_H_1356337495
#define DDA_MESSAGE_H_1356337495
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void show_message(const char *caption, const char *message, int timeout /*=0*/);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_MESSAGE_H_1356337495*/

