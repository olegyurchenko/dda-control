/*----------------------------------------------------------------------------*/
/**
* @pkg graphplotter
*/
/**
* Plotter for function graph as singe squares.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 18.09.2012 11:21:07<br>
* @pkgdoc graphplotter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "graphplotter.h"
#include <QPainter>
/*----------------------------------------------------------------------------*/
GraphPlotter :: GraphPlotter(QObject *parent)
  : AxisPlotter(parent)
{
  m_pen.setColor(QColor("black"));
  m_pen.setStyle(Qt::SolidLine);
  m_pen.setWidth(1);
  //m_pen.setStyle(Qt::NoPen);

  m_brush.setColor(QColor("royalblue"));
  m_brush.setStyle(Qt::SolidPattern);
  setPointSize(6);
}
/*----------------------------------------------------------------------------*/
void GraphPlotter :: paint(QPaintDevice *dev)
{
  AxisPlotter::prepare(dev);

  QPainter p(dev);
  p.setPen(m_pen);
  p.setBrush(m_brush);

  int size = data.size();
  for(int i = 0; i < size; i++)
  {
    const QPointF& point = data.at(i);
    int xi = m_rect.left() + (point.x() - x.min()) * m_x_scale;
    int yi = m_rect.bottom() - (point.y() - y.min()) * m_y_scale;
    xi -= m_pointSIze/2;
    yi -= m_pointSIze/2;
    p.drawRect(xi, yi, m_pointSIze, m_pointSIze);
  }
  AxisPlotter::paint(dev);
}
/*----------------------------------------------------------------------------*/
#ifdef USE_QML
void GraphPlotter :: dataListAppend(QDeclarativeListProperty<QPointF> *property, QPointF *value)
{
  GraphPlotter *self = qobject_cast<GraphPlotter*>(property->object);
  self->data.append(*value);
}
/*----------------------------------------------------------------------------*/
QPointF *GraphPlotter :: dataListAt(QDeclarativeListProperty<QPointF> *property, int index)
{
  GraphPlotter *self = qobject_cast<GraphPlotter*>(property->object);
  return &self->data[index];
}
/*----------------------------------------------------------------------------*/
int GraphPlotter :: dataListCount(QDeclarativeListProperty<QPointF> *property)
{
  GraphPlotter *self = qobject_cast<GraphPlotter*>(property->object);
  return self->data.size();
}
/*----------------------------------------------------------------------------*/
void GraphPlotter :: dataListClear(QDeclarativeListProperty<QPointF> *property)
{
  GraphPlotter *self = qobject_cast<GraphPlotter*>(property->object);
  self->data.clear();
}
/*----------------------------------------------------------------------------*/
QDeclarativeListProperty<QPointF> GraphPlotter :: getData()
{
  return QDeclarativeListProperty<QPointF>(this, 0, dataListAppend, dataListCount, dataListAt, dataListClear);
}
/*----------------------------------------------------------------------------*/
#endif

