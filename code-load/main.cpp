#include <stdio.h>
#include <stdlib.h>
#include <get_opts.h>
#include <pc_serial.h>
#include <boot_protocol.h>

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

  /*
  if(args.non_options.empty() || args.non_options.size() > 1)
  {
    //fprintf( stderr, "Invalid filename cpecification\n" );
    help(argv[0], &args);
    return 1;
  }
  */

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

  protocol.setVerbose(args.get("verbose"));
  if(!protocol.getVersionAndAllowedCmd())
  {
    fprintf(stderr, "Get command error: %s\n", protocol.errorString().c_str());
    return 3;
  }
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

