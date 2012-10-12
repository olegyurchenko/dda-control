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
#ifdef USE_QML
#include <qdeclarative.h>
#endif
/*----------------------------------------------------------------------------*/
class HistogrammPlotter : public AxisPlotter
{
  Q_OBJECT
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
#ifdef USE_QML
  Q_PROPERTY(QDeclarativeListProperty<double> data READ getData)
#endif
protected:
  QBrush m_brush;
  QPen m_pen;
public:
  HistogrammPlotter(QObject *parent = 0);
  virtual void paint(QPaintDevice *dev);
  QList<double> data;
  QBrush& brush() {return m_brush;}
  QPen& pen() {return m_pen;}
  void setBrush(const QBrush& b) {m_brush = b;}
  void setPen(const QPen& p) {m_pen = p;}

#ifdef USE_QML
  static void dataListAppend(QDeclarativeListProperty<double> *property, double *value);
  static double *dataListAt(QDeclarativeListProperty<double> *property, int index);
  static int dataListCount(QDeclarativeListProperty<double> *property);
  static void dataListClear(QDeclarativeListProperty<double> *property);
  QDeclarativeListProperty<double> getData();
#endif

public slots:
  void clear() {data.clear();}
  void add(double d) {data.append(d);}

};
/*----------------------------------------------------------------------------*/
#endif /*HISTOGRAMMPLOTER_H_1347949203*/

