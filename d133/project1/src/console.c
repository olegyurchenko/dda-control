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
#include "console.h"
#include <stm32f10x.h>
#include <dda_uart.h>
#include <sys_timer.h>
#include <stdint.h>
#include <stdarg.h>
#include <dda_clib.h>
/*----------------------------------------------------------------------------*/
#ifndef ABS
#define ABS(a)              (((a) < 0) ? (- (a)) : (a))
#endif
/*----------------------------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif
/*----------------------------------------------------------------------------*/
static enum
{
  INIT_STATE,
  IDLE_STATE,
  CALL_BUFFER_STATE,
  USER_STATE

} console_state = INIT_STATE;
/*----------------------------------------------------------------------------*/
static union
{
  uint32_t sleep_end;
  int dummy;
} params;
/*----------------------------------------------------------------------------*/
static CONSOLE_CMD *root_cmd = NULL;
static int (*user_handler)() = NULL;
/*----------------------------------------------------------------------------*/
static void idle_handler();
static int sleep_handler();
static void console_prompt();
static int parse_cmd(char *src, char **args);
static int call_cmd(int argc, char **argv);
static int help_cmd(int argc, char **argv);
static int sleep_cmd(int argc, char **argv);
static int buffer_handler();
/*----------------------------------------------------------------------------*/
static char console_buffer[256];
static int console_index = 0;

#define VERSION 1
const char *prompt = ">";
/*----------------------------------------------------------------------------*/
static int str_out(const char *src, int before, int after)
{
  int l, r = 0;
  char buf = ' ';
  l = strlen(src);
  while(l < before)
  {
    r += uart_write(&buf, 1);
    before --;
  }
  r += uart_write(src, l);
  while(l < after)
  {
    r += uart_write(&buf, 1);
    after --;
  }

  return r;
}
/*----------------------------------------------------------------------------*/
static int int_out(char spec, uint32_t src, int width, char blank)
{
  const char ascii[] = "0123456789abcdef";
  const char ASCII[] = "0123456789ABCDEF";
  const char *table;
  int r = 0, base, i = 0, sign = 0, len;
  char buffer[20];

  i = sizeof(buffer);
  table = ascii;
  switch(spec)
  {
  default:
  case 'd':
  case 'i':
    if(src & 0x80000000)
      sign = 1;
    src &= 0x7fffffff;
  case 'u':
    base = 10;
    break;

  case 'o':
    base = 8;
    break;
  case 'X':
    table = ASCII;
  case 'x':
    base = 16;
    break;
  }

  if(!src)
    buffer[--i] = '0';
  else
  while(src && i)
  {
    buffer[--i] = table[src % base];
    src /= base;
  }
  if(sign && i)
    buffer[--i] = '-';
  len = sizeof(buffer) - i;
  while(len < width)
  {
    r += uart_write(&blank, 1);
    width --;
  }
  r += uart_write(&buffer[i], len);
  return r;
}
/*----------------------------------------------------------------------------*/
int console_printf(const char *fmt, ...)
{
  char spec[20];
  int r = 0, i, brk;
  const char *fmt0;
  va_list ap;
  va_start(ap, fmt);

  fmt0 = fmt;
  while(*fmt)
  {
    if(*fmt == '%')
    {
      r += uart_write(fmt0, fmt - fmt0);
      fmt ++;
      brk = 0;
      i = 0;
      spec[0] = '\0';
      while(*fmt && !brk)
      {
        brk = 1;
        switch(*fmt)
        {
        case '%':
          r += uart_write(fmt, 1);
          break;
        case 'c':
          spec[0] = va_arg(ap, int);
          r += uart_write(spec, 1);
          break;

        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          r += int_out(*fmt, va_arg(ap, uint32_t), atoi(spec), spec[0] == '0' ? '0' : ' ');
          break;
        case 's':
          i = atoi(spec);
          r += str_out(va_arg(ap, char*), i > 0 ? i : 0, i < 0 ? -i : 0);
          break;
        default:
          if(i < (int)sizeof(spec))
          {
            spec[i++] = *fmt;
            spec[i] = '\0';
          }
          brk = 0;
          break;
        }
        fmt ++;
      }
      fmt0 = fmt;
      continue;
    }
    fmt ++;
  }
  va_end(ap);
  if(fmt > fmt0)
    r += uart_write(fmt0, fmt - fmt0);
  return r;
}
/*----------------------------------------------------------------------------*/
static void console_prompt()
{
  console_printf("\r\n%s", prompt);
}
/*----------------------------------------------------------------------------*/
void console_init()
{
  static CONSOLE_CMD help, sleep;
  console_state = INIT_STATE;
  console_cmd_init(&help);
  help.cmd = "help";
  help.handler = help_cmd;
  help.help = "This help";
  console_add_cmd(&help);
  console_cmd_init(&sleep);
  sleep.cmd = "sleep";
  sleep.handler = sleep_cmd;
  sleep.state_handler = sleep_handler;
  sleep.help = "sleep <ms> - Non blocked sleep";
  console_add_cmd(&sleep);
}
/*----------------------------------------------------------------------------*/
void console_handler()
{
  switch(console_state)
  {
  default:
    console_state = INIT_STATE;
  case INIT_STATE:
    console_printf("\r\n\r\nConsole v%d build: %s %s\r\n", VERSION, __DATE__, __TIME__);
    console_printf("SystemCoreClock:%u Hz\r\n", SystemCoreClock);
    console_prompt();
    console_state = IDLE_STATE;
    console_index = 0;
  case IDLE_STATE:
    idle_handler();
    break;
  case CALL_BUFFER_STATE:
    if(CONSOLE_OK == buffer_handler())
      break;
    console_state = IDLE_STATE;
    console_index = 0;
    console_prompt();
    break;
  case USER_STATE:
    if(user_handler != NULL && (CONSOLE_OK == user_handler()))
      break;
    console_state = IDLE_STATE;
    console_index = 0;
    console_prompt();
    break;
  }
}
/*----------------------------------------------------------------------------*/
static void idle_handler()
{
  int i, j, received;
  char *args[20];

  if(console_index >= (int)sizeof(console_buffer) - 1)
  {
    console_index = 0;
    console_prompt();
  }

  received = uart_read(&console_buffer[console_index], sizeof(console_buffer) - console_index);
  for(i = 0; i < received && console_index < (int)sizeof(console_buffer) - 1; i++)
  {
    if(console_buffer[console_index] == '\t')
      console_buffer[console_index] = ' ';

    if(console_buffer[console_index] == 8) /*Bksp*/
    {
      if(console_index)
      {
        const char bkspace[] = "\010\033[K";
        int console_index1;
        console_printf(bkspace);
        for(j = i, console_index1 = console_index; j < received; j++, console_index1 ++)
          console_buffer[console_index1] = console_buffer[console_index1 + 1];
        console_index --;
      }
      continue;
    }

    if(console_buffer[console_index] == '\n' || console_buffer[console_index] == '\r')
    {
      console_buffer[console_index] = '\0';
      received = parse_cmd(console_buffer, args);
      console_index = 0;
      if(received)
      {
        args[received] = NULL;
        call_cmd(received, args);
      }

      if(console_state == IDLE_STATE)
        console_prompt();
      else
        console_printf("\r");
      break;
    }

    uart_write(&console_buffer[console_index], 1);
    console_index ++;
  }

}
/*----------------------------------------------------------------------------*/
static int buffer_handler()
{
  return -1;
}
/*----------------------------------------------------------------------------*/
static int sleep_handler()
{
  if(params.sleep_end < sys_tick_count())
    return -1;
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int parse_cmd(char *src, char **args)
{
  int count = 0;
  int notBlank = 0;
  int quote = 0;

  while(*src)
  {
    if(!quote && (*src == ' ' || *src == '\t' || *src == '\r' || *src == '\n'))
    {
      notBlank = 0;
      *src = '\0';
    }
    else
    {
      if(!notBlank)
      {
        if(*src == '"' || *src == '\'')
        {
          quote = 1;
          args[count ++] = src + 1;
        }
        else
        {
          args[count ++] = src;
        }
      }
      else
      if(quote)
      {
        if(*src == '\\')
        {
          char *p;
          p = src;
          while(*p)
          {
            *p = *(p + 1);
            p ++;
          }
        }
        else
        if(*src == '"' || *src == '\'')
        {
          *src = '\0';
          quote = 0;
        }
      }

      notBlank = 1;
    }
    src ++;
  }
  return count;
}
/*----------------------------------------------------------------------------*/
static int call_cmd(int argc, char **argv)
{
  CONSOLE_CMD *c;

  if(!argc)
    return 0;

  c = root_cmd;
  while(c != NULL)
  {
    if(!strcmp(argv[0], c->cmd))
    {
      if(c->handler != NULL
         && CONSOLE_OK == c->handler(argc, argv)
         && c->state_handler != NULL)
      {
        user_handler = c->state_handler;
        console_state = USER_STATE;
      }
      return CONSOLE_OK;
    }
    c = c->next;
  }

  console_printf("Invalid command '%s'\r\nUse help command for help\r\n", argv[0]);
  return 1;
}
/*----------------------------------------------------------------------------*/
void console_add_cmd(CONSOLE_CMD *cmd)
{
  CONSOLE_CMD *c;

  c = root_cmd;

  if(root_cmd == NULL)
    root_cmd = cmd;
  else
  while(c != NULL)
  {
    if(c->next == NULL)
    {
      c->next = cmd;
      break;
    }
    c = c->next;
  }
  cmd->next = NULL;
}
/*----------------------------------------------------------------------------*/
static int help_cmd(int argc, char **argv)
{
  int i;
  CONSOLE_CMD *c;
  if(argc < 2)
  {
    console_printf("\r\nAvailable commands:\r\n");
    c = root_cmd;
    while(c != NULL)
    {
      console_printf("%-20s %s\r\n", c->cmd, c->help);
      while(uart_tx_size()) {} //Wait TX buffer empty
      c = c->next;
    }
  }
  else
    for(i = 1; i < argc; i++)
    {
      c = root_cmd;
      while(c != NULL)
      {
        if(strcmp(argv[i], c->cmd))
        {
          console_printf("%-20s %s\r\n", c->cmd, c->help);
          break;
        }
        c = c->next;
      }
      if(c == NULL)
      {
        console_printf("%-20s %s\r\n", argv[i], "Unknown cmd");
      }
    }
  return 0;
}
/*----------------------------------------------------------------------------*/
static int sleep_cmd(int argc, char **argv)
{
  int ms;
  if(argc < 2)
  {
    console_printf("\r\nUsing: sleep <ms>\r\n");
    return 1;
  }
  ms = str2int(argv[1]);
  if(ms > 0)
  {
    params.sleep_end = sys_tick_count() + ms;
    return CONSOLE_OK;
  }
  return 1;
}
/*----------------------------------------------------------------------------*/
