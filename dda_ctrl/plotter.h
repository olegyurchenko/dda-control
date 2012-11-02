/*----------------------------------------------------------------------------*/
/**
* @pkg plotter
*/
/**
* Base class for plot chart.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 17.09.2012 11:03:25<br>
* @pkgdoc plotter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef PLOTTER_H_1347869006
#define PLOTTER_H_1347869006
/*----------------------------------------------------------------------------*/
#include <QPaintDevice>
#include <QObject>
#include <QVariant>
#include <QPen>
#include <QBrush>
/*----------------------------------------------------------------------------*/
class Plotter : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString legend READ legend WRITE setLegend)
  Q_PROPERTY(QVariantMap brush READ vBrush WRITE setVBrush)
  Q_PROPERTY(QVariantMap pen READ vPen WRITE setVPen)
protected:
  QString m_legend;
  QBrush m_brush;
  QPen m_pen;
public:
  Plotter(QObject *parent = 0);
  virtual ~Plotter() {}
  virtual void paint(QPaintDevice *dev) = 0;

  QString legend() {return m_legend;}
  void setLegend(const QString &l) {m_legend = l;}
  /**For QML adaptors*/
  QVariantMap vPen() const;
  QVariantMap vBrush() const;
  void setVPen(const QVariantMap& m);
  void setVBrush(const QVariantMap& m);
};

/*----------------------------------------------------------------------------*/
#endif /*PLOTTER_H_1347869006*/

