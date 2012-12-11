#include <stdio.h>
#include <stdlib.h>
#include <get_opts.h>
#include <pc_serial.h>
#include <boot_protocol.h>
#include <string.h>

#define PRODUCT "DDA code load utility"
#define VERSION "0.0.1"
#define COPYRIGHT "(C) DDA team 2012"
#define BUILD __DATE__
void help(char *argv0, opt_args *args);
void version();
//---------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int baud;
  std::string str, filename;

  opt_args args;
  args.add("help", 'h', "This help");
  args.add("version", 'V', "Type version and exit");
  args.add("verbose", 'v', "Verbose mode");
  args.add("check", 'c', "Check after write");
  args.add("check-only", 'C', "Check only (do not write)");
  args.add("port", 'p', "1", "Serial port");
  args.add("baud", 'b', "57600", "Serail baud");


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

  if(args.non_options.empty() || args.non_options.size() > 1)
  {
    //fprintf( stderr, "Invalid filename cpecification\n" );
    help(argv[0], &args);
    return 1;
  }

  if(args.empty())
  {
    version();
    printf("Use -h for help\n");
  }

  args.get("baud", &str);
  baud = atoi(str.c_str());

  args.get("port", &str);

  BootLoaderProtocol protocol;
  if(!protocol.open(str.c_str(), baud))
  {
    fprintf(stderr, "%s\n", protocol.errorString().c_str());
    return 2;
  }

  str = args.non_options[0];
  FILE *f = fopen(str.c_str(), "rb");
  if(f == NULL)
  {
    fprintf(stderr, "Error open file'%s'\n", str.c_str());
    return 2;
  }

  protocol.setVerbose(args.get("verbose"));
  if(!protocol.getVersionAndAllowedCmd())
  {
    fprintf(stderr, "Get command error: %s\n", protocol.errorString().c_str());
    return 3;
  }

  fseek(f, 0, SEEK_END);
  long fileSize = ftell(f);

  if(!args.get("check-only"))
  { //Erase flash
    unsigned short pageCount = fileSize / 1024;
    if(pageCount > 256)
    {
      fprintf(stderr, "Programm not support file grater then 256kb\n");
      return 2;
    }

    unsigned char pages[256];
    for(int i = 0; i < 256; i++)
      pages[i] = i;
    if(!protocol.eraseFlash(pageCount, pages))
    {
      fprintf(stderr, "Erase flash error: %s\n", protocol.errorString().c_str());
      return 3;
    }
  }

  unsigned char buffer[256];
  unsigned addr = 0x8000000;
  if(!args.get("check-only"))
  {
    fseek(f, 0, SEEK_SET);
    while(!feof(f))
    {
      int sz = fread(buffer, 1, 256, f);
      if(ferror(f))
      {
        fprintf(stderr, "Error read file'%s'\n", str.c_str());
        return 2;
      }

      if(!protocol.writeMemory(addr, sz, buffer))
      {
        fprintf(stderr, "Write memory error: %s\n", protocol.errorString().c_str());
        return 3;
      }
      if(protocol.verbose())
        printf("Write %d bytes Ok\n", sz);
      addr += sz;
    }
    printf("Writed %ld bytes Ok\n", ftell(f));
  }

  if(args.get("check-only") || args.get("check"))
  {
    unsigned char rBuffer[256];
    fseek(f, 0, SEEK_SET);
    addr = 0x8000000;
    while(!feof(f))
    {
      int sz = fread(buffer, 1, 256, f);
      if(ferror(f))
      {
        fprintf(stderr, "Error read file'%s'\n", str.c_str());
        return 2;
      }

      if(!protocol.readMemory(addr, sz, rBuffer))
      {
        fprintf(stderr, "Write memory error: %s\n", protocol.errorString().c_str());
        return 3;
      }

      if(protocol.verbose())
        printf("Read %d bytes Ok\n", sz);

      if(memcmp(buffer, rBuffer, sz))
      {
        fprintf(stderr, "Compare memory error at addr %08x\n", addr);
        return 3;
      }

      addr += sz;
    }
    printf("Verified %ld bytes Ok\n", ftell(f));
  }


  fclose(f);

  return 0;
}
//---------------------------------------------------------------------------
void help(char *argv0, opt_args *args)
{
  printf("%s V%s (build %s)\n", PRODUCT, VERSION, BUILD);
  printf("Using: %s [switches] <filename>\nWhere switches are:\n%s\n", argv0, args->help().c_str());
}
//---------------------------------------------------------------------------
void version()
{
  printf("%s V%s (build %s)\n%s\n", PRODUCT, VERSION, BUILD, COPYRIGHT);
}
//---------------------------------------------------------------------------

