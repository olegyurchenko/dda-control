#include <stdio.h>
#include <get_opts.h>
#include <dda_protocol.h>
#include <stdlib.h>
#include <string.h>
#include <map>
/*----------------------------------------------------------------------------*/
#define PRODUCT "DDA setting read/load utility"
#define VERSION "0.0.1"
#define COPYRIGHT "(C) DDA team 2013"
#define BUILD __DATE__
void help(char *argv0, opt_args *args);
void version();
/*----------------------------------------------------------------------------*/
typedef enum
{
  ReadMode,
  WriteMode
} SettingsMode;
/*----------------------------------------------------------------------------*/
SettingsMode mode;
/*----------------------------------------------------------------------------*/
typedef std::map<std::string, std::string> StringMap;
StringMap settings;
/*----------------------------------------------------------------------------*/
class DDASettingsProtocol : public DDAProtocol
{
protected:
  virtual void handleData(const ByteVector&);
public:
  DDASettingsProtocol() : DDAProtocol() {}
};
/*----------------------------------------------------------------------------*/
static void parse_file(FILE *f);
static void output_settings(FILE *f);
/*----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
  int baud;
  std::string str, filename;

  opt_args args;
  args.add("help", 'h', "This help");
  args.add("version", 'V', "Type version and exit");
  args.add("port", 'p', "1", "Serial port");
  args.add("baud", 'b', "9600", "Serail baud");
  args.add("clean", 'c', "Clean settings before write");
  args.add("read", 'r', "Read settings from device (default mode)");
  args.add("write", 'w', "Write settings to device");


  if(!args.parse(argc, argv))
  {
    fprintf(stderr, "%s\n", args.error().c_str());
    return 1;
  }

  if(args.get("help"))
  {
    help(argv[0], &args);
    return 0;
  }

  if(args.get("version"))
  {
    version();
    return 0;
  }


  if(args.empty())
  {
    version();
    printf("Use -h for help\n");
  }

  args.get("baud", &str);
  baud = atoi(str.c_str());

  args.get("port", &str);

  DDASettingsProtocol protocol;
  if(!protocol.open(str.c_str(), baud))
  {
    fprintf(stderr, "Error open serial port\n");
    return 2;
  }

  FILE *f;
  if(args.get("write"))
  {
    mode = WriteMode;
    if(!args.non_options.empty())
    {
      str = args.non_options[0];
      f = fopen(str.c_str(), "rb");
      if(f == NULL)
      {
        fprintf(stderr, "Error open file'%s'\n", str.c_str());
        return 2;
      }
      parse_file(f);
      fclose(f);
      DDAProtocol::ByteVector data;
      if(args.get("clean"))
      {
        data.clear();
        data.push_back(1); //Clear instruction
        protocol.pushData('W', data);
      }

      for(StringMap::iterator it = settings.begin(); it != settings.end(); ++it)
      {
        unsigned sz;
        std::string str;
        data.clear();
        data.push_back(0); //Set setting instruction
        str = it->first;
        sz = str.size();
        for(unsigned i = 0; i < sz; i++)
          data.push_back(str[i]);
        data.push_back('=');
        str = it->second;
        sz = str.size();
        for(unsigned i = 0; i < sz; i++)
          data.push_back(str[i]);
        protocol.pushData('W', data);
      }
      data.clear();
      data.push_back(2); //Write to flash instruction
      protocol.pushData('W', data);

    }
    else
    {
      fprintf(stderr, "Missing parameters filename\n");
      return 1;
    }
  }
  else
  {
    mode = ReadMode;
    f = stdout;

    protocol.pushData('R');
  }

  if(!protocol.exec())
  {
    fprintf(stderr, "Communication error\n");
    return 2;
  }

  if(mode == ReadMode)
    output_settings(f);

  return 0;
}
/*----------------------------------------------------------------------------*/
void help(char *argv0, opt_args *args)
{
  printf("%s V%s (build %s)\n", PRODUCT, VERSION, BUILD);
  printf("Using: %s [switches] [<filename>]\nWhere switches are:\n%s\n", argv0, args->help().c_str());
}
//---------------------------------------------------------------------------
void version()
{
  printf("%s V%s (build %s)\n%s\n", PRODUCT, VERSION, BUILD, COPYRIGHT);
}
//---------------------------------------------------------------------------
void DDASettingsProtocol::handleData(const ByteVector& data)
{
  if(mode == WriteMode)
  {
    if(m_txData.empty())
      m_terminated = true;
  }
  else
  {
    if(data[1] == 'R' && data[2] > 3)
    {
      std::string name, value;
      std::string *s = &name;
      unsigned size = data[2] - 3;
      for(unsigned i = 0; i < size; i++)
      {
        unsigned char c = data[3 + i];
        if(c == '=')
        {
          s = &value;
          continue;
        }
        s->push_back(c);
      }
      settings[name] = value;
    }
    else
    if(data[1] == 'R')
    {
      m_terminated = true;
    }
  }
}
//---------------------------------------------------------------------------
static const char *get_word(const char *src, char *dst, unsigned size)
{
  int i = 0;
  int str = 0;
  if(size && dst)
    *dst = 0;

  //printf("Parse word '%s'\n", src);
  while(*src && i < (int)size - 1)
  {
    switch(*src)
    {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        if(i && !str)
        {
          if(dst)
            dst[i] = 0;
          return src;
        }

        if(dst && str)
          dst[i ++] = *src;
        break;

      case '\'':
      case '"':
        if(str)
          {
            dst[i] = 0;
            return ++src;
          }
        str = !str;
        break;
      case '\\':
        src ++;
        if(*src && str)
        {
          char c = *src;
          switch(*src)
          {
            case 'a':
              c = '\a';
              break;
            case 'b':
              c = '\b';
              break;
            case 'f':
              c = '\f';
              break;
            case 'n':
              c = '\n';
              break;
            case 'r':
              c = '\r';
              break;
            case 't':
              c = '\t';
              break;
            case 'v':
              c = '\v';
              break;
          }

          if(dst)
            dst[i ++] = c;
        }
        break;

      default:
        if(dst)
          dst[i ++] = *src;
        break;
    } /*case*/

    src ++;
  } /*while*/

  if(size && dst)
    dst[i] = 0;

  return src;
}
/*----------------------------------------------------------------------------*/
int parse_assign(const char *src, char *name, unsigned nsize, char *value, unsigned vsize)
{
  const char *c = strchr(src, '=');
  if(!c)
    return -1;

  get_word((char *)src, name, (c - src + 1) > (int)nsize ? nsize : (c - src) + 1);
  if(!strlen(name))
    return -1;

  if(vsize && value)
    value[0] = 0;

  get_word(c+1, value, vsize);

  return 0;
}
/*----------------------------------------------------------------------------*/
static void parse_file(FILE *f)
{
  char buffer[1024], name[32], value[32];
  int index = 0;
  while(!feof(f))
  {
    int c = fgetc(f);
    if(c == '\n' || c == '\r')
    {
      buffer[index] = '\0';
      if(index)
      {
        if(!parse_assign(buffer, name, sizeof(name), value, sizeof(value)))
        {
          printf("%s=%s\n", name, value);
          settings[name] = value;
        }
      }
      index = 0;
      continue;
    }

    if(index < (int)sizeof(buffer) - 2)
    {
      buffer[index] = c;
      if(c == '#') //Comment char
        buffer[index] = '\0';
      index ++;
    }
  }
}
//---------------------------------------------------------------------------
static void output_settings(FILE *f)
{
  for(StringMap::iterator it = settings.begin(); it != settings.end(); ++it)
  {
    fprintf(f, "%s=%s\n", it->first.c_str(), it->second.c_str());
  }
}
//---------------------------------------------------------------------------

