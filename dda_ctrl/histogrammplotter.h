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
#ifndef HISTOGRAMMPLOTER_H_1347949203
#define HISTOGRAMMPLOTER_H_1347949203
/*----------------------------------------------------------------------------*/
#include <axisplotter.h>
#include <QList>
#include <QBrush>
#include <QPen>
/*----------------------------------------------------------------------------*/
class HistogrammPlotter : public AxisPlotter
{
protected:
  QBrush m_brush;
  QPen m_pen;
public:
  HistogrammPlotter();
  virtual void paint(QPaintDevice *dev);
  QList<double> data;
  QBrush& brush() {return m_brush;}
  QPen& pen() {return m_pen;}
  void setBrush(const QBrush& b) {m_brush = b;}
  void setPen(const QPen& p) {m_pen = p;}

};
/*----------------------------------------------------------------------------*/
#endif /*HISTOGRAMMPLOTER_H_1347949203*/

