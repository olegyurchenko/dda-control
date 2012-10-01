/*----------------------------------------------------------------------------*/
/*"$Id$"*/
/**
* @pkg pc_serial
*/
/**
* Cross platform (windows && linux) serial port API (for PC).
*
* (C) T&T, Kiev, Ukraine 2004.
* start 12.11.04 10:01:16
* @pkgdoc pc_serial
* @author Oleg Yurchenko
* @version $Revision:   1.0  $
*/
/*----------------------------------------------------------------------------*/
#undef UNICODE
#include "pc_serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

#if !defined(WIN32) && !defined(LINUX)

  //Check for known compilers
  #if defined(_MSC_VER) || defined(__VISUALC__) || defined(__BORLANDC__) || defined(__MINGW32__)
    #define WIN32
  #else
    #if defined(__CYGWIN__)
      #define LINUX
    #endif
    #if linux
      #define LINUX
    #endif
  #endif

  #if !defined(WIN32) && !defined(LINUX)
    #error Unknown platform (not defined LINUX or WIN32)
  #endif
#endif

#ifdef WIN32
#include <windows.h>
/*----------------------------------------------------------------------------*/
#define MAX_PORT_COUNT 8
HANDLE handles[MAX_PORT_COUNT] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
/*----------------------------------------------------------------------------*/
static HANDLE get_h(int fd)
{
  if(fd < 100 || fd >= 100 + MAX_PORT_COUNT)
    return NULL;

  return handles[fd - 100];
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
static void err_trace(char *file, int line)
{
  if(GetLastError() != NOERROR)
	{
    LPVOID lpMsgBuf;

    FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
         NULL,
         GetLastError(),
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
         (LPTSTR) &lpMsgBuf,
         0,
         NULL
      );

     // Display the string.

     if(lpMsgBuf)
     {
      fprintf(stderr,"%s:%d: Serial error #%d,(%xh): %s\n", file, line, (int)GetLastError(),(int)GetLastError(), (char *)lpMsgBuf);
      LocalFree( lpMsgBuf );
     }
  }
}
#else
static void err_trace(char *f, int l)
{
}
#endif
/*----------------------------------------------------------------------------*/
#if defined(_MSC_VER) 
#define snprintf _snprintf
#endif
/*----------------------------------------------------------------------------*/
int serial_open_dev(const char *device, int bits, int baud, char parity, int stopbits)
{
  HANDLE h;
  DCB dcb;
  COMMTIMEOUTS ct;
  char stcom[1024];
  int fd;

  /*Find empty handle*/
  for(fd = 0; fd < MAX_PORT_COUNT && handles[fd]; fd++) {}

  if(fd >= MAX_PORT_COUNT)
    return 0; /*No empty handle*/

  h = CreateFile(device, GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING,0,NULL);

  if(h == INVALID_HANDLE_VALUE)
    return 0;


  if(!GetCommState(h, &dcb))
  {
    if(GetLastError() != ERROR_OPERATION_ABORTED)
    {
      CloseHandle(h);
      return 0;
    }
  }

  sprintf(stcom,
    "BAUD=%d PARITY=%c DATA=%d STOP=%d \
  to=off xon=off odsr=off octs=off dtr=off rts=off idsr=off",
  baud, parity, bits, stopbits);

  if(!BuildCommDCB(stcom,&dcb))
  {
    if(GetLastError() != ERROR_OPERATION_ABORTED)
    {
      CloseHandle(h);
      return 0;
    }
  }


  dcb.fRtsControl=RTS_CONTROL_DISABLE;       /* RTS flow control*/
  dcb.fDtrControl=DTR_CONTROL_DISABLE;       /* DTR flow control type*/


  if(!SetCommState(h, &dcb))
  {
    if(GetLastError() != ERROR_OPERATION_ABORTED)
    {
      CloseHandle(h);
      return 0;
    }
  }


  SetCommMask(h, EV_RXCHAR | EV_TXEMPTY);
  SetupComm(h, 1024, 1024);
  PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);

  GetCommTimeouts(h, &ct);

  ct.ReadTotalTimeoutConstant = 1;
  ct.ReadTotalTimeoutMultiplier = 1;

  ct.WriteTotalTimeoutConstant = 10000;
  ct.WriteTotalTimeoutMultiplier = 1;

  if(!SetCommTimeouts(h, &ct))
  {
    CloseHandle(h);
    return 0;
  }

  ClearCommBreak(h);
  handles[fd] = h;

  return fd + 100;
}
/*----------------------------------------------------------------------------*/
int serial_open(int ncom /*1,2...*/, int bits, int baud, char parity, int stopbits)
{
  char device[256];

  snprintf(device, sizeof(device), "\\\\.\\com%d",ncom);
  return serial_open_dev(device, bits, baud, parity, stopbits);
}
/*----------------------------------------------------------------------------*/
void serial_close(int fd)
{
  if(fd >= 100 && fd < 100 + MAX_PORT_COUNT && handles[fd - 100] != NULL)
  {
    CloseHandle(handles[fd - 100]);
    handles[fd - 100] = NULL;
  }

}
/*----------------------------------------------------------------------------*/
void serial_flush(int fd)
{
  HANDLE h;
  h = get_h(fd);
  if(h)
    PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);
}
/*----------------------------------------------------------------------------*/
int serial_read(int fd, void *buffer, unsigned size, unsigned timeout)
{
  HANDLE h;
  COMMTIMEOUTS ct;
  int received = 0;

  h = get_h(fd);
  if(!h)
    return 0;

  if(!GetCommTimeouts(h,&ct))
  {
    err_trace(__FILE__, __LINE__);
    return 0;
  }

  ct.ReadIntervalTimeout = MAXDWORD;
  ct.ReadTotalTimeoutMultiplier = MAXDWORD;
  ct.ReadTotalTimeoutConstant = timeout;

  if(!SetCommTimeouts(h,&ct))
  {
    err_trace(__FILE__, __LINE__);
    return 0;
  }

  if(!ReadFile(h, buffer, size, (DWORD *)&received, NULL))
  {
    DWORD Err;
    err_trace(__FILE__, __LINE__);
    ClearCommBreak(h);
    ClearCommError(h, &Err, NULL);
    return 0;
  }

#ifdef DEBUG
  if(!received)
  {
//    err_trace(__FILE__, __LINE__);
//    TRACE("%s:%d: Timeout reached. Timeout: %u\n", __FILE__, __LINE__, timeout );
  }
	else
	{
		int i;
		fprintf(stderr, "rx: ");
		for(i = 0; i < received; i++)
			fprintf(stderr, "%02x ", (unsigned)((char *)buffer)[i] & 0xff);
		fprintf(stderr, "\n");
	}
#endif

  return received;
}
/*----------------------------------------------------------------------------*/
int serial_write(int fd, void *buffer, unsigned size)
{
  HANDLE h;
  int send = 0;

  h = get_h(fd);
  if(!h)
    return 0;

  if (!WriteFile(h, buffer, size, (DWORD *)&send, NULL))
  {
    err_trace(__FILE__, __LINE__);
    return 0;
  }

#ifdef DEBUG
	{
		unsigned i;
		fprintf(stderr, "tx: ");
		for(i = 0; i < size; i++)
			fprintf(stderr, "%02x ", (unsigned)((char *)buffer)[i] & 0xff);
		fprintf(stderr, "\n");
	}
#endif

  return send;
}
/*----------------------------------------------------------------------------*/
unsigned serial_waitrx(int fd, unsigned timeout)
{
  return 0;
}
/*----------------------------------------------------------------------------*/
unsigned serial_rxlen(int fd)
{
  HANDLE h;
  DWORD err;
  COMSTAT stat;
  h = get_h(fd);
  if(h)
  {
    ClearCommError(h, &err, &stat);
    return stat.cbInQue;
  }
  return 0;
}
/*----------------------------------------------------------------------------*/
void serial_flush_tx(int fd)
{
  HANDLE h;
  h = get_h(fd);
  if(h)
    PurgeComm(h, PURGE_TXCLEAR);
}
/*----------------------------------------------------------------------------*/
void serial_flush_rx(int fd)
{
  HANDLE h;
  h = get_h(fd);
  if(h)
    PurgeComm(h, PURGE_RXCLEAR);
}
/*---------------------------------------------------------------------------*/
int serial_get_ctrl(int fd, int *_signals)
{
  HANDLE h;
  DWORD status;

  h = get_h(fd);
  if(!h)
    return -1;

  if(!GetCommModemStatus(h, &status))
    return -1;

  *signals = 0;
  if(status & MS_CTS_ON) *_signals |= SER_CTS;
  if(status & MS_DSR_ON) *_signals |= SER_DSR;
  if(status & MS_RING_ON) *_signals |= SER_RNG;
  if(status & MS_RLSD_ON) *_signals |= SER_DCD;
  return 1;
}
/*---------------------------------------------------------------------------*/
int serial_set_ctrl(int fd, int _signals)
{
  HANDLE h;
  //DWORD status;

  h = get_h(fd);
  if(!h)
    return -1;

  if(_signals & SER_DTR)
    EscapeCommFunction(h, SETDTR);
  else
    EscapeCommFunction(h, CLRDTR);

  if(_signals & SER_RTS)
    EscapeCommFunction(h, SETRTS);
  else
    EscapeCommFunction(h, CLRRTS);
  return 1;
}
/*---------------------------------------------------------------------------*/
#endif /*WIN32*/
/*****************************************************************************/
#ifdef LINUX
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int serial_open_dev(const char *device, int bits, int baud, char parity, int stopbits)
{
  int f;
  struct termios newtio;
  tcflag_t csize;
  tcflag_t stopb = 0;
  tcflag_t cparity = 0;
  tcflag_t cbaud = B0;


  f = open(device, O_RDWR | O_NOCTTY /*?| O_NONBLOCK*/);
  if(f <= 0)
  {
    return 0;
  }

  //tcgetattr(hCom,&oldtio); /* save current port settings */

  bzero(&newtio, sizeof(newtio));

  switch(bits)
  {
  case 5:
      csize = CS5;
      break;
  case 6:
      csize = CS6;
      break;
  case 7:
      csize = CS7;
      break;
  default:
      csize = CS8;
      break;
  }

  if(stopbits >= 2)
    stopb = CSTOPB;

  if(parity != 'n' && parity != 'N')
    cparity |= PARENB;

  if(parity == 'o' || parity == 'O')
    cparity |= PARODD;

  switch(baud)
  {
  case 110:
      cbaud = B110;
      break;
  case 134:
      cbaud = B134;
      break;
  case 150:
      cbaud = B150;
      break;
  case 200:
      cbaud = B200;
      break;
  case 300:
      cbaud = B300;
      break;
  case 600:
      cbaud = B600;
      break;
  case 1200:
      cbaud = B1200;
      break;
  case 1800:
      cbaud = B1800;
      break;
  case 2400:
      cbaud = B2400;
      break;
  case 4800:
      cbaud = B4800;
      break;
  case 9600:
      cbaud = B9600;
      break;
  case 19200:
      cbaud = B19200;
      break;
  case 38400:
      cbaud = B38400;
      break;
  case 57600:
      cbaud = B57600;
      break;
  case 115200:
      cbaud = B115200;
      break;
/*
  case 230400:
      cbaud = B230400;
      break;
*/
  }

  cfsetospeed(&newtio, cbaud);
  cfsetispeed(&newtio, cbaud);

  newtio.c_cflag = cbaud | csize | stopb | cparity | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer*/
  newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */

  tcflush(f, TCIFLUSH);
  tcsetattr(f, TCSANOW, &newtio);
  return f;

}
/*----------------------------------------------------------------------------*/
int serial_open(int ncom /*1,2...*/, int bits, int baud, char parity, int stopbits)
{
  char device[256];
  snprintf(device,sizeof(device), "/dev/ttyS%d", ncom - 1);
  return serial_open_dev(device, bits, baud, parity, stopbits);
}
/*----------------------------------------------------------------------------*/
void serial_close(int fd)
{
  close(fd);
}
/*----------------------------------------------------------------------------*/
void serial_flush(int fd)
{
  tcflush(fd, TCIOFLUSH);
}
/*----------------------------------------------------------------------------*/
int serial_read(int fd, void *buffer, unsigned size, unsigned timeout)
{
  int received = 0, s;
  struct timeval tv;
  fd_set rfds;


  /* Wait up timeout mseconds. */
  if(timeout > 2147)
  {
    s = timeout / 1000;
    timeout %= 1000;
  }
  else
    s = 0;


  tv.tv_sec = s;
  tv.tv_usec = timeout * 1000;

#if 0
  fprintf(stderr, "tv_sec=%d tv_usec=%d\n", tv.tv_sec, tv.tv_usec);
#endif

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  if(!timeout || select(fd + 1, &rfds, NULL, NULL, &tv) > 0)
  {
    received = read(fd, buffer, size);
//#if 0
    if(received < 0)
    {
      perror("Serial :: read error");
    }
//#endif
  }


#if 0
  fprintf(stderr, "received [");
  for(s = 0; s < received; s++)
    fprintf(stderr, "%02X ", ((unsigned char *)buffer)[s] & 0xff);
  fprintf(stderr, "]\n");
#endif
  return received;
}
/*----------------------------------------------------------------------------*/
int serial_write(int fd, void *buffer, unsigned size)
{
  return write(fd, buffer, size);
}
/*----------------------------------------------------------------------------*/
unsigned serial_waitrx(int fd, unsigned timeout)
{
  fd_set rfds;
  struct timeval tv;
  int s;

  /* Watch serial to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  /* Wait up timeout mseconds. */
  if(timeout > 2147)
  {
    s = timeout / 1000;
    timeout %= 1000;
  }
  else
    s = 0;


  tv.tv_sec = s;
  tv.tv_usec = timeout * 1000;


  if(select(fd + 1, &rfds, NULL, NULL, &tv) > 0)
  {
    struct stat st;

    if(fstat(fd, &st) == -1 )
      return 0;

    return st.st_size;
  }

  return 0;
}
/*----------------------------------------------------------------------------*/
unsigned serial_rxlen(int fd)
{
  struct stat st;

  if(fstat(fd, &st) == -1 ) {
//    perror("Serial :: RxLen error");
    return 0;
  }

  return st.st_size;

}
/*----------------------------------------------------------------------------*/
void serial_flush_tx(int fd)
{
  tcflush(fd, TCOFLUSH);
}
/*----------------------------------------------------------------------------*/
void serial_flush_rx(int fd)
{
  tcflush(fd, TCIFLUSH);
}
/*----------------------------------------------------------------------------*/
int serial_get_ctrl(int fd, int *_signals)
{
  int status;
  if(ioctl(fd, TIOCMGET, &status) < 0 || _signals == NULL)
    return -1;
  *_signals = 0;
  if(status & TIOCM_DTR) *_signals |= SER_DSR;
  if(status & TIOCM_RTS) *_signals |= SER_RTS;
  if(status & TIOCM_CTS) *_signals |= SER_CTS;
  if(status & TIOCM_CD)  *_signals |= SER_DCD;
  if(status & TIOCM_RNG) *_signals |= SER_RNG;
#if 0
  fprintf(stderr, "Signals: ");
  if(status & TIOCM_DTR) fprintf(stderr, "DTR ");
  if(status & TIOCM_RTS) fprintf(stderr, "RTS ");
  if(status & TIOCM_CTS) fprintf(stderr, "CTS ");
  if(status & TIOCM_CD)  fprintf(stderr, "CD ");
  if(status & TIOCM_RNG) fprintf(stderr, "RNG ");
  fprintf(stderr, "\n");
#endif
  return 1;
}
/*----------------------------------------------------------------------------*/
int serial_set_ctrl(int fd, int _signals)
{
  int status = 0;

  if(_signals & SER_DSR)
    status |= TIOCM_DTR;
  if(_signals & SER_RTS)
    status |= TIOCM_RTS;
  if(_signals & SER_CTS)
    status |= TIOCM_CTS;
  if(_signals & SER_DCD)
    status |= TIOCM_CD;
  if(_signals & SER_RNG)
    status |= TIOCM_RNG;

  return ioctl(fd, TIOCMSET, &status);
}
/*----------------------------------------------------------------------------*/
#endif /*UNIX*/

