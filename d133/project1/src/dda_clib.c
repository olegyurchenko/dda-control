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
#define NO_CANONICAL_NAMES
#include "dda_clib.h"
#include <stdarg.h>
/*----------------------------------------------------------------------------*/
typedef int (*writer_t)(void *, const char *, int);
/*----------------------------------------------------------------------------*/
#ifndef ABS
#define ABS(a)              (((a) < 0) ? (- (a)) : (a))
#endif
/*----------------------------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif
/*----------------------------------------------------------------------------*/
#define isdigit(c) ((c) >= '0' && (c) <= '9')
/*----------------------------------------------------------------------------*/
int sys_strcmp(const void *v1, const void *v2)
{
  const char *s1, *s2;
  s1 = (const char *)v1;
  s2 = (const char *)v2;

  while(*s1 && *s2)
  {
    if(*s1 != *s2)
      return *s1 - *s2;
    s1 ++;
    s2 ++;
  }
  return *s1 - *s2;
}
/*----------------------------------------------------------------------------*/
int sys_strlen(const void *v)
{
  const char *s;
  s = (const char *)v;

  while(*s)
    s ++;

  return s - (const char *)v;
}
/*----------------------------------------------------------------------------*/
const char *sys_strchr(const char *s, int c)
{
  while(*s)
  {
    if(*s == c)
      return s;
    s ++;
  }

  return 0;
}
/*----------------------------------------------------------------------------*/
void sys_memset(void *dst, int c, unsigned size)
{
  char *s;
  s = (char *)dst;
  while(size --)
    *(s ++) = c;
}
/*----------------------------------------------------------------------------*/
void sys_memcpy(void *dst, const void *src, unsigned size)
{
  char *d;
  const char *s;
  d = (char *)dst;
  s = (const char *)src;
  while(size --)
  {
    *d = *s;
    d ++; s ++;
  }
}
/*----------------------------------------------------------------------------*/
int sys_memcmp(const void *m1, const void *m2, unsigned size)
{
  const char *s1, *s2;
  s1 = (const char *)m1;
  s2 = (const char *)m2;

  while(size --)
  {
    if(*s1 != *s2)
      return *s1 - *s2;
    s1 ++;
    s2 ++;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
int sys_atoi(const void *v)
{
  const char *s;
  int i = 0, m = 1;
  s = (const char *)v;

  if(*s == '-')
  {
    m = -1;
    s ++;
  }
  else
  if(*s == '+')
    s++;

  while(*s)
  {
    if(*s >= '0' && *s <= '9')
    {
      i *= 10;
      i += *s - '0';
    }
    else
      break;
    s ++;
  }

  return i * m;
}
/*----------------------------------------------------------------------------*/
int sys_atoi8(const void *v)
{
  const char *s;
  int i = 0, m = 1;
  s = (const char *)v;

  if(*s == '-')
  {
    m = -1;
    s ++;
  }
  else
  if(*s == '+')
    s++;

  while(*s)
  {
    if(*s >= '0' && *s <= '7')
    {
      i *= 8;
      i += *s - '0';
    }
    else
      break;
    s ++;
  }

  return i * m;
}
/*----------------------------------------------------------------------------*/
int sys_atoi16(const void *v)
{
  const char *s;
  int i = 0, m = 1;
  s = (const char *)v;

  if(*s == '-')
  {
    m = -1;
    s ++;
  }
  else
  if(*s == '+')
    s++;

  while(*s)
  {
    if(*s >= '0' && *s <= '9')
    {
      i *= 16;
      i += *s - '0';
    }
    else
    if(*s >= 'A' && *s <= 'F')
    {
      i *= 16;
      i += *s - 'A' + 0xa;
    }
    else
    if(*s >= 'a' && *s <= 'f')
    {
      i *= 16;
      i += *s - 'a' + 0xa;
    }
    else
      break;
    s ++;
  }

  return i * m;
}
/*----------------------------------------------------------------------------*/
int sys_str2int(const char *s)
{
  int m = 1;
  if(*s == '-')
  {
    m = -1;
    s ++;
  }
  else
  if(*s == '+')
    s++;

  if(*s == '0')
  {
    s ++;
    if(*s == 'x' || *s == 'X')
      return m * sys_atoi16(s + 1);
    else
    if(*s)
      return m * sys_atoi8(s + 1);
    else
      return 0;
  }
  else
  if(*s == 'x' || *s == 'X')
    return m * sys_atoi16(s + 1);

  return m * sys_atoi(s);
}
/*----------------------------------------------------------------------------*/
static int write_str(writer_t write, void *data, const char *src, int before, int after)
{
  int l, r = 0;
  char buf = ' ';
  l = sys_strlen(src);
  while(l < before)
  {
    r += write(data, &buf, 1);
    before --;
  }
  r += write(data, src, l);
  while(l < after)
  {
    r += write(data, &buf, 1);
    after --;
  }

  return r;
}
/*----------------------------------------------------------------------------*/
static int write_int(writer_t write, void *data, char spec, unsigned src, int width, char blank)
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
  if(width < 0)
  {
    width = - width;
    r += write(data, &buffer[i], len);
    while(len < width)
    {
      r += write(data, &blank, 1);
      width --;
    }
  }
  else
  {
    while(len < width)
    {
      r += write(data, &blank, 1);
      width --;
    }
    r += write(data, &buffer[i], len);
  }
  return r;
}
/*----------------------------------------------------------------------------*/
int sys_nvprintf(writer_t write, void *data, const char *fmt, va_list ap)
{
  char spec[20];
  int r = 0, i, brk;
  const char *fmt0;

  fmt0 = fmt;
  while(*fmt)
  {
    if(*fmt == '%')
    {
      r += write(data, fmt0, fmt - fmt0);
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
          r += write(data, fmt, 1);
          break;
        case 'c':
          spec[0] = va_arg(ap, int);
          r += write(data, spec, 1);
          break;

        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          r += write_int(write, data, *fmt, va_arg(ap, unsigned), sys_atoi(spec), spec[0] == '0' ? '0' : ' ');
          break;
        case 's':
          i = sys_atoi(spec);
          r += write_str(write, data, va_arg(ap, char*), i > 0 ? i : 0, i < 0 ? -i : 0);
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

  if(fmt > fmt0)
    r += write(data, fmt0, fmt - fmt0);
  return r;
}
/*----------------------------------------------------------------------------*/
typedef struct
{
  char *buf;
  int size;
  int len;
} buffer_data_t;
/*----------------------------------------------------------------------------*/
static int buf_write(void *d, const char *txt, int len)
{
  buffer_data_t *data;
  int r = 0;
  data = (buffer_data_t *)d;
  while(len > 0 && (!data->size || data->size > data->len))
  {
    data->buf[data->len] = *txt;
    data->len ++;
    r++;
    len--;
    txt ++;
  }
  return r;
}
/*----------------------------------------------------------------------------*/
int sys_sprintf(char *buf, const char *fmt, ...)
{
  buffer_data_t data;
  va_list ap;
  int r;

  if(!buf)
    return 0;

  va_start(ap, fmt);
  data.buf = buf;
  data.len = 0;
  data.size = 0;
  r = sys_nvprintf(buf_write, &data, fmt, ap);
  va_end(ap);
  buf[r] = '\0';
  return r;
}
/*----------------------------------------------------------------------------*/
int sys_snprintf(char *buf, unsigned buf_size, const char *fmt, ...)
{
  buffer_data_t data;
  va_list ap;
  int r;

  if(!buf_size || !buf)
    return 0;

  va_start(ap, fmt);

  data.buf = buf;
  data.len = 0;
  data.size = buf_size;
  r = sys_nvprintf(buf_write, &data, fmt, ap);
  va_end(ap);
  if(r < buf_size)
    buf[r] = '\0';
  else
    buf[buf_size - 1] = '\0';
  return r;
}
/*----------------------------------------------------------------------------*/

