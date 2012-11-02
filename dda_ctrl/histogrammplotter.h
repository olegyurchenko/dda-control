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
#include <QVariantList>
#include <QBrush>
#include <QPen>
#ifdef USE_QML
#include <qdeclarative.h>
#endif
/*----------------------------------------------------------------------------*/
/*
#ifdef USE_QML
Q_DECLARE_METATYPE(QDeclarativeListProperty<double>)
#endif
*/
class HistogrammPlotter : public AxisPlotter
{
  Q_OBJECT
  Q_PROPERTY(QVariantList data READ getData WRITE setData)
public:
  HistogrammPlotter(QObject *parent = 0);
  virtual void paint(QPaintDevice *dev);
  QList<double> data;
  QBrush& brush() {return m_brush;}
  QPen& pen() {return m_pen;}
  void setBrush(const QBrush& b) {m_brush = b;}
  void setPen(const QPen& p) {m_pen = p;}

  QVariantList getData();
  void setData(const QVariantList& d);

public slots:
  void clear() {data.clear();}
  void add(double d) {data.append(d);}

};
/*----------------------------------------------------------------------------*/
#endif /*HISTOGRAMMPLOTER_H_1347949203*/

