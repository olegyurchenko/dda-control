/*----------------------------------------------------------------------------*/
/**
* @pkg axisplotter
*/
/**
* Base class for plot diagrams with x & y axis.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 17.09.2012 11:20:16<br>
* @pkgdoc axisplotter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "axisplotter.h"
#include <QPainter>
#include <QtDebug>
#include <QApplication>
#define TITLE_FONT_MUL 1.5
/*----------------------------------------------------------------------------*/
AxisPlotter :: AxisPlotter(QObject *parent)
  : Plotter(parent)
{
  m_style = BarStyle;
  m_x_border = 3;
  m_y_border = 3;
}
/*----------------------------------------------------------------------------*/
void AxisPlotter :: prepare(QPainter *p)
{
  //qDebug() << "AxisPlotter :: prepare";
  QFont f = p->font();
  QFont tf = f;
  tf.setPointSize(f.pointSize() * TITLE_FONT_MUL);

  m_rect.setLeft(0);
  if(y.showValues())
  {
    QString value;
    int x1, x2;
    value = QString::number(y.min(), 'f', y.decimals());
    x1 = p->fontMetrics().boundingRect(value).width();
    value = QString::number(y.max(), 'f', y.decimals());
    x2 = p->fontMetrics().boundingRect(value).width();
    m_rect.setLeft((x1 > x2 ? x1 : x2) + m_x_border + p->fontMetrics().boundingRect(y.text()).width());
  }

  m_rect.setRight(p->device()->width() - m_x_border);
  m_rect.setBottom(p->device()->height() - m_y_border - p->fontMetrics().boundingRect(x.text()).height());
  if(x.showValues())
  {
    m_rect.setBottom(p->device()->height() - m_y_border - p->fontMetrics().boundingRect("0.00").height()  - p->fontMetrics().boundingRect(x.text()).height());
  }

  int top = m_y_border;
  if(!title().isEmpty())
  {
    QString txt = title();
    txt.remove('\r');
    QStringList lst = txt.split('\n');
    p->setFont(tf);
    foreach(txt, lst)
    {
      top += p->fontMetrics().boundingRect(txt).height();
    }
    top += m_y_border;
    p->setFont(f);
  }
  m_rect.setTop(top);

  m_x_scale = (double)m_rect.width()/(x.max() - x.min());
  m_y_scale = (double)m_rect.height()/(y.max() - y.min());

  //qDebug() << "AxisPlotter :: prepare end";
}
/*----------------------------------------------------------------------------*/
void AxisPlotter :: paint(QPaintDevice *dev)
{
  //qDebug() << "AxisPlotter :: paint";
  QPainter p(dev);
  prepare(&p);
  QFont f = p.font();
  QFont tf = f;
  tf.setPointSize(f.pointSize() * TITLE_FONT_MUL);

  p.setPen(QColor(0, 0, 0));
  p.drawLine(m_rect.left(), m_rect.top(), m_rect.left(), m_rect.bottom());
  p.drawLine(m_rect.right(), m_rect.top(), m_rect.right(), m_rect.bottom());
  p.drawLine(m_rect.left(), m_rect.top(), m_rect.right(), m_rect.top());
  p.drawLine(m_rect.left(), m_rect.bottom(), m_rect.right(), m_rect.bottom());

  if(!title().isEmpty()) //Draw title
  {
    QString txt = title();
    txt.remove('\r');
    QStringList lst = txt.split('\n');
    int y = 0;//m_y_border;
    p.setFont(tf);
    foreach(txt, lst)
    {
      int x = (p.device()->width() - p.fontMetrics().boundingRect(txt).width())/2;
      y +=  p.fontMetrics().boundingRect(txt).height();
      if(x < 0)
        x = 0;
      p.drawText(x, y, txt);
    }
    p.setFont(f);
  }

  int rmargin = 0;
  for(int i = 0; i < x.steps(); i++)
  {
    int xi = m_rect.left() + i * x.step() * m_x_scale;
    if(m_style == BarStyle)
      p.drawLine(xi, m_rect.top(), xi, m_rect.bottom() + 1);
    else
    if(m_style == StrokeStyle)
      p.drawLine(xi, m_rect.bottom() - m_y_border, xi, m_rect.bottom() + 1);

    if(x.showValues())
    {
      QString value = QString::number(x.min() + i * x.step(), 'f', x.decimals());
      int w = p.fontMetrics().boundingRect(value).width();
      int h = p.fontMetrics().boundingRect(value).height();
      if(xi - w/2 - rmargin > m_x_border)
      {
        p.drawText(xi - w/2, m_rect.bottom() + h, value);
        rmargin = xi + w/2;
      }
    }
  }

  if(!x.text().isEmpty())
  {
    int w = p.fontMetrics().boundingRect(x.text()).width();
    //int h = p.fontMetrics().boundingRect(x.text()).height();
    p.drawText(m_rect.right() - w - m_x_border, dev->height() - m_y_border, x.text());
  }

  for(int i = 0; i < y.steps(); i++)
  {
    int yi = m_rect.bottom() - i * y.step() * m_y_scale;
    if(m_style == BarStyle)
      p.drawLine(m_rect.left() - 1, yi, m_rect.right(), yi);
    else
    if(m_style == StrokeStyle)
      p.drawLine(m_rect.left() - 1, yi, m_rect.left() + m_x_border, yi);

    if(y.showValues() && i)
    {
      QString value = QString::number(y.min() + i * y.step(), 'f', y.decimals());
      int w = p.fontMetrics().boundingRect(value).width();
      int h = p.fontMetrics().boundingRect(value).height();
      p.drawText(m_rect.left() - w - m_x_border, yi + h/2, value);
    }
  }

  if(!y.text().isEmpty())
  {
    int h = p.fontMetrics().boundingRect(y.text()).height();
    //int w = p.fontMetrics().boundingRect(y.text()).width();
    p.drawText(m_x_border, m_rect.top() + h/2, y.text());
  }
  //qDebug() << "AxisPlotter :: paint end";
}
/*----------------------------------------------------------------------------*/
