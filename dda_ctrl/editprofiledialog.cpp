#include "editprofiledialog.h"
#include "ui_editprofiledialog.h"
#include <QIntValidator>

/*----------------------------------------------------------------------------*/
EditProfileDialog::EditProfileDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EditProfileDialog)
{
  ui->setupUi(this);
}
/*----------------------------------------------------------------------------*/
void EditProfileDialog::setProfile(const DDAProfile& p)
{
  m_profile = p;

  ui->nameEdit->setText(m_profile.name);

  QValidator *validator = new QIntValidator(300, 115200, this);

  ui->baudCombo->addItem("300");
  ui->baudCombo->addItem("600");
  ui->baudCombo->addItem("1200");
  ui->baudCombo->addItem("2400");
  ui->baudCombo->addItem("4800");
  ui->baudCombo->addItem("9600");
  ui->baudCombo->addItem("19200");
  ui->baudCombo->addItem("38400");
  ui->baudCombo->addItem("57600");
  ui->baudCombo->addItem("115200");
  ui->baudCombo->setCurrentIndex(5);
  ui->baudCombo->setEditText(QString::number(m_profile.baud));
  ui->baudCombo->setValidator(validator);

  ui->serialSpin->setValue(m_profile.serial);
}
/*----------------------------------------------------------------------------*/
EditProfileDialog::~EditProfileDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void EditProfileDialog::onAccepted()
{
  m_profile.name = ui->nameEdit->text();
  m_profile.serial = ui->serialSpin->value();
  m_profile.baud = ui->baudCombo->currentText().toInt();
  accept();
}
/*----------------------------------------------------------------------------*/
