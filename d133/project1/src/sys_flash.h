/*----------------------------------------------------------------------------*/
/**
* @pkg sys_flash
*/
/**
* SPI flash AT45DB081D functions.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 21.01.2013  9:34:15<br>
* @pkgdoc sys_flash
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef SYS_FLASH_H_1358753655
#define SYS_FLASH_H_1358753655
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void sflash_init();
int sflash_read_id(int32_t *dst);


#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*SYS_FLASH_H_1358753655*/

