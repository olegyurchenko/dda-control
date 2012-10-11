#include "analyswindow.h"
#include "ui_analyswindow.h"
#include <qmlprocessing.h>
#include <QMessageBox>
/*----------------------------------------------------------------------------*/
AnalysWindow::AnalysWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::AnalysWindow)
{
  ui->setupUi(this);
  ui->sessionBox->setSession(ui->selSessionBox->session());
  connect(ui->selSessionBox->session(), SIGNAL(sessionChanged()), this, SLOT(onSessionChanged()));
}
/*----------------------------------------------------------------------------*/
AnalysWindow::~AnalysWindow()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void AnalysWindow::onSessionChanged()
{
  if(!processing->open(ui->selSessionBox->session()))
  {
    QMessageBox::critical(this, tr("Error execute QML"), processing->message());
    return;
  }
}
/*----------------------------------------------------------------------------*/
