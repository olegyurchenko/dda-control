/*----------------------------------------------------------------------------*/
/**
* @pkg plotwidget
*/
/**
* Widget to plot histogramm.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 17.09.2012 10:48:36<br>
* @pkgdoc plotwidget
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef PLOTWIDGET_H_1347868116
#define PLOTWIDGET_H_1347868116
/*----------------------------------------------------------------------------*/
#include <QWidget>

class Plotter;
class PlotWidget : public QWidget
{
public:
  PlotWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
  ~PlotWidget();
protected:
  Plotter *m_plotter;
  virtual void paintEvent (QPaintEvent * event);
public:
  void setPlotter(Plotter *p) {m_plotter = p;}
  Plotter *plotter() {return m_plotter;}
};

/*----------------------------------------------------------------------------*/
#endif /*PLOTWIDGET_H_1347868116*/

