/*----------------------------------------------------------------------------*/
/**
* @pkg dda_settings
*/
/**
* Device settings for store in flash.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 24.01.2013  9:19:08<br>
* @pkgdoc dda_settings
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_SETTINGS_H_1359011948
#define DDA_SETTINGS_H_1359011948
/*----------------------------------------------------------------------------*/
typedef struct
{
  unsigned hname;
  char name[12];
  char value[16];
} DDA_SETTING;

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void settings_init();
int settings_read();
int settings_write();

const char* setting_get(const char *name);
void setting_set(const char *name, const char *value);
int setting_count();
const DDA_SETTING *setting(int index);
int setting_modified();
void setting_set_modified(int m);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_SETTINGS_H_1359011948*/

