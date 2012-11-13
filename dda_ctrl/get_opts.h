//Get opts
#ifndef GET_OPTS_H
#define GET_OPTS_H

#include <string>
#include <map>
#include <vector>
//---------------------------------------------------------------------------
enum OPT_TYPE
{
  OPT_SWITCH = 0,
  OPT_VALUE = 1
};

typedef struct
{
  std::string opt_long;
  char opt_short;
  OPT_TYPE opt_type;
  std::vector<std::string> opt_values;
  std::string def_value;
  std::string help_message;
  bool opt_present;
} _OPTION_ARG;

//---------------------------------------------------------------------------
typedef std::map <std::string, _OPTION_ARG> _OPTION_MAP;
class opt_args : public _OPTION_MAP
{
protected:
  std::string error_;
  bool empty_;
public:
  void add(const std::string &long_opt, char short_opt, const std::string &def_value, OPT_TYPE t, const std::string &help_message);

  void add(const std::string &long_opt, char short_opt,
    const std::string &help_message)
  {
    add(long_opt, short_opt, "", OPT_SWITCH, help_message);
  }

  void add(const std::string &long_opt, char short_opt, const std::string &def_value,
    const std::string &help_message)
  {
    add(long_opt, short_opt, def_value, OPT_VALUE, help_message);
  }

  int get(const std::string &long_opt, std::string *value = NULL);

  bool parse(int argc, char **argv);
  std::string error() {return error_;}
  std::string help();
  std::vector<std::string> non_options;
  //Return true if noone option was found while parse
  bool empty() {return empty_;}
};
//---------------------------------------------------------------------------
//Parse region like: 1,2-5,3,6-,-8
struct REGION_ELEM
{
  int from;
  int to;
  REGION_ELEM(int f,int t) {from = f; to = t; }
  REGION_ELEM() {from = 0; to = 0; }
};

typedef std::vector <REGION_ELEM> region_t;
bool parse_int_region(const std::string &src, region_t &dst);
bool val_in_region(const region_t &r, int value);
//---------------------------------------------------------------------------
#endif
