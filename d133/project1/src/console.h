/*----------------------------------------------------------------------------*/
/**
* @pkg console
*/
/**
* Conole via serial.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 27.08.2012 11:29:22<br>
* @pkgdoc console
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef CONSOLE_H_1346056162
#define CONSOLE_H_1346056162
/*----------------------------------------------------------------------------*/
typedef struct CONSOLE_CMD_TAG
{
  const char *cmd;
  const char *help;
  int (*handler)(int argc, char **argv);
  int (*state_handler)(void);
  struct CONSOLE_CMD_TAG *next;
} CONSOLE_CMD;

#define console_cmd_init(c) do {\
  (c)->handler = 0; \
  (c)->state_handler = 0;\
  (c)->next = 0;\
  (c)->cmd = 0;\
  (c)->help = 0;\
  } while(0)

#define CONSOLE_OK 0

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void console_init();
void console_add_cmd(CONSOLE_CMD *cmd);
int console_printf(const char *fmt, ...);
void console_handler();
/**Convert string to int. Support hex(0xabcd) octal(0777) and decimal string*/
int str2int(const char *str);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*CONSOLE_H_1346056162*/

