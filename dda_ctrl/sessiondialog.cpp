#include "sessiondialog.h"
#include "ui_sessiondialog.h"

SessionDialog::SessionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SessionDialog)
{
  ui->setupUi(this);
}

SessionDialog::~SessionDialog()
{
  delete ui;
}
