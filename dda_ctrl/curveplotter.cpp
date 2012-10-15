/*----------------------------------------------------------------------------*/
/**
* @pkg curveplotter
*/
/**
* Plotter for function graph as a curve.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 18.09.2012 12:19:22<br>
* @pkgdoc curveplotter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "curveplotter.h"
#include <QPainter>
#include <QVector>
#include <QPoint>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
CurvePlotter :: CurvePlotter(QObject *parent)
  : GraphPlotter(parent)
{
  m_pen.setColor(QColor("black"));
  m_pen.setStyle(Qt::SolidLine);
  m_pen.setWidth(1);
  //m_pen.setStyle(Qt::NoPen);

  m_brush.setColor(QColor("royalblue"));
  m_brush.setStyle(Qt::SolidPattern);
  setPointSize(0);
  m_filed = true;
}
/*----------------------------------------------------------------------------*/
void CurvePlotter :: paint(QPaintDevice *dev)
{
  //qDebug() << "CurvePlotter :: paint";

  QPainter p(dev);
  AxisPlotter::prepare(&p);
  p.setPen(m_pen);
  p.setBrush(m_brush);

  int size = data.size();
  QVector<QPoint> points;
  for(int i = 0; i < size; i++)
  {
    const QPointF& point = data.at(i);
    int xi = m_rect.left() + (point.x() - x.min()) * m_x_scale;
    int yi = m_rect.bottom() - (point.y() - y.min()) * m_y_scale;
    if(xi < m_rect.left())
      xi = m_rect.left();
    if(xi > m_rect.right())
      xi = m_rect.right();
    if(yi < m_rect.top())
      yi = m_rect.top();
    if(yi > m_rect.bottom())
      yi = m_rect.bottom();

    points.append(QPoint(xi, yi));
  }

  if(m_filed && size)
  {
    points.push_back(QPoint(points.last().x(), m_rect.bottom()));
    points.push_back(QPoint(points.first().x(), m_rect.bottom()));
    p.drawPolygon(QPolygon(points), Qt::WindingFill);
  }
  else
    p.drawPolyline(QPolygon(points));

  p.end();
  if(pointSize())
    GraphPlotter::paint(dev);
  else
    AxisPlotter::paint(dev);

  //qDebug() << "CurvePlotter :: paint end";
}
/*----------------------------------------------------------------------------*/

