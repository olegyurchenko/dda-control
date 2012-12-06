/*----------------------------------------------------------------------------*/
/**
* @pkg sys_sheduler
*/
/**
* Simple sheduler.
*
* Sheduler functions.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 06.12.2012 10:37:50<br>
* @pkgdoc sys_sheduler
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef SYS_SHEDULER_H_1354783070
#define SYS_SHEDULER_H_1354783070
/*----------------------------------------------------------------------------*/
#include <stdint.h>
typedef void (*sh_callback)(void *);

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**Init sheduler*/
void sheduler_init();
/**
* Add task to sheduler.
*
* @param cb - Addres of callback procedure.
* @param user_data - optional param to pass callback procedure.
* @param delay - Delay in ms before the first call.
* @param period - Period procedure calls. Can be 0 - callback will call once.
* @return Index in sheduler table or -1 if fail.
*/
int sheduler_add(sh_callback cb, void *user_data, uint32_t delay, uint32_t period);
/**
* Remove task.
*
* @param index - index in sheduler table.
*/
void sheduler_remove(int index);
/**Sheduler handle procedure. Should be called periodically*/
void sheduler_handler();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*SYS_SHEDULER_H_1354783070*/

