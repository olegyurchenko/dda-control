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

#ifndef SIZE_T_DEFINED
typedef uint32_t size_t;
#define SIZE_T_DEFINED
#endif

#ifndef OFF_T_DEFINED
typedef int32_t off_t;
#define OFF_T_DEFINED
#endif

#ifndef SEEK_SET
/*whence const*/
#define SEEK_SET 0 //The offset is set to offset bytes.
#define SEEK_CUR 1 //The offset is set to its current location plus offset bytes.
#define SEEK_END 2 //The offset is set to the size of the file plus offset bytes.
#endif

//open mode values
#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void sflash_init();
int sflash_read_id(int32_t *dst);

/**Open flash for read or write*/
int sflash_open(int mode);
/**Close flash*/
void sflash_close();

off_t sflash_seek(off_t offset, int whence);
off_t sflash_tell();

/**Write flash*/
int sflash_write(const void *src, unsigned size);
/**Read from flash*/
int sflash_read(void *dst, unsigned size);


#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*SYS_FLASH_H_1358753655*/

