/*----------------------------------------------------------------------------*/
/**
* @pkg boot_protocol
*/
/**
* Short description of boot_protocol.
*
* Long description of boot_protocol.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 13.11.2012 11:49:52<br>
* @pkgdoc boot_protocol
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "boot_protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <pc_serial.h>
/*----------------------------------------------------------------------------*/
#ifdef UNIX
#include <unistd.h>
static void Sleep(unsigned ms)
{
    usleep(ms * 1000);
}
#endif
/*----------------------------------------------------------------------------*/
const unsigned char ACK = 0x79;
const unsigned char NACK = 0x1F;
/*----------------------------------------------------------------------------*/
BootLoaderProtocol :: BootLoaderProtocol()
{
  m_serial = -1;
  m_version = 0;
  m_verbose = false;
}
/*----------------------------------------------------------------------------*/
BootLoaderProtocol :: ~BootLoaderProtocol()
{
  if(m_serial > 0)
    serial_close(m_serial);
}
/*----------------------------------------------------------------------------*/
bool BootLoaderProtocol :: open(int serial, int baud)
{
  m_serial = serial_open(serial, 8, baud, 'e', 1);
  if(m_serial <= 0)
  {
    m_errorString = "Error oprn serial";
    return false;
  }
  unsigned char c = 0x7f;
  serial_write(m_serial, &c, 1);
  waitAck();
  return true;
}
/*----------------------------------------------------------------------------*/
bool BootLoaderProtocol :: open(const char *device, int baud)
{
  if(*device >= '1' && *device <= '9')
    return open(atoi(device), baud);
  m_serial = serial_open_dev(device, 8, baud, 'e', 1);
  if(m_serial <= 0)
  {
    m_errorString = "Error oprn serial";
    return false;
  }
  unsigned char c = 0x7f;
  serial_write(m_serial, &c, 1);
  waitAck();
  //Sleep(100);
  return true;
}
/*----------------------------------------------------------------------------*/
bool BootLoaderProtocol :: sendComplement(unsigned char b)
{
  unsigned char buf[2];
  buf[0] = b;
  buf[1] = ~b;
  if(serial_write(m_serial, buf, 2) != 2)
  {
    m_errorString = "Error write to serial device";
    return false;
  }
  return true;
}
/*----------------------------------------------------------------------------*/
bool BootLoaderProtocol :: waitAck()
{
  unsigned char b;
  if(serial_read(m_serial, &b, 1, 1000) <= 0)
  {
    m_errorString ="ACK timeout";
    return false;
  }
  if(b == NACK)
  {
    m_errorString = "NAK received";
    return false;
  }
  if(b != ACK)
  {
    char buf[80];
    snprintf(buf, sizeof(buf), "Required ACK. Received %02xh", (unsigned) b & 0xff);
    m_errorString = buf;
    return false;
  }
  return true;
}
/*----------------------------------------------------------------------------*/
bool BootLoaderProtocol :: getVersionAndAllowedCmd()
{
  serial_flush_rx(m_serial);
  if(!sendComplement(0) || !waitAck())
    return false;
  unsigned char b, size;
  if(serial_read(m_serial, &size, 1, 1000) <= 0)
  {
    m_errorString = "Rx size timeout";
    return false;
  }

  //if(verbose())
  //  printf("Size %02xh\n", (unsigned) size & 0xff);

  if(serial_read(m_serial, &m_version, 1, 1000) <= 0)
  {
    m_errorString = "version timeout";
    return false;
  }
  if(verbose())
    printf("Bootloader version %02xh\n", (unsigned) m_version & 0xff);

  m_allowedCmd.clear();
  while(size)
  {
    if(serial_read(m_serial, &b, 1, 1000) <= 0)
    {
      m_errorString = "data timeout";
      return false;
    }
    if(verbose())
      printf("Allowed command %02xh\n", (unsigned) b & 0xff);
    m_allowedCmd.push_back(b);
    size --;
  }
  if(!waitAck())
    return false;
  return true;
}
/*----------------------------------------------------------------------------*/

