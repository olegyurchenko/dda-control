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
/*----------------------------------------------------------------------------*/
class Plotter : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString legend READ legend WRITE setLegend)
protected:
  QString m_legend;
public:
  Plotter(QObject *parent = 0) : QObject(parent) {}
  virtual ~Plotter() {}
  virtual void paint(QPaintDevice *dev) = 0;

  QString legend() {return m_legend;}
  void setLegend(const QString &l) {m_legend = l;}
};

/*----------------------------------------------------------------------------*/
#endif /*PLOTTER_H_1347869006*/

