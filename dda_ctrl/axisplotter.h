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
/*----------------------------------------------------------------------------*/
class Axis
{
protected:
  bool m_modified;
  double m_min;
  double m_max;
  int m_steps;
  int m_decimals;
  bool m_showValues;
  QString m_text;
public:
  Axis() {
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
  void prepare(QPaintDevice *dev);
public:
  AxisPlotter();
  Axis x;
  Axis y;
  virtual void paint(QPaintDevice *dev);
  AxisPlotter::Style style() const {return m_style;}
  void setStyle(AxisPlotter::Style s) {m_style = s;}
  QRect& rect() {return m_rect;}
};
/*----------------------------------------------------------------------------*/
#endif /*AXISPLOTTER_H_1347870016*/

