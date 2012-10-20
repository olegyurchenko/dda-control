/*----------------------------------------------------------------------------*/
/**
* @pkg controller
*/
/**
* DDA controller.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 01.10.2012  9:47:44<br>
* @pkgdoc controller
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "controller.h"
#include <QTime>
#include <QCoreApplication>
#include <pc_serial.h>
#include <QByteArray>
#include <QStringList>
#include <logfile.h>
#include <stdio.h>
/*----------------------------------------------------------------------------*/
DDAController *controller = NULL;
/*----------------------------------------------------------------------------*/
const int RxTimeout = 1000;
const int AckTimeout = 1000;
const unsigned char ENQ = 5;	  //Запрос обмена источником – это начало каждого пакета
const unsigned char ACK = 6;	  //Ответ приемника о распознавании пакета
const unsigned char NAK = 0x15;	//Ответ приемника о нераспознавании пакета
/*----------------------------------------------------------------------------*/
DDAController :: DDAController(QObject *parent)
  : QThread(parent)
{
  m_terminated = false;
  m_serial = -1;
  m_status = Offline;
  m_lastCharTime.start();
  m_log = new logfile("controller.log", false);
  m_log->Info("Controller created");
}
/*----------------------------------------------------------------------------*/
DDAController :: ~DDAController()
{
  m_terminated = true;
  wait(1000);
  m_log->Info("Controlled destroyed");
}
/*----------------------------------------------------------------------------*/
void DDAController :: setStatus(DDAStatus s)
{
  if(m_status != s)
  {
    m_status = s;
    emit statusChanged((int)s);
  }
}
/*----------------------------------------------------------------------------*/
bool DDAController :: open(int serial, int baud)
{
  m_log->Info("Try to open: serial:%d, baud:%d", serial, baud);
  if(m_serial > 0)
    close();
  setStatus(Offline);
  int s = serial_open(serial, 8, baud, 'n', 2);
  if(s <= 0)
  {
    m_log->Exception("Error open port");
    return false;
  }
  m_log->Info("Port opened Ok");
  lock();
  m_serial = s;
  unlock();
  return true;
}
/*----------------------------------------------------------------------------*/
void DDAController :: close()
{
  m_log->Info("Port close");
  if(m_serial > 0)
  {
    int s = m_serial;
    lock();
    m_serial = -1;
    unlock();
    serial_close(s);
  }
  setStatus(Offline);
}
/*----------------------------------------------------------------------------*/
QString DDAController :: statusString(int s)
{
  QString str = "???";
  switch(s)
  {
  case Offline:
    str = tr("offline");
    break;
  case Idle:
    str = tr("idle");
    break;
  case Calibrating:
    str = tr("calibrating");
    break;
  case Measuring:
    str = tr("measuring");
    break;
  case NoParticle:
    str = tr("no particle");
    break;
  case NextCasseteWaiting:
    str = tr("next cassete waiting");
    break;
  case EndOfMeasuring:
    str = tr("end of measuring");
    break;
  }
  return str;
}
/*----------------------------------------------------------------------------*/
void DDAController :: run()
{
  while(!m_terminated)
  {
    QTime t;
    t.start();
    QCoreApplication::processEvents();

    int serial;
    lock();
    serial = m_serial;
    unlock();
    if(serial > 0)
    {
      if(rxPacket(serial) && txPacket(serial))
        waitAck(serial);
    }

    if(!t.elapsed())
      msleep(10);
  }
}
/*----------------------------------------------------------------------------*/
int DDAController :: serial_read(int fd, void *buffer, unsigned size, unsigned timeout)
{
  int count = ::serial_read(fd, buffer, size, timeout);
  if(count)
  {
    char *buf = new char[count * 8];
    int sz = 0;
    for(int i = 0; i < count; i++)
      sz += sprintf(buf + sz, "%02X ", (unsigned)((char *)buffer)[i] & 0xff);
    m_log->Info("RX %s", buf);
    delete [] buf;
  }
  return count;
}
/*----------------------------------------------------------------------------*/
int DDAController :: serial_write(int fd, void *buffer, unsigned size)
{
  int count = ::serial_write(fd, buffer, size);
  if(count)
  {
    char *buf = new char[count * 8];
    int sz = 0;
    for(int i = 0; i < count; i++)
      sz += sprintf(buf + sz, "%02X ", (unsigned)((char *)buffer)[i] & 0xff);
    m_log->Info("TX %s", buf);
    delete [] buf;
  }
  return count;
}
/*----------------------------------------------------------------------------*/
bool DDAController :: rxPacket(int serial)
{
  if(m_lastCharTime.elapsed() >= RxTimeout)
  {
    m_rxData.clear();
  }

  bool ok = false;
  unsigned char rx;
  while(!ok && serial_read(serial, &rx, 1, 10) > 0 && !m_terminated)
  {
    m_lastCharTime.restart();
    if(m_rxData.isEmpty())
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
        if(rx < 4) //Invalid packet length
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
          int txSize;
          m_log->Info("Crc Ok");
          lock();
          txSize = m_txData.size();
          unlock();
          if(!txSize)
          {
            //Send ACK
            unsigned char tx = ACK;
            serial_write(serial, &tx, 1);
          }
          else
            ok = true;
          handleData(m_rxData);
          m_rxData.clear();
        }
        else
        {
          //Send NAK
          unsigned char tx = NAK;
          m_log->Message("Crc Error");
          serial_write(serial, &tx, 1);
          m_rxData.clear();
        }
      }
      else
        m_rxData.push_back(rx);

    }
  }
  return ok;
}
/*----------------------------------------------------------------------------*/
bool DDAController :: txPacket(int serial)
{
  ByteVector txData;
  lock();
  if(m_txData.size())
    txData = m_txData[0];
  unlock();
  if(txData.isEmpty())
    return false;

  int size = txData.size();
  unsigned char crc = 0;
  for(int i = 1; i < size; i++)
    crc += txData[i];
  txData.push_back(crc);
  serial_write(serial, (void *)txData.begin(), size + 1);
  return true;
}
/*----------------------------------------------------------------------------*/
bool DDAController :: waitAck(int serial)
{
  QTime t;
  t.start();
  while(!m_terminated && t.elapsed() < AckTimeout)
  {
    QCoreApplication::processEvents();
    unsigned char rx;
    if(serial_read(serial, &rx, 1, 10) > 0)
    {
      if(rx == ACK)
      {
        m_log->Info("ACK received");
        lock();
        if(m_txData.size())
          m_txData.erase(m_txData.begin());
        unlock();
        emit cmdSendOk();
        return true;
      }
      break;
    }
  }
  m_log->Message("Wait ACK error");
  emit cmdSendError();
  return false;
}
/*----------------------------------------------------------------------------*/
void DDAController :: handleData(const ByteVector& data)
{
  int size = data.size();
  if(size < 3)
    return;
  QString s = QByteArray((const char *)&data[3], size - 3);
  switch(data[1]) //Header
  {
  case 'N':
    m_log->Info("Serial received '%s'", s.toAscii().constData());
    setDeviceSerial(s);
    emit serialReceived(s);
    setStatus(Idle);
    break;
  case 'A':
    m_log->Info("Strength receved '%s'", s.toAscii().constData());
    if(s[0] == '0' && s.toInt() == 0)
    {
      setStatus(Calibrating);
    }
    else
    {
      setStatus(Measuring);
      setStrength(s.simplified().toDouble());
      emit currentStretch(strength());
    }
    m_size = 0;
    break;
  case 'M':
    m_log->Info("Measure received '%s'", s.toAscii().constData());
    if(s[0] == '-')
    {
      setStatus(NoParticle);
      emit noParticle();
    }
    else
    {
      QStringList lst = s.split(",");
      if(lst.size() >= 3)
      {
        setStrength(lst[0].simplified().toDouble());
        int number = lst[1].simplified().toInt();
        //int nextCell = lst[2].simplified().toInt();
        setStatus(Measuring);
        emit measure(strength(), DDAController::size(), number);
      }
    }
    break;
  case 'G':
    m_log->Info("Size received '%s'", s.toAscii().constData());
    setSize(s.simplified().toDouble());
    emit sizeReceived(s.simplified().toDouble());
    break;
  case 'C':
    m_log->Info("NextCasseteWaiting received");
    setStatus(NextCasseteWaiting);
    emit nextCasseteRequest();
    break;
  case 'E':
    m_log->Info("EndOfMeasuring received");
    setStatus(EndOfMeasuring);
    emit endOfMeasuring();
    break;
  }
}
/*----------------------------------------------------------------------------*/
void DDAController :: setMode(int meshIndex, int samples, DDAMode mode)
{
  m_log->Info("setMode: meshIndex:%d samples:%d mode:%d", meshIndex, samples, mode);
  ByteVector txData;
  txData.push_back(ENQ);
  txData.push_back('I');
  txData.push_back(7);
  txData.push_back((unsigned char)meshIndex);
  txData.push_back((unsigned char)(meshIndex >> 8));
  txData.push_back((unsigned char)samples);
  txData.push_back((unsigned char)((mode << 1) | 1));
  lock();
  m_txData.append(txData);
  unlock();
}
/*----------------------------------------------------------------------------*/
void DDAController :: resume()
{
  m_log->Info("resume()");
  ByteVector txData;
  txData.clear();
  txData.push_back(ENQ);
  txData.push_back('P');
  txData.push_back(3);
  lock();
  m_txData.append(txData);
  unlock();
}
/*----------------------------------------------------------------------------*/
void DDAController :: manualMode()
{
  m_log->Info("manualMode()");
  ByteVector txData;
  txData.clear();
  txData.push_back(ENQ);
  txData.push_back('S');
  txData.push_back(3);
  lock();
  m_txData.append(txData);
  unlock();
}
/*----------------------------------------------------------------------------*/
// DevoController
/*----------------------------------------------------------------------------*/
DemoController :: DemoController(QObject *parent)
  : DDAController(parent)
{
  m_count = 5;
  m_time.start();
  m_cmd = NoCmd;
}
/*----------------------------------------------------------------------------*/
void DemoController :: run()
{
  qsrand(QDateTime::currentDateTime().toTime_t());
  while(!m_terminated)
  {
    QCoreApplication::processEvents();
    if(m_time.elapsed() >= 300/*1000*/)
    {
      m_time.restart();
      m_count --;
      if(m_count <= 0)
      {
        m_count = 1;
        doSimulation();
      }
    }
    msleep(10);
  }
}
/*----------------------------------------------------------------------------*/
void DemoController :: setMode(int, int samples, DDAMode mode)
{
  m_mode = mode;
  m_particles = samples;
}
/*----------------------------------------------------------------------------*/
void DemoController :: resume()
{
  m_cmd = StartCmd;
}
/*----------------------------------------------------------------------------*/
void DemoController :: manualMode()
{
  m_mode = Manual;
}
/*----------------------------------------------------------------------------*/
void DemoController :: doSimulation()
{
  switch(m_status)
  {
  case Offline:
    setStatus(Idle);
    break;
  case Idle:
    if(m_cmd == StartCmd)
    {
      m_cmd = NoCmd;
      emit cmdSendOk();
      setStatus(Calibrating);
      m_count = 5;
      m_strength = 0;
      m_number = 1;
      m_nextCell = 1;
    }
    else
    {
      setDeviceSerial("00013");
      emit serialReceived(deviceSerial());
    }
    break;
  case Calibrating:
    setStatus(Measuring);
    break;
  case Measuring:
    if(m_strength < 1)
    {
      if(!(qrand() % 5))
      {
        setStatus(NoParticle);
        emit noParticle();
        m_strength = 0;
        //m_number ++;
        m_nextCell ++;
        break;
      }

      setSize(234.5 + 345.6 * (double)(qrand() % 20)/10.);
      emit sizeReceived(size());
    }

    switch((qrand() % 4))
    {
    case 0:
      m_strength += 12.3;
      break;
    case 1:
      m_strength += 6.5;
      break;
    case 2:
      m_strength += 3.1;
      break;
    case 3:
      m_strength += 1.8;
      break;
    }
    setStrength(m_strength);
    emit currentStretch(strength());
    if(m_strength > 50)
    {
      if(!(qrand() % 4)) //end of measure
      {
        setStrength(m_strength);
        emit measure(strength(), size(), m_number);
        m_strength = 0;

        m_number ++;
        m_nextCell ++;
        /*
        if(!(m_number % 50))
        {
          m_count = 5;
          setStatus(NextCasseteWaiting);
          emit nextCasseteRequest();
        }
        */

        if(m_number >= m_particles)
        {
          m_count = 5;
          setStatus(EndOfMeasuring);
          emit endOfMeasuring();
        }
      }
    }
    break;
  case NoParticle:
    setStatus(Measuring);
    break;
  case NextCasseteWaiting:
    setStatus(Measuring);
    break;
  case EndOfMeasuring:
    setStatus(Idle);
    break;
  }
}
/*----------------------------------------------------------------------------*/

