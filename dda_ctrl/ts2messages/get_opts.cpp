#include <get_opts.h>
#include <stdio.h>
#include <string.h>

using namespace std;

//---------------------------------------------------------------------------
void opt_args :: add(const std::string &long_opt, char short_opt, const std::string &def_value, OPT_TYPE t, const std::string &help_message)
{
  _OPTION_ARG arg;
  arg.opt_long = long_opt;
  arg.opt_short = short_opt;
  arg.opt_type = t;
  arg.opt_present = false;
  arg.def_value = def_value;
  arg.help_message = help_message;
  insert(pair<string, _OPTION_ARG>(long_opt, arg));
}
//---------------------------------------------------------------------------
int opt_args :: get(const std::string &long_opt, std::string *value)
{
  iterator i = find(long_opt);
  if(i == end())
    return 0;

  int result = (int) i->second.opt_values.size();
  if(value)
  {
    if(!i->second.opt_values.empty())
    {
      *value = i->second.opt_values[0];
      i->second.opt_values.erase(i->second.opt_values.begin());
    }
    else
      *value = i->second.def_value;
  }

  if(i->second.opt_present)
    return  i->second.opt_type == OPT_SWITCH ? 1 : result;

  return 0;
}
//---------------------------------------------------------------------------
bool opt_args :: parse(int argc, char **argv)
{
  empty_ = true;
  string last_opt;
  iterator im = end();
  non_options.clear();
  for(int i = 1; i < argc; i++)
  {
    empty_ = false;
    string opt = argv[i];
    if(opt.find("-") == 0)
    {
      if(im != end() && im->second.opt_type == OPT_VALUE)
      {
        error_ = "Invalid option '";
        error_ += last_opt;
        error_ += "' value '";
        error_ += opt;
        error_ += "'";
        return false;
      }
    }

    if(opt.find("--") == 0)
    {
      last_opt = opt.substr(2, opt.size() - 2);
      im = find(last_opt);
      if(im == end())
      {
        error_ = "Invalid option '";
        error_ += opt;
        error_ += "'";
        return false;
      }

      im->second.opt_present = true;
    }
    else
    if(opt.find("-") == 0)
    {
      for(unsigned j = 1; j < opt.size(); j++)
      {
        if(im != end() && im->second.opt_type == OPT_VALUE)
        {
          error_ = "Invalid option '";
          error_ += last_opt;
          error_ += "' value '";
          error_ += opt;
          error_ += "'";
          return false;
        }

        char opt_short = opt[j];
        for(im = begin(); im != end(); im++)
        {
          if(im->second.opt_short == opt_short)
            break;
        }

        if(im == end())
        {
          error_ = "Invalid option '";
          error_ += opt.substr(j,1);
          error_ += "'";
          return false;
        }

        last_opt = im->second.opt_long;
        im->second.opt_present = true;
      }
    }
    else
    {
      if(im == end() || im->second.opt_type != OPT_VALUE)
      {
        non_options.push_back(argv[i]);
      }
      else
      {
        im->second.opt_values.push_back(argv[i]);
        im->second.opt_present = true;
      }
      im = end();
    }

  }

  if(im != end() && im->second.opt_type == OPT_VALUE)
  {
    error_ = "Enough option '";
    error_ += last_opt;
    error_ += "' value";
    return false;
  }

  return true;
}
//---------------------------------------------------------------------------
std::string opt_args :: help()
{
  #define SCREEN_WIDTH 80
  #define MESSAGE_START 30

  string help;
  iterator im = begin();
  char buffer[1024];
  while(im != end())
  {
    sprintf(buffer, "--%s ", im->second.opt_long.c_str());
    if(im->second.opt_type == OPT_VALUE)
      sprintf(buffer + strlen(buffer), "<arg> ");

    if(im->second.opt_short)
    {
    	sprintf(buffer + strlen(buffer), "or -%c ", im->second.opt_short);
      if(im->second.opt_type == OPT_VALUE)
        sprintf(buffer + strlen(buffer), "<arg> ");
    }

    help += buffer;
//    int blanks = SCREEN_WIDTH - strlen(buffer) - im->second.help_message.size();
    int blanks = MESSAGE_START - strlen(buffer);
    if(blanks < 0) blanks = 0;
    if(blanks)
      help += string(blanks, ' ');
    help += im->second.help_message;
    help += string(1, '\n');

    im ++;
  }
  return help;
}
//---------------------------------------------------------------------------
bool parse_int_region(const std::string &src, region_t &dst)
{
  REGION_ELEM elem;
  int *cur = NULL;
  unsigned i;

  for(i = 0; i < src.size(); i++)
  {
    if(src[i] >= '0' && src[i] <= '9')
    {
      if(cur == NULL)
      {
        cur = &elem.from;
        elem.from = 0;
        elem.to = 0;
      }
      else
      if(cur == &elem.to && elem.to == -1)
        elem.to = 0;

      (*cur) *= 10;
      (*cur) += src[i] - '0';
      if(cur == &elem.from)
        elem.to = *cur;
    }
    else
    if(src[i] == ',')
    {
      if(cur != NULL)
      {
        if(elem.to == -1)
          elem.to = 0x7fffffff;
        dst.push_back(elem);
        cur = NULL;
      }
    }
    else
    if(src[i] == '-')
    {
      if(cur == NULL)
      {
        elem.from = 0;
        elem.to = -1;
        cur = &elem.to;
      }
      else
      if(cur == &elem.to)
        return false; //Error
      else
      {
        elem.to = -1;
        cur = &elem.to;
      }
    }
    else
    if(src[i] == ' ' || src[i] == '\t')
      continue;
    else
      return false; //Error
  }

  if(cur != NULL)
  {
    if(elem.to == -1)
      elem.to = 0x7fffffff;
    dst.push_back(elem);
  }

  return true;
}
//---------------------------------------------------------------------------
bool val_in_region(const region_t &r, int value)
{
  for(unsigned i = 0; i < r.size(); i++)
  {
    if(value >= r[i].from && value <= r[i].to)
      return true;
  }
  return false;
}
//---------------------------------------------------------------------------

