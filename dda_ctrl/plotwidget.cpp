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
#include "plotwidget.h"
#include <plotter.h>
/*----------------------------------------------------------------------------*/
PlotWidget :: PlotWidget(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  m_plotter = NULL;
}
/*----------------------------------------------------------------------------*/
PlotWidget :: ~PlotWidget()
{
}
/*----------------------------------------------------------------------------*/
void PlotWidget :: paintEvent (QPaintEvent * event)
{
  QWidget::paintEvent(event);
  if(m_plotter != NULL)
    m_plotter->paint(this);
}
/*----------------------------------------------------------------------------*/
