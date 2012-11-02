/*----------------------------------------------------------------------------*/
/**
* @pkg plotter
*/
/**
* Short description of plotter.
*
* Long description of plotter.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 17.09.2012 11:03:25<br>
* @pkgdoc plotter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "plotter.h"
#include <QVariantMap>
/*----------------------------------------------------------------------------*/
Plotter :: Plotter(QObject *parent)
  : QObject(parent)
{
}
/*----------------------------------------------------------------------------*/
QVariantMap Plotter :: vPen() const
{
  QVariantMap m;
  m["style"] = m_pen.style();
  m["width"] = m_pen.width();
  m["color"] = m_pen.color();
  return m;
}
/*----------------------------------------------------------------------------*/
QVariantMap Plotter :: vBrush() const
{
  QVariantMap m;
  m["style"] = m_brush.style();
  m["color"] = m_brush.color();
  return m;
}
/*----------------------------------------------------------------------------*/
void Plotter :: setVPen(const QVariantMap& m)
{
  QVariantMap::const_iterator i = m.find("style");
  if(i != m.end())
    m_pen.setStyle((Qt::PenStyle)i.value().toInt());
  i = m.find("width");
  if(i != m.end())
    m_pen.setWidth(i.value().toInt());
  i = m.find("color");
  if(i != m.end())
    m_pen.setColor(i.value().value<QColor>());
}
/*----------------------------------------------------------------------------*/
void Plotter :: setVBrush(const QVariantMap& m)
{
  QVariantMap::const_iterator i = m.find("style");
  if(i != m.end())
    m_brush.setStyle((Qt::BrushStyle)i.value().toInt());
  i = m.find("color");
  if(i != m.end())
    m_brush.setColor(i.value().value<QColor>());
}
/*----------------------------------------------------------------------------*/

