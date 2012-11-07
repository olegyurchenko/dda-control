/*----------------------------------------------------------------------------*/
/**
* @pkg histogrammploter
*/
/**
* Class for draw histogramm.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 18.09.2012  9:20:03<br>
* @pkgdoc histogrammploter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "histogrammplotter.h"
#include <QPainter>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
HistogrammPlotter :: HistogrammPlotter(QObject *parent)
  : AxisPlotter(parent)
{
  m_pen.setColor(QColor("black"));
  //m_pen.setStyle(Qt::SolidLine);
  //m_pen.setWidth(1);
  m_pen.setStyle(Qt::NoPen);

  m_brush.setColor(QColor("royalblue"));
  m_brush.setStyle(Qt::SolidPattern);
}
/*----------------------------------------------------------------------------*/
void HistogrammPlotter :: paint(QPaintDevice *dev)
{
  //qDebug() << "HistogrammPlotter :: paint";

  QPainter p(dev);
  AxisPlotter::prepare(&p);

  int step = x.step() * m_x_scale;
  int w = step / 2;

  p.setPen(m_pen);
  p.setBrush(m_brush);

  int size = data.size();
  for(int i = 0; i < size && i < x.steps(); i++)
  {
    int xi = m_rect.left() + i * x.step() * m_x_scale;
    int yi = m_rect.height() - data[i] * m_y_scale;

    xi += (step - w)/2;

    if(yi < m_rect.top())
      yi = m_rect.top();

    if(m_rect.height() - yi > 0)
      p.drawRect(xi, yi + 1, w, m_rect.bottom() - yi - (m_pen.style() != Qt::NoPen ? 0 : 1));
  }
  p.end();
  AxisPlotter::paint(dev);
  //qDebug() << "HistogrammPlotter :: paint end";
}
/*----------------------------------------------------------------------------*/
QVariantList HistogrammPlotter::getData()
{
  QVariantList lst;
  int size = data.size();
  for(int i = 0; i < size; i++)
    lst.append(data[i]);
  return lst;
}
/*----------------------------------------------------------------------------*/
void HistogrammPlotter::setData(const QVariantList& d)
{
  data.clear();
  int size = d.size();
  for(int i = 0; i < size; i++)
    data.append(d[i].toDouble());
}
/*----------------------------------------------------------------------------*/

