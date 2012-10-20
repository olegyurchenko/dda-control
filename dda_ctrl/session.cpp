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
#define SESSION_CHANGED 1
#define MEASURES_CHANGED 2
/*----------------------------------------------------------------------------*/
DDAMeasureSession :: DDAMeasureSession(QObject *parent)
  :QObject(parent)
{
  m_updateState = false;
  m_updateMask = 0;
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setSessionChanged()
{
  if(m_updateState)
    m_updateMask |= SESSION_CHANGED;
  else
    emit sessionChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setMeasureListChanged()
{
  if(m_updateState)
    m_updateMask |= MEASURES_CHANGED;
  else
    emit measureListChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: beginUpdate()
{
  if(!m_updateState)
  {
    m_updateMask = 0;
    m_updateState = true;
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: endUpate()
{
  if(m_updateState)
  {
    if(m_updateMask & SESSION_CHANGED)
      emit sessionChanged();
    if(m_updateMask & MEASURES_CHANGED)
      emit measureListChanged();
    m_updateMask = 0;
    m_updateState = false;
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setSerial(const QString& serial)
{
  if(m_session.deviceSerial != serial)
  {
    m_session.deviceSerial = serial;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: addMeasure(double strength, double size, int)
{
  DDAMeasure m;
  m.strenght = strength;
  m.size = size;
  m_measureList.append(m);
  setMeasureListChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: onNoParticle()
{
  DDAMeasure m;
  m.strenght = 0;
  m.size = 0;
  m.ignored = true;
  m_measureList.append(m);
  setMeasureListChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setSession(const DDASession& s)
{
  m_session = s;
  setSessionChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: addMeasure(const DDAMeasure& m)
{
  m_measureList.append(m);
  setMeasureListChanged();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setId(int i)
{
  if(m_session.id != i)
  {
    m_session.id = i;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setUserId(int i)
{
  if(m_session.userId != i)
  {
    m_session.userId = i;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setStart(const QDateTime& d)
{
  if(m_session.start != d)
  {
    m_session.start = d;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setEnd(const QDateTime& d)
{
  if(m_session.end != d)
  {
    m_session.end = d;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setLlot(const QString& l)
{
  if(m_session.lot != l)
  {
    m_session.lot = l;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setStandard(int i)
{
  if(m_session.standard != i)
  {
    m_session.standard = i;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setGritIndex(int i)
{
  if(m_session.gritIndex != i)
  {
    m_session.gritIndex = i;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setMark(const QString& m)
{
  if(m_session.mark != m)
  {
    m_session.mark = m;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: setParticles(int i)
{
  if(m_session.particles != i)
  {
    m_session.particles = i;
    setSessionChanged();
  }
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: onEndOfMeasuring()
{
  m_session.end = QDateTime::currentDateTime();
}
/*----------------------------------------------------------------------------*/
void DDAMeasureSession :: clear()
{
  m_session.id = InvalidId;
  m_session.start = QDateTime::currentDateTime();
  m_session.end = QDateTime::currentDateTime();
  m_session.lot = "";
  m_session.mark = "";
  m_measureList.clear();
  setSessionChanged();
  setMeasureListChanged();
}
/*----------------------------------------------------------------------------*/

