/*----------------------------------------------------------------------------*/
/**
* @pkg view
*/
/**
* Text view via LCD.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 18.01.2013  9:55:01<br>
* @pkgdoc view
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef VIEW_H_1358495701
#define VIEW_H_1358495701
#include <dda_lcd.h>
/*----------------------------------------------------------------------------*/
typedef void (*view_get_t)(void *data, int index, char **dst, SCR_ALIGN *align);

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void viewer(view_get_t get, void *data, int string_count);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*VIEW_H_1358495701*/

