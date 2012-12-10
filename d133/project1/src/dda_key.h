/*----------------------------------------------------------------------------*/
/**
* @pkg dda_key
*/
/**
* DDA front panel key handle.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 09.12.2012 11:55:36<br>
* @pkgdoc dda_key
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_KEY_H_1355046936
#define DDA_KEY_H_1355046936
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#define KEY_UP 32
#define KEY_DOWN 16
#define KEY_OK 8
#define KEY_START 4
#define KEY_MENU 2
#define KEY_STOP 1

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**Init keys*/
void keys_init();
/**Read keys nonbuffered state*/
uint32_t keys_real_state();
/**Read key buffer (anti-bounce)*/
uint32_t keys_state();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_KEY_H_1355046936*/

