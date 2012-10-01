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
  QThread::start();
}
/*----------------------------------------------------------------------------*/
DDAController :: ~DDAController()
{
  m_terminated = true;
  wait(1000);
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
  if(m_serial > 0)
    close();
  setStatus(Offline);
  int s = serial_open(serial, 8, baud, 'n', 2);
  if(s <= 0)
    return false;
  lock();
  m_serial = s;
  unlock();
  return true;
}
/*----------------------------------------------------------------------------*/
void DDAController :: close()
{
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
bool DDAController :: rxPacket(int serial)
{
  if(m_lastCharTime.elapsed() >= RxTimeout)
  {
    m_rxData.clear();
  }

  bool ok = false;
  unsigned char rx;
  while(serial_read(serial, &rx, 1, 10) > 0 && !m_terminated)
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
          m_rxData.clear();
      }
    }
    else
    {
      unsigned sz = m_rxData[3];
      if((unsigned)m_rxData.size() == sz) //CRC received
      {
        unsigned char crc = 0;
        for(unsigned i = 1; i < sz; i++)
          crc += m_rxData[i];
        if(crc == rx) //CRC Ok !
        {
          int txSize;
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
  lock();
  ByteVector txData = m_txData;
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
        lock();
        m_txData.clear();
        unlock();
        emit cmdSendOk();
        return true;
      }
      break;
    }
  }
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
    emit serialReceived(s);
    setStatus(Idle);
    break;
  case 'A':
    if(s[0] == '0' && s.toInt() == 0)
    {
      setStatus(Calibrating);
    }
    else
    {
      setStatus(Measuring);
      emit currentStretch(s.simplified().toDouble());
    }
    break;
  case 'M':
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
        double strength = lst[0].simplified().toDouble();
        int number = lst[1].simplified().toInt();
        int nextCell = lst[2].simplified().toInt();
        setStatus(Measuring);
        emit measure(strength, number, nextCell);
      }
    }
    break;
  case 'G':
    emit giftSize(s.simplified().toDouble());
    break;
  case 'C':
    setStatus(NextCasseteWaiting);
    emit nextCasseteRequest();
    break;
  case 'E':
    setStatus(EndOfMeasuring);
    emit endOfMeasuring();
    break;
  }
}
/*----------------------------------------------------------------------------*/
void DDAController :: setMode(int meshIndex, int samples, DDAMode mode)
{
  lock();
  m_txData.clear();
  m_txData.push_back(ENQ);
  m_txData.push_back('I');
  m_txData.push_back(7);
  m_txData.push_back((unsigned char)meshIndex);
  m_txData.push_back((unsigned char)(meshIndex >> 8));
  m_txData.push_back((unsigned char)samples);
  m_txData.push_back((unsigned char)((mode << 1) | 1));
  unlock();
}
/*----------------------------------------------------------------------------*/
void DDAController :: start()
{
  lock();
  m_txData.clear();
  m_txData.push_back(ENQ);
  m_txData.push_back('P');
  m_txData.push_back(3);
  unlock();
}
/*----------------------------------------------------------------------------*/
void DDAController :: manualMode()
{
  lock();
  m_txData.clear();
  m_txData.push_back(ENQ);
  m_txData.push_back('S');
  m_txData.push_back(3);
  unlock();
}
/*----------------------------------------------------------------------------*/

