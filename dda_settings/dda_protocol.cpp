/*----------------------------------------------------------------------------*/
/**
* @pkg dda_protocol
*/
/**
* DDA serial protocol.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 26.01.2013 15:42:02<br>
* @pkgdoc dda_protocol
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <pc_serial.h>

#ifdef LINUX
#include <sys/time.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

/*----------------------------------------------------------------------------*/
const unsigned RxTimeout = 1000;
const unsigned AckTimeout = 1000;
const unsigned char ENQ = 5;	  //Запрос обмена источником – это начало каждого пакета
const unsigned char ACK = 6;	  //Ответ приемника о распознавании пакета
const unsigned char NAK = 0x15;	//Ответ приемника о нераспознавании пакета
/*----------------------------------------------------------------------------*/
static unsigned sys_tick_count(void)
{
#ifdef WIN32
  return (unsigned)GetTickCount();
#endif
#ifdef LINUX
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;

#endif
}
/*----------------------------------------------------------------------------*/
DDAProtocol :: DDAProtocol()
{
  m_terminated = false;
  m_serial = -1;
  m_log = new logfile("serial.log", false);
  m_log->Info("Controller created");
}
/*----------------------------------------------------------------------------*/
DDAProtocol :: ~DDAProtocol()
{
  m_terminated = true;
  close();
  m_log->Info("Controlled destroyed");
}
/*----------------------------------------------------------------------------*/
bool DDAProtocol :: open(int serial, int baud)
{
  m_log->Info("Try to open: serial:%d, baud:%d", serial, baud);
  if(m_serial > 0)
    close();
  m_serial = serial_open(serial, 8, baud, 'n', 2);
  if(m_serial <= 0)
  {
    m_log->Exception("Error open port");
    return false;
  }
  m_log->Info("Port opened Ok");
  return true;
}
/*----------------------------------------------------------------------------*/
bool DDAProtocol :: open(const char *device, int baud)
{
  if(*device >= '1' && *device <= '9')
    return open(atoi(device), baud);
  m_serial = serial_open_dev(device, 8, baud, 'e', 1);
  if(m_serial <= 0)
  {
    m_log->Exception("Error open port");
    return false;
  }
  m_log->Info("Port opened Ok");
  return true;
}
/*----------------------------------------------------------------------------*/
void DDAProtocol :: close()
{
  m_log->Info("Port close");
  if(m_serial > 0)
  {
    serial_close(m_serial);
    m_serial = -1;
  }
}
/*----------------------------------------------------------------------------*/
#define ERROR_COUNT 3
bool DDAProtocol :: exec()
{
  int errorCount = ERROR_COUNT;
  while(!m_terminated)
  {
    if(rxPacket())
    {
      errorCount = ERROR_COUNT;
      unsigned txSize = m_txData.size();
      if(!txSize)
      {
        //Send ACK
        unsigned char tx = ACK;
        serial_write(m_serial, &tx, 1);
      }
      else
      {
        txPacket();
        waitAck();
      }
      handleData(m_rxData);
      m_rxData.clear();
    }
    else
    {
      if(--errorCount <= 0)
        return false;
    }
  }
  return true;
}
/*----------------------------------------------------------------------------*/
int DDAProtocol :: serial_read(int fd, void *buffer, unsigned size, unsigned timeout)
{
  int count = ::serial_read(fd, buffer, size, timeout);
  if(count)
  {
    char *buf = new char[count * 8];
    int sz = 0;
    for(int i = 0; i < count; i++)
    {
      char c = ((char *)buffer)[i];
      sz += sprintf(buf + sz, "%02X '%c' ", (unsigned)c & 0xff, (c >= 0x20) ? c : '.');
    }

    m_log->Info("RX %s", buf);
    delete [] buf;
  }
  return count;
}
/*----------------------------------------------------------------------------*/
int DDAProtocol :: serial_write(int fd, void *buffer, unsigned size)
{
  int count = ::serial_write(fd, buffer, size);
  if(count)
  {
    char *buf = new char[count * 8];
    int sz = 0;
    for(int i = 0; i < count; i++)
    {
      char c = ((char *)buffer)[i];
      sz += sprintf(buf + sz, "%02X '%c' ", (unsigned)c & 0xff, (c >= 0x20) ? c : '.');
    }
    m_log->Info("TX %s", buf);
    delete [] buf;
  }
  return count;
}
/*----------------------------------------------------------------------------*/
bool DDAProtocol :: rxPacket()
{
  bool ok = false;
  unsigned char rx;
  while(!ok &&  !m_terminated)
  {
    if(serial_read(m_serial, &rx, 1, 10) > 0)
    {
      m_lastCharTime = sys_tick_count();
      if(m_rxData.empty())
      {
        if(rx == ENQ)
          m_rxData.push_back(rx);
      }
      else
      if(m_rxData.size() < 3)
      {
        m_rxData.push_back(rx);
        if(m_rxData.size() == 3) //Pack length received
        {
          if(rx < 3) //Invalid packet length
          {
            m_log->Exception("Invalid packet length (%d)", rx);
            m_rxData.clear();
          }
        }
      }
      else
      {
        unsigned sz = m_rxData[2];
        if((unsigned)m_rxData.size() == sz) //CRC received
        {
          unsigned char crc = 0;
          for(unsigned i = 1; i < sz; i++)
            crc += m_rxData[i];
          if(crc == rx) //CRC Ok !
          {
            m_log->Info("Crc Ok");
            ok = true;
            break;
          }
          else
          {
            //Send NAK
            unsigned char tx = NAK;
            m_log->Message("Crc Error");
            serial_write(m_serial, &tx, 1);
            m_rxData.clear();
          }
        }
        else
          m_rxData.push_back(rx);
      }
    } //Rx char

    if(sys_tick_count() - m_lastCharTime >= RxTimeout)
    {
      m_log->Exception("Rx timeout");
      m_rxData.clear();
      m_lastCharTime = sys_tick_count();
      break;
    }
  }
  return ok;
}
/*----------------------------------------------------------------------------*/
bool DDAProtocol :: txPacket()
{
  ByteVector txData;
  if(m_txData.size())
    txData = m_txData[0];
  if(txData.empty())
    return false;

  unsigned size = txData.size();
  serial_write(m_serial, &txData.at(0), size);
  return true;
}
/*----------------------------------------------------------------------------*/
bool DDAProtocol :: waitAck()
{
  unsigned t;
  t = sys_tick_count();
  while(!m_terminated && sys_tick_count() - t < AckTimeout)
  {
    unsigned char rx;
    if(serial_read(m_serial, &rx, 1, 10) > 0)
    {
      if(rx == ACK)
      {
        m_log->Info("ACK received");
        if(m_txData.size())
          m_txData.erase(m_txData.begin());
        return true;
      }
      break;
    }
  }
  m_log->Message("Wait ACK error");
  return false;
}
/*----------------------------------------------------------------------------*/
void DDAProtocol :: pushData(unsigned char header, const DDAProtocol :: ByteVector &data)
{
  ByteVector txData;
  unsigned size = data.size();
  unsigned char crc = 0;
  txData.push_back(ENQ);
  txData.push_back(header);
  crc += header;
  txData.push_back(size + 3);
  crc += size + 3;
  for(unsigned i = 0; i < size; i++)
  {
    txData.push_back(data[i]);
    crc += data[i];
  }
  txData.push_back(crc);
  m_txData.push_back(txData);
}
/*----------------------------------------------------------------------------*/

