#include "analyswindow.h"
#include "ui_analyswindow.h"
#include <qmlprocessing.h>
#include <QMessageBox>
#include <plotwidget.h>
#include <QTableView>
#include <QVBoxLayout>
#include <axisplotter.h>
#include <editdatadialog.h>
#include <usermanagedialog.h>
/*----------------------------------------------------------------------------*/
AnalysWindow::AnalysWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::AnalysWindow)
{
  ui->setupUi(this);

  m_colCount = 2;
  m_col = 0;
  m_row = 0;

  connect(ui->selSessionBox->session(), SIGNAL(sessionChanged()), this, SLOT(onSessionChanged()));
  connect(processing, SIGNAL(modelAdded(QObject*)), this, SLOT(onModelAdded(QObject*)));

  //ui->scrollArea->setWidget(ui->sessionBox);

  dataLayout = new QGridLayout(ui->scrollAreaWidgetContents);
  dataLayout->addWidget(ui->selSessionBox, m_col ++, m_row);
  if(!processing->open(ui->selSessionBox->session()))
  {
    QMessageBox::critical(this, tr("Error execute JavaScript"), processing->message());
    return;
  }

}
/*----------------------------------------------------------------------------*/
void AnalysWindow::onModelAdded(QObject *obj)
{
  QWidget *w = NULL;
  if(qobject_cast<AxisPlotter *>(obj))
  {
    PlotWidget *b = new PlotWidget(ui->scrollAreaWidgetContents);
    b->setMinimumSize(200, 200);
    b->setPlotter(qobject_cast<AxisPlotter *>(obj));
    w = b;
  }
  else
  if(qobject_cast<QAbstractTableModel *>(obj))
  {
    QTableView *v = new QTableView(ui->scrollAreaWidgetContents);
    v->setMinimumSize(200, 200);
    v->setModel(qobject_cast<QAbstractTableModel *>(obj));
    w = v;
  }

  if(w)
  {
    dataLayout->addWidget(w, m_row, m_col ++);
    if(m_col >= m_colCount)
    {
      m_col = 0;
      m_row ++;
    }
  }
  ui->scrollAreaWidgetContents->adjustSize();
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
  ui->scrollAreaWidgetContents->update();
}
/*----------------------------------------------------------------------------*/
void AnalysWindow::onEditData()
{
  if(!passwordCheck(this, ui->selSessionBox->session()->session().userId))
    return;

  EditDataDialog dlg(this, ui->selSessionBox->session());
  dlg.exec();
}
/*----------------------------------------------------------------------------*/
void AnalysWindow::onGenerateReport()
{
}
/*----------------------------------------------------------------------------*/
