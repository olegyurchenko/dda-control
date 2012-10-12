#include "analyswindow.h"
#include "ui_analyswindow.h"
#include <qmlprocessing.h>
#include <QMessageBox>
#include <plotwidget.h>
#include <QTableView>
#include <QVBoxLayout>
#include <axisplotter.h>

/*----------------------------------------------------------------------------*/
AnalysWindow::AnalysWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::AnalysWindow)
{
  ui->setupUi(this);

  connect(ui->selSessionBox->session(), SIGNAL(sessionChanged()), this, SLOT(onSessionChanged()));
  connect(processing, SIGNAL(modelAdded(QObject*)), this, SLOT(onModelAdded(QObject*)));

  dataLayout = new QVBoxLayout(ui->sessionBox);
  if(!processing->open(ui->selSessionBox->session()))
  {
    QMessageBox::critical(this, tr("Error execute JavaScript"), processing->message());
    return;
  }

  /*
  QTableView *v = new QTableView(this);
  dataLayout->addWidget(v);
  v = new QTableView(this);
  dataLayout->addWidget(v);
  v = new QTableView(this);
  dataLayout->addWidget(v);

  PlotWidget *b = new PlotWidget(this);
  b->setMinimumWidth(120);
  b->setMinimumHeight(120);
  dataLayout->addWidget(b);
  b = new PlotWidget(this);
  b->setMinimumWidth(120);
  b->setMinimumHeight(120);
  dataLayout->addWidget(b);
  b = new PlotWidget(this);
  b->setMinimumWidth(120);
  b->setMinimumHeight(120);
  dataLayout->addWidget(b);
  */


}
/*----------------------------------------------------------------------------*/
void AnalysWindow::onModelAdded(QObject *obj)
{
  if(qobject_cast<AxisPlotter *>(obj))
  {
    PlotWidget *b = new PlotWidget(ui->sessionBox);
    b->setMinimumSize(120, 120);
    dataLayout->addWidget(b);
    b->setPlotter(qobject_cast<AxisPlotter *>(obj));
  }
  else
  if(qobject_cast<QAbstractTableModel *>(obj))
  {
    QTableView *v = new QTableView(ui->sessionBox);
    v->setMidLineWidth(120);
    v->setModel(qobject_cast<QAbstractTableModel *>(obj));
    dataLayout->addWidget(v);
  }
}
/*----------------------------------------------------------------------------*/
AnalysWindow::~AnalysWindow()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void AnalysWindow::onSessionChanged()
{
  processing->update();
  if(processing->isError())
  {
    QMessageBox::critical(this, tr("Error execute JavaScript"), processing->message());
    return;
  }
}
/*----------------------------------------------------------------------------*/
