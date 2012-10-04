#include "newpassworddialog.h"
#include "ui_newpassworddialog.h"
#include <QPushButton>
/*----------------------------------------------------------------------------*/
NewPasswordDialog::NewPasswordDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::NewPasswordDialog)
{
  ui->setupUi(this);
  onPasswordChanged();
}
/*----------------------------------------------------------------------------*/
NewPasswordDialog::~NewPasswordDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void NewPasswordDialog::onPasswordChanged()
{
  QString text1 = ui->passEdit1->text();
  QString text2 = ui->passEdit2->text();
  if(text1 == text2)
  {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    m_password = text1;
  }
  else
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}
/*----------------------------------------------------------------------------*/
