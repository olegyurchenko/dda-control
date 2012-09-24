/*----------------------------------------------------------------------------*/
/**
* @pkg curveplotter
*/
/**
* Plotter for function graph as a curve.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 18.09.2012 12:19:22<br>
* @pkgdoc curveplotter
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef CURVEPLOTTER_H_1347959962
#define CURVEPLOTTER_H_1347959962
/*----------------------------------------------------------------------------*/
#include <graphplotter.h>
class CurvePlotter :public GraphPlotter
{
protected:
  bool m_filed;
public:
  CurvePlotter();
  virtual void paint(QPaintDevice *dev);
  bool filed() {return m_filed;}
  void setFiled(bool f) {m_filed = f;}
};

/*----------------------------------------------------------------------------*/
#endif /*CURVEPLOTTER_H_1347959962*/

