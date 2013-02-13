/*----------------------------------------------------------------------------*/
/**
* @pkg dda_mode
*/
/**
* DDA main module.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.12.2012 10:26:55<br>
* @pkgdoc dda_mode
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_MODE_H_1355992015
#define DDA_MODE_H_1355992015
/*----------------------------------------------------------------------------*/
#include <menu.h>

extern MENU_ITEM *root_menu;

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

  void mode_init();
  void set_root_mode();
  void start_root_menu();
  void device_startup();

  const char *device_version_str();
  const char *device_model_str();
  const char *device_serial_str();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_MODE_H_1355992015*/

