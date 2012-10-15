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
#ifndef AXISPLOTTER_H_1347870016
#define AXISPLOTTER_H_1347870016
/*----------------------------------------------------------------------------*/
#include <plotter.h>
#include <QString>
#include <QPainter>
/*----------------------------------------------------------------------------*/
class Axis : public QObject
{
  Q_OBJECT
  Q_PROPERTY(qreal min READ min WRITE setMin)
  Q_PROPERTY(qreal max READ max WRITE setMax)
  Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
  Q_PROPERTY(int steps READ steps WRITE setSteps)
  Q_PROPERTY(qreal step READ step)
  Q_PROPERTY(bool showValues READ showValues WRITE setShowValues)
  Q_PROPERTY(QString text READ text WRITE setText)

protected:
  bool m_modified;
  double m_min;
  double m_max;
  int m_steps;
  int m_decimals;
  bool m_showValues;
  QString m_text;
public:
  Axis(QObject *parent = 0)
    : QObject(parent)
  {
    m_modified = false;
    m_min = 0;
    m_max = 1;
    m_steps = 3;
    m_decimals = 2;
    m_showValues = true;
  }

  double min() const {return m_min;}
  void setMin(double m)
  {
    if(m == m_min)
      return;
    m_modified = true;
    m_min = m;
  }

  double max() const {return m_max;}
  void setMax(double m)
  {
    if(m == m_max)
      return;
    m_modified = true;
    m_max = m;
  }

  int steps() const {return m_steps;}
  void setSteps(int s)
  {
    if(s == m_steps)
      return;
    m_modified = true;
    m_steps = s;
  }

  double step() const {return (m_max - m_min)/(double)(!m_steps ? 1 : m_steps);}

  int decimals() const {return m_decimals;}
  void setDecimals(int d)
  {
    if(d == m_decimals)
      return;
    m_modified = true;
    m_decimals = d;
  }

  bool showValues() const {return m_showValues;}
  void setShowValues(bool s)
  {
    if(s == m_showValues)
      return;
    m_modified = true;
    m_showValues = s;
  }

  QString text() const {return m_text;}
  void setText(const QString& t)
  {
    if(t == m_text)
      return;
    m_modified = true;
    m_text = t;
  }

  bool modified() const {return m_modified;}
  void setModified(bool m) {m_modified = m;}
};
/*----------------------------------------------------------------------------*/
class AxisPlotter : public Plotter
{
  Q_OBJECT
  Q_PROPERTY(QObject* x READ getX)
  Q_PROPERTY(QObject* y READ getY)
  Q_PROPERTY(int style READ style WRITE setStyle)
  Q_ENUMS(Style)
public:
  enum Style
  {
    BarStyle,
    StrokeStyle,
    CleanStyle
  };
protected:
  AxisPlotter::Style m_style;
  int m_x_border;
  int m_y_border;
  QRect m_rect;
  double m_x_scale;
  double m_y_scale;
  void prepare(QPainter *p);
  Axis* getX() {return &x;}
  Axis* getY() {return &y;}
public:
  AxisPlotter(QObject *parent = 0);
  Axis x;
  Axis y;
  virtual void paint(QPaintDevice *dev);
  AxisPlotter::Style style() const {return m_style;}
  void setStyle(int s) {m_style = (AxisPlotter::Style)s;}
  QRect& rect() {return m_rect;}
};
/*----------------------------------------------------------------------------*/
#endif /*AXISPLOTTER_H_1347870016*/

