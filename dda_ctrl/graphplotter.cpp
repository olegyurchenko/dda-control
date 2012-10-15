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
#include <QtDebug>
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

  //qDebug() << "CurvePlotter :: paint";
  QPainter p(dev);
  AxisPlotter::prepare(&p);
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
    if(xi < 0)
      xi = 0;
    if(yi < 0)
      yi = 0;
    p.drawRect(xi, yi, m_pointSIze, m_pointSIze);
  }
  p.end();
  AxisPlotter::paint(dev);
  //qDebug() << "CurvePlotter :: paint end";
}
/*----------------------------------------------------------------------------*/

