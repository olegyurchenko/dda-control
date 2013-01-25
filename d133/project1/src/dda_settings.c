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
#include "dda_settings.h"
#include <crc8.h>
#include <dda_clib.h>
#include <sys_flash.h>
#include <dda_flash.h>
#include <dda_config.h>
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
#include <console.h>
#include <sys_timer.h>

#define DEBUG

static CONSOLE_CMD console1, console2, console3, console4;
static int cmd1(int argc, char **argv);
static int cmd2(int argc, char **argv);
static int cmd3(int argc, char **argv);
static int cmd4(int argc, char **argv);

#endif
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
#define TRACE         console_printf
#else
#define TRACE(...)
#endif /**< #ifdef DEBUG */
/*----------------------------------------------------------------------------*/
#define MAX_SETTING_COUNT 32
static DDA_SETTING m_settings[MAX_SETTING_COUNT];
static int m_setting_count = 0;
static int m_setting_modified = 0;
/*----------------------------------------------------------------------------*/
static unsigned name_hash(const char *str)
{
  unsigned hash = 5381;
  int c;

  /*printf("hash '%s' = ", str);*/
  while((c = *str++) != 0)
      hash = ((hash << 5) + hash) + tolower(c); /* hash * 33 + c */

  /*printf("%08x\n", hash);*/
  return hash;
}
/*----------------------------------------------------------------------------*/
void settings_init()
{
#ifdef USE_CONSOLE
  console_cmd_init(&console1);
  console1.cmd = "set";
  console1.help = "set <name> <value> - Set setting value";
  console1.handler = cmd1;
  console_add_cmd(&console1);

  console_cmd_init(&console2);
  console2.cmd = "get";
  console2.help = "get <name> - Get setting value";
  console2.handler = cmd2;
  console_add_cmd(&console2);

  console_cmd_init(&console3);
  console3.cmd = "read";
  console3.help = "read - Read settings from flash";
  console3.handler = cmd3;
  console_add_cmd(&console3);

  console_cmd_init(&console4);
  console4.cmd = "write";
  console4.help = "write - Write settings to flash";
  console4.handler = cmd4;
  console_add_cmd(&console4);

#endif //USE_CONSOLE
}
/*----------------------------------------------------------------------------*/
int setting_count()
{
  return m_setting_count;
}
/*----------------------------------------------------------------------------*/
const DDA_SETTING *setting(int index)
{
  if(index >= 0 && index < m_setting_count)
    return &m_settings[index];
  return 0;
}
/*----------------------------------------------------------------------------*/
int setting_modified()
{
  return m_setting_modified;
}
/*----------------------------------------------------------------------------*/
void setting_set_modified(int m)
{
  m_setting_modified = m;
}
/*----------------------------------------------------------------------------*/
void settings_clear()
{
  m_setting_modified = m_setting_modified | m_setting_count;
  m_setting_count = 0;
}
/*----------------------------------------------------------------------------*/
int settings_read()
{
  char buffer[sizeof(DDA_SETTING) + 4];
  int i, index = 0, sz;
  unsigned char crc;

  sflash_open(O_RDONLY);
#if(SETTINGS_OFFSET != 0)
  sflash_seek(SETTINGS_OFFSET);
#endif
  m_setting_count = 0;
  for(i = 0; i < MAX_SETTING_COUNT; i++)
  {
    crc = 0;
    if(sflash_read(&buffer[0], 1) != 1 || /*buffer[0] < 0 ||*/ buffer[0] >= (int) sizeof(m_settings[0].name))
      break;
    crc = get_crc_8(crc, &buffer[0], 1);
    index = buffer[0];
    if(sflash_read(&buffer[0], index) != index)
      break;
    crc = get_crc_8(crc, &buffer[0], index);
    buffer[index ++] = '\0';
    if(sflash_read(&buffer[index], 1) != 1 || /*buffer[index] < 0 ||*/ buffer[index] >= (int) sizeof(m_settings[0].value))
      break;
    crc = get_crc_8(crc, &buffer[index], 1);
    sz = buffer[index];
    if(sflash_read(&buffer[index], sz) != sz)
      break;
    crc = get_crc_8(crc, &buffer[index], sz);
    if(sflash_read(&buffer[index + sz], 1) != 1)
      break;
    if((unsigned char)buffer[index + sz] != crc)
    {
      TRACE("read_settings Crc error %02x != %02x\r\n", (unsigned char)buffer[sz], crc);
      break;
    }
    buffer[index + sz] = '\0';
    setting_set(buffer, &buffer[index]);
  }

  sflash_close();
  m_setting_modified = 0;
  return m_setting_count;
}
/*----------------------------------------------------------------------------*/
int settings_write()
{
  char buffer[sizeof(DDA_SETTING) + 4];
  int i, index = 0;

  sflash_open(O_WRONLY);
#if(SETTINGS_OFFSET != 0)
  sflash_seek(SETTINGS_OFFSET);
#endif

  for(i = 0; i < m_setting_count; i++)
  {
    index = snprintf(buffer, sizeof(buffer), "%c%s%c%s",
                     strlen(m_settings[i].name),
                     m_settings[i].name,
                     strlen(m_settings[i].value),
                     m_settings[i].value
                     );
    buffer[index] =  get_crc_8(0, buffer, index);
    index ++;
    if(sflash_write(buffer, index) != index)
    {
      TRACE("write_settings write error\r\n");
      break;
    }
  }

  memset(buffer, 0xff, sizeof(buffer));
  sflash_write(buffer, sizeof(buffer));
  sflash_close();
  m_setting_modified = 0;

  return i;
}
/*----------------------------------------------------------------------------*/
const char* setting_get(const char *name)
{
  static char zero = '\0';
  int i;
  unsigned h;

  h = name_hash(name);
  for(i = 0; i < m_setting_count; i++)
  {
    if(m_settings[i].hname == h)
      return m_settings[i].value;
  }
  return &zero;
}
/*----------------------------------------------------------------------------*/
void setting_set(const char *name, const char *value)
{
  int i, sz;
  unsigned h;

  h = name_hash(name);
  for(i = 0; i < m_setting_count; i++)
  {
    if(m_settings[i].hname == h)
    {
      if(strcmp(m_settings[i].value, value))
      {
        sz = strlen(value);
        if(sz >= (int)sizeof(m_settings[i].value))
          sz = sizeof(m_settings[i].value) - 1;
        memcpy(m_settings[i].value, value, sz);
        m_settings[i].value[sz] = '\0';
        TRACE("\r\nModify: '%s'='%s'=%u", m_settings[i].name, m_settings[i].value, m_settings[i].hname);
        m_setting_modified = 1;
      }
      return;
    }
  }

  if(m_setting_count < MAX_SETTING_COUNT - 1)
  {
    sz = strlen(name);
    if(sz >= (int)sizeof(m_settings[m_setting_count].name))
      sz = sizeof(m_settings[m_setting_count].name) - 1;
    memcpy(m_settings[m_setting_count].name, name, sz);
    m_settings[m_setting_count].name[sz] = '\0';
    m_settings[m_setting_count].hname = name_hash(m_settings[m_setting_count].name);
    sz = strlen(value);
    if(sz >= (int)sizeof(m_settings[m_setting_count].value))
      sz = sizeof(m_settings[m_setting_count].value) - 1;
    memcpy(m_settings[m_setting_count].value, value, sz);
    m_settings[m_setting_count].value[sz] = '\0';
    TRACE("\r\nSet new: '%s'='%s'=%u", m_settings[m_setting_count].name, m_settings[m_setting_count].value, m_settings[m_setting_count].hname);
    m_setting_count ++;
    m_setting_modified = 1;
  }
}
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
static int cmd1(int argc, char **argv)
{
  if(argc < 3)
  {
    console_printf("\r\nName and value parameter omitted");
    return 1;
  }
  setting_set(argv[1], argv[2]);
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int cmd2(int argc, char **argv)
{
  if(argc < 2)
  {
    console_printf("\r\nName parameter omitted");
    return 1;
  }
  console_printf("\r\n%s=`%s'", argv[1], setting_get(argv[1]));

  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int cmd3(int argc, char **argv)
{
  unsigned tm;
  (void) argc; //Prevent unused warning
  (void) argv;

  tm = sys_tick_count();
  console_printf("\r\nRead resiult:%d Elapsed %u ms", settings_read(), sys_tick_count() - tm);
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int cmd4(int argc, char **argv)
{
  unsigned tm;
  (void) argc; //Prevent unused warning
  (void) argv;

  tm = sys_tick_count();
  console_printf("\r\nWrite resiult:%d Elapsed %u ms", settings_write(), sys_tick_count() - tm);
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
#endif //USE_CONSOLE
