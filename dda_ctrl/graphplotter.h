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
#ifndef GRAPHPLOTTER_H_1347956468
#define GRAPHPLOTTER_H_1347956468
/*----------------------------------------------------------------------------*/
#include <axisplotter.h>
#include <QList>
#include <QPointF>
#include <QBrush>
#include <QPen>

class GraphPlotter : public AxisPlotter
{
protected:
  QBrush m_brush;
  QPen m_pen;
  int m_pointSIze;
public:
  GraphPlotter();
  virtual void paint(QPaintDevice *dev);
  QList<QPointF> data;
  QBrush& brush() {return m_brush;}
  QPen& pen() {return m_pen;}
  void setBrush(const QBrush& b) {m_brush = b;}
  void setPen(const QPen& p) {m_pen = p;}
  int pointSize() {return m_pointSIze;}
  void setPointSize(int s) {m_pointSIze = s;}
};

/*----------------------------------------------------------------------------*/
#endif /*GRAPHPLOTTER_H_1347956468*/

