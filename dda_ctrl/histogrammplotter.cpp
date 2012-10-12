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
  AxisPlotter::prepare(dev);
  int step = x.step() * m_x_scale;
  int w = step / 2;

  QPainter p(dev);
  p.setPen(m_pen);
  p.setBrush(m_brush);

  int size = data.size();
  for(int i = 0; i < size && i < x.steps(); i++)
  {
    int xi = m_rect.left() + i * x.step() * m_x_scale;
    int yi = m_rect.bottom() - data[i] * m_y_scale;

    xi += (step - w)/2;

    if(yi < m_rect.top())
      yi = m_rect.top();

    if(m_rect.height() - yi > 0)
      p.drawRect(xi, yi + 1, w, m_rect.height() - yi + (m_pen.style() != Qt::NoPen ? 0 : 2));
  }
  AxisPlotter::paint(dev);
}
/*----------------------------------------------------------------------------*/
#ifdef USE_QML
void HistogrammPlotter :: dataListAppend(QDeclarativeListProperty<double> *property, double *value)
{
  HistogrammPlotter *self = qobject_cast<HistogrammPlotter *>(property->object);
  self->data.append(*value);
}
/*----------------------------------------------------------------------------*/
double *HistogrammPlotter :: dataListAt(QDeclarativeListProperty<double> *property, int index)
{
  HistogrammPlotter *self = qobject_cast<HistogrammPlotter *>(property->object);
  return &self->data[index];
}
/*----------------------------------------------------------------------------*/
int HistogrammPlotter :: dataListCount(QDeclarativeListProperty<double> *property)
{
  HistogrammPlotter *self = qobject_cast<HistogrammPlotter *>(property->object);
  return self->data.size();
}
/*----------------------------------------------------------------------------*/
void HistogrammPlotter :: dataListClear(QDeclarativeListProperty<double> *property)
{
  HistogrammPlotter *self = qobject_cast<HistogrammPlotter *>(property->object);
  self->data.clear();
}
/*----------------------------------------------------------------------------*/
QDeclarativeListProperty<double> HistogrammPlotter :: getData()
{
  return QDeclarativeListProperty<double>(this, 0, dataListAppend, dataListCount, dataListAt, dataListClear);
}
/*----------------------------------------------------------------------------*/
#endif
