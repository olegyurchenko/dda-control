/*----------------------------------------------------------------------------*/
/**
* @pkg dda_clib
*/
/**
* DDA minimal library.
*
* Minimal C library calls.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 09.12.2012 14:48:23<br>
* @pkgdoc dda_clib
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_CLIB_H_1355057303
#define DDA_CLIB_H_1355057303

#define sys_tolower(c)       ((c>='A' && c<='Z') ? (c+('a'-'A')) : c)
#define sys_toupper(c)       ((c>='a' && c<='z') ? (c-('a'-'A')) : c)

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
int sys_strcmp(const void *v1, const void *v2);
const char *sys_strchr(const char *v, int c);
int sys_strlen(const void *v);
void sys_memset(void *dst, int c, unsigned size);
void sys_memcpy(void *dst, const void *src, unsigned size);
int sys_memcmp(const void *m1, const void *m2, unsigned size);
int sys_atoi(const void *v);
int sys_atoi8(const void *v);
int sys_atoi16(const void *v);
int sys_str2int(const char *s);
int sys_sprintf(char *buf, const char *fmt, ...);
int sys_snprintf(char *buf, unsigned buf_size, const char *fmt, ...);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#ifndef NO_CANONICAL_NAMES
#define strcmp sys_strcmp
#define strchr sys_strchr
#define strlen sys_strlen
#define atoi sys_atoi
#define str2int sys_str2int
#define memset sys_memset
#define memcpy sys_memcpy
#define memcmp sys_memcmp
#define sprintf sys_sprintf
#define snprintf sys_snprintf
#define tolower sys_tolower
#define toupper sys_toupper
#endif
/*----------------------------------------------------------------------------*/
#endif /*DDA_CLIB_H_1355057303*/

