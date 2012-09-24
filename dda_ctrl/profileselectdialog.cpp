#include "profileselectdialog.h"
#include "ui_profileselectdialog.h"

ProfileSelectDialog::ProfileSelectDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ProfileSelectDialog)
{
  ui->setupUi(this);
}

ProfileSelectDialog::~ProfileSelectDialog()
{
  delete ui;
}
