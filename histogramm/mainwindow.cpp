#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <histogrammplotter.h>
#include <graphplotter.h>
#include <curveplotter.h>

HistogrammPlotter *plotter1, *plotter2;
GraphPlotter *plotter3;
CurvePlotter *plotter4;
/*----------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  plotter1 = new HistogrammPlotter();

  plotter1->x.setMin(19);
  plotter1->x.setMax(1255);
  plotter1->x.setSteps(12);
  plotter1->x.setDecimals(0);
  plotter1->x.setText("[H]");

  plotter1->y.setMin(0);
  plotter1->y.setMax(37);
  plotter1->y.setSteps(11);
  plotter1->y.setDecimals(0);
  plotter1->y.setText("[%]");

  plotter1->data.append(23.3);
  plotter1->data.append(36.7);
  plotter1->data.append(13.3);
  plotter1->data.append(10.0);
  plotter1->data.append(3.3);
  plotter1->data.append(6.7);
  plotter1->data.append(3.3);
  plotter1->data.append(0.);
  plotter1->data.append(0.);
  plotter1->data.append(0.);
  plotter1->data.append(0.);
  plotter1->data.append(3.3);

  plotter1->setStyle(AxisPlotter::StrokeStyle);
  //plotter->setStyle(AxisPlotter::BarStyle);
  plotter1->setTitle(QString::fromUtf8("Распределение по прочности"));
  ui->histogrammWidget->setPlotter(plotter1);

  plotter2 = new HistogrammPlotter();

  plotter2->x.setMin(377);
  plotter2->x.setMax(881);
  plotter2->x.setSteps(12);
  plotter2->x.setDecimals(0);
  plotter2->x.setText(QString::fromUtf8("[мкм]"));

  plotter2->y.setMin(0);
  plotter2->y.setMax(27);
  plotter2->y.setSteps(11);
  plotter2->y.setDecimals(0);
  plotter2->y.setText("[%]");

  plotter2->data.append(3.3);
  plotter2->data.append(0.);
  plotter2->data.append(0.);
  plotter2->data.append(0.);
  plotter2->data.append(0.);
  plotter2->data.append(10.0);
  plotter2->data.append(13.3);
  plotter2->data.append(10.0);
  plotter2->data.append(27);
  plotter2->data.append(6.7);
  plotter2->data.append(8.5);
  plotter2->data.append(13.3);

  plotter2->setStyle(AxisPlotter::StrokeStyle);
  plotter2->setPen(QPen(Qt::SolidLine));
  plotter2->setTitle(QString::fromUtf8("Распределение по размеру"));
  ui->histogrammWidget2->setPlotter(plotter2);


  plotter3 = new GraphPlotter();

  plotter3->x.setMin(377.0);
  plotter3->x.setMax(877.0);
  plotter3->x.setSteps(4);
  plotter3->x.setDecimals(0);
  plotter3->x.setText(QString::fromUtf8("[мкм]"));

  plotter3->y.setMin(43);
  plotter3->y.setMax(1429);
  plotter3->y.setSteps(5);
  plotter3->y.setDecimals(0);
  plotter3->y.setText(QString::fromUtf8("Н/мм2"));

  plotter3->data.append(QPointF(377,45));
  plotter3->data.append(QPointF(600,300));
  plotter3->data.append(QPointF(620,320));
  plotter3->data.append(QPointF(700,500));
  plotter3->data.append(QPointF(377,445));
  plotter3->data.append(QPointF(600,1300));
  plotter3->data.append(QPointF(655,1320));
  plotter3->data.append(QPointF(730,900));
  plotter3->setTitle(QString::fromUtf8("Прочность к размеру зерна"));
  ui->chartWidget->setPlotter(plotter3);


  plotter4 = new CurvePlotter();

  plotter4->x.setMin(19.0);
  plotter4->x.setMax(1255.0);
  plotter4->x.setSteps(12);
  plotter4->x.setDecimals(0);
  plotter4->x.setText(QString::fromUtf8("[H]"));

  plotter4->y.setMin(0);
  plotter4->y.setMax(100);
  plotter4->y.setSteps(5);
  plotter4->y.setDecimals(0);
  plotter4->y.setText(QString::fromUtf8("%"));

  plotter4->data.append(QPointF(19,100));
  plotter4->data.append(QPointF(122,80));
  plotter4->data.append(QPointF(225,40));
  plotter4->data.append(QPointF(328,30));
  plotter4->data.append(QPointF(431,20));
  plotter4->data.append(QPointF(534,15));
  plotter4->data.append(QPointF(637,10));
  plotter4->data.append(QPointF(740,5));
  plotter4->data.append(QPointF(1152,5));

  plotter4->setFiled(false);
  plotter4->setPointSize(4);

  plotter4->setTitle(QString::fromUtf8("Плотность распределения\nпо прочности"));
  ui->chartWidget2->setPlotter(plotter4);
}
/*----------------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
  delete plotter1;
  delete ui;
}
/*----------------------------------------------------------------------------*/
