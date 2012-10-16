/*--------------------------------------------------------------------------
NAFTA POS T&T(R)
(C) TiT, kiev, Ukraine 2002
www.tit.kiev.ua tit@diawest.net.ua
DESCRIPTION: class for NAFTA POS logfiles support
AUTHOR: Oleg Yurchenko
--------------------------------------------------------------------------*/
//#define DEBUG
#include <logfile.h>
#include <stdio.h>
//#include <sys/stdtypes.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifdef UNIX
#define DIRCHAR '/'
#define DIRSTR "/"
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#else
#define DIRCHAR '\\'
#define DIRSTR "\\"
#include <dir.h>
#endif
//---------------------------------------------------------------------------
#ifdef UNIX
static unsigned GetTickCount();
static void Sleep(unsigned ms);
#endif

#ifdef WIN32
#include <windows.h>
#endif
//---------------------------------------------------------------------------
int n_vsnprintf(char* str, size_t size, const char* fmt, va_list ap);
bool write_log(const char *file_name, unsigned long max_size, bool create_flag, const char* fmt, va_list ap);
/*------------------------------------------------------------------------*/
logfile :: logfile(const char *file_name, bool create, unsigned long max_size)
{
  SetFileName(file_name);
	CreateFlag = create;
  MaxSize = max_size;

}
/*------------------------------------------------------------------------*/
logfile :: ~logfile()
{
}
/*------------------------------------------------------------------------*/
void logfile :: SetFileName(const char *file_name)
{
	if(!strchr(file_name,DIRCHAR))
	{
  	getcwd(FileName,sizeof(FileName));

		if(!strlen(FileName) || FileName[strlen(FileName) - 1] != DIRCHAR)
			strcat(FileName,DIRSTR);
		strcat(FileName, file_name);
	}
  else
		strcpy(FileName, file_name);
}
/*------------------------------------------------------------------------*/
void logfile :: SetCreateFlag(bool create_flag)
{
	CreateFlag = create_flag;

}
/*------------------------------------------------------------------------*/
void logfile :: SetMaxSize(unsigned long max_size)
{
  MaxSize = max_size;
}
/*------------------------------------------------------------------------*/
bool logfile :: Info(const char *fmt, ...)
{
	char buffer[1024];
	int len = 0;
  len += sprintf(buffer,"I\t");
	time_t t = time(NULL);
  struct tm *stm = localtime(&t);
	len += strftime(buffer + len,sizeof(buffer) - len,"%d.%m.%y %H:%M:%S",stm);
  len += sprintf(buffer + len,"(%03lu)", GetTickCount()  % 1000 );
	sprintf(buffer + len, "\t%s",fmt);

  va_list ap;

  va_start(ap, fmt);

	bool result = write_log(FileName,MaxSize,CreateFlag,buffer,ap);
  va_end(ap);
  return result;

}
/*------------------------------------------------------------------------*/
bool logfile :: Message(const char *fmt, ...)
{
	char buffer[1024];
	int len = 0;
  len += sprintf(buffer,"M\t");
	time_t t = time(NULL);
  struct tm *stm = localtime(&t);
	len += strftime(buffer + len,sizeof(buffer) - len,"%d.%m.%y %H:%M:%S",stm);
  len += sprintf(buffer + len,"(%03lu)", GetTickCount()  % 1000 );
	sprintf(buffer + len, "\t%s",fmt);

  va_list ap;
  va_start(ap, fmt);
	bool result = write_log(FileName,MaxSize,CreateFlag,buffer,ap);
  va_end(ap);

  //Print message
	va_start(ap, fmt);
  n_vsnprintf(buffer, sizeof(buffer), fmt, ap);
  va_end(ap);
  //fprintf(stderr,"%s\n",buffer);

	return result;
}
/*------------------------------------------------------------------------*/
bool logfile :: Exception(const char *fmt, ...)
{
	char buffer[1024];
	int len = 0;
  len += sprintf(buffer,"E\t");
	time_t t = time(NULL);
  struct tm *stm = localtime(&t);
	len += strftime(buffer + len,sizeof(buffer) - len,"%d.%m.%y %H:%M:%S",stm);
  len += sprintf(buffer + len,"(%03lu)", GetTickCount()  % 1000 );
	sprintf(buffer + len, "\t%s",fmt);

  va_list ap;
  va_start(ap, fmt);
	bool result = write_log(FileName,MaxSize,CreateFlag,buffer,ap);
  va_end(ap);

  //Print message
	va_start(ap, fmt);
  n_vsnprintf(buffer, sizeof(buffer), fmt, ap);
  va_end(ap);
  //fprintf(stderr,"%s\n",buffer);

	return result;
}
/*------------------------------------------------------------------------*/
bool write_log(const char *file_name, unsigned long max_size, bool create_flag, const char* fmt, va_list ap)
{
	char buffer[1024];
  int ret;
  ret = n_vsnprintf(buffer, sizeof(buffer), fmt, ap);
	if(ret > 0)
	{
		if(buffer[ret - 1] != '\n' && ret < sizeof(buffer) - 1)
			strcat(buffer,"\n");

		FILE *f = fopen(file_name, create_flag ? "a+" : "r+");
		if(!f)
			{
        //TRACE1("Can't open logfile %s\n", file_name);
				return false;
			}

		fseek(f,0,SEEK_END);
		long size = ftell(f);
		if(size == -1)
		{
      //TRACE1("Can't tell logfile size %s\n", file_name);
			fclose(f);
			return false;
		}

		if((unsigned long) size > max_size)
		{
			fclose(f);
			f = fopen(file_name, "w");
			if(!f)
			{
        //TRACE1("Can't open logfile %s\n", file_name);
				return false;
			}
		}

		if(fwrite(buffer,1,strlen(buffer),f) == -1)
    {
      //TRACE1("Can't write logfile %s\n", file_name);
    }

    fflush(f);
		fclose(f);
    //TRACE3("log:%s:%u %s",file_name,size,buffer);
		return true;
	}

return false;
}
/*------------------------------------------------------------------------*/
int n_vsnprintf(char* str, size_t size, const char* fmt, va_list ap) {
  const char* e = str+size-1;
  char* p = str;
  char copy[20];
  char* copy_p;
  char sprintf_out[100];

  while (*fmt && p < e) {
    if (*fmt != '%') {
      *p++ = *fmt++;
    } else {
      fmt++;
      copy[0] = '%';
      for (copy_p = copy+1; copy_p < copy+19;) {
	switch ((*copy_p++ = *fmt++)) {
	case 0:
	  fmt--; goto CONTINUE;
	case '%':
	  *p++ = '%'; goto CONTINUE;
	case 'c':
	  *p++ = va_arg(ap, int);
	  goto CONTINUE;
	case 'd':
	case 'i':
	case 'o':
	case 'u':
	case 'x':
	case 'X':
	  *copy_p = 0;
	  sprintf(sprintf_out, copy, va_arg(ap, int));
	  copy_p = sprintf_out;
	  goto DUP;
	case 'e':
	case 'E':
	case 'f':
	case 'g':
	  *copy_p = 0;
	  sprintf(sprintf_out, copy, va_arg(ap, double));
	  copy_p = sprintf_out;
	  goto DUP;
	case 'p':
	  *copy_p = 0;
	  sprintf(sprintf_out, copy, va_arg(ap, void*));
	  copy_p = sprintf_out;
	  goto DUP;
	case 'n':
	  *(va_arg(ap, int*)) = p-str;
	  goto CONTINUE;
	case 's':
	  copy_p = va_arg(ap, char*);
	  if (!copy_p) copy_p = "NULL";
	DUP:
	  while (*copy_p && p < e) *p++ = *copy_p++;
	  goto CONTINUE;
	}
      }
    }
  CONTINUE:;
  }
  *p = 0;
  if (*fmt) return -1;
  return p-str;
}
/*------------------------------------------------------------------------*/
#ifdef UNIX
static void LastSystemErrorMessage(char *str, int MaxStrLength, bool oem)
{
if(strlen(strerror(errno)) < MaxStrLength)
  strcpy(str,strerror(errno));
else
  {
  memcpy(str,strerror(errno),MaxStrLength-2);
  str[MaxStrLength-1] = 0;
  }
}

static unsigned GetTickCount()
{
	timeval tv;
	gettimeofday(&tv,NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static void Sleep(unsigned ms)
{
usleep(ms * 1000);
}
//---------------------------------------------------------------------------
#endif




