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

class Plotter
{
public:
  virtual ~Plotter() {}
  virtual void paint(QPaintDevice *dev) = 0;
};

/*----------------------------------------------------------------------------*/
#endif /*PLOTTER_H_1347869006*/

