/*----------------------------------------------------------------------------*/
/**
* @pkg session
*/
/**
* Measure session.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 03.10.2012  8:46:33<br>
* @pkgdoc session
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "session.h"
/*----------------------------------------------------------------------------*/
DDAMeasureSession *session = NULL;
/*----------------------------------------------------------------------------*/
DDAMeasureSession :: DDAMeasureSession(QObject *parent)
  :QObject(parent)
{
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setSerial(const QString& serial)
{
  if(m_session.deviceSerial != serial)
  {
    m_session.deviceSerial = serial;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: addMeasure(double strength, double size, int)
{
  DDAMeasure m;
  m.strenght = strength;
  m.size = size;
  m_measureList.append(m);
  emit measureListChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setSession(const DDASession& s)
{
  m_session = s;
  emit sessionChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: addMeasure(const DDAMeasure& m)
{
  m_measureList.append(m);
  emit measureListChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setId(int i)
{
  if(m_session.id != i)
  {
    m_session.id = i;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setUserId(int i)
{
  if(m_session.userId != i)
  {
    m_session.userId = i;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setStart(const QDateTime& d)
{
  if(m_session.start != d)
  {
    m_session.start = d;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setEnd(const QDateTime& d)
{
  if(m_session.end != d)
  {
    m_session.end = d;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setLlot(const QString& l)
{
  if(m_session.lot != l)
  {
    m_session.lot = l;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setMeshIndex(int i)
{
  if(m_session.meshIndex != i)
  {
    m_session.meshIndex = i;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setGostIndex(int i)
{
  if(m_session.gostIndex != i)
  {
    m_session.gostIndex = i;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setMark(const QString& m)
{
  if(m_session.mark != m)
  {
    m_session.mark = m;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setGiftCount(int i)
{
  if(m_session.giftCount != i)
  {
    m_session.giftCount = i;
    emit sessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: onEndOfMeasuring()
{
  m_session.end = QDateTime::currentDateTime();
}
/*----------------------------------------------------------------------------*/

