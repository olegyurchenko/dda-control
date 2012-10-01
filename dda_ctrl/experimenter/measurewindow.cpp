#include "measurewindow.h"
#include "ui_measurewindow.h"
#include <configuration.h>
#include <editprofiledialog.h>
#include <profileselectdialog.h>
#include <database.h>
#include <controller.h>
#include <QMessageBox>
/*----------------------------------------------------------------------------*/
MeasureWindow::MeasureWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MeasureWindow)
{
  m_created = false;
  ui->setupUi(this);
  ui->profileCombo->addItems(config->profileList());
  ui->profileCombo->setCurrentIndex(config->profileIndex());
  m_created = true;
  onProfileChanged(config->profileIndex());

  connect(database, SIGNAL(dbError(QString)), this, SLOT(onDatabaseError()));

  connect(controller, SIGNAL(cmdSendError()), this, SLOT(onCmdSendError()));
  connect(controller, SIGNAL(cmdSendOk()), this, SLOT(onCmdSendOk()));
  connect(controller, SIGNAL(currentStretch(double)), this, SLOT(onCurrentStretch(double)));
  connect(controller, SIGNAL(endOfMeasuring()), this, SLOT(onEndOfMeasuring()));
  connect(controller, SIGNAL(giftSize(double)), this, SLOT(onGiftSize(double)));
  connect(controller, SIGNAL(measure(double,int,int)), this, SLOT(onMeasure(double,int,int)));
  connect(controller, SIGNAL(nextCasseteRequest()), this, SLOT(onNextCasseteRequest()));
  connect(controller, SIGNAL(noParticle()), this, SLOT(onNoParticle()));
  connect(controller, SIGNAL(serialReceived(QString)), this, SLOT(onSerialReceived(QString)));
  connect(controller, SIGNAL(statusChanged(int)), this, SLOT(onStatusChanged(int)));

}
/*----------------------------------------------------------------------------*/
MeasureWindow::~MeasureWindow()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onProfileChanged(int index)
{
  if(!m_created)
    return;

  if(index != config->profileIndex())
    config->setProfileIndex(index);

  DDAProfile profile = config->profile();
  ui->profileCombo->setCurrentIndex(index);
  ui->profileLabel->setText(
        tr("Serial:%1 Baud:%2").arg(profile.serial).arg(profile.baud)
        );
  if(!controller->open(profile.serial, profile.baud))
    QMessageBox::critical(this, tr("Error"), tr("Error open serial port"));
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onManageProfile()
{
  ProfileSelectDialog dlg;
  if(dlg.exec() != QDialog::Accepted)
    return;
  ui->profileCombo->clear();
  ui->profileCombo->addItems(config->profileList());
  onProfileChanged(dlg.selectedProfile());
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onEditProfile()
{
  EditProfileDialog dlg;
  dlg.setProfile(config->profile());
  if(dlg.exec() != QDialog::Accepted)
    return;
  config->setProfile(dlg.profile());
  ui->profileCombo->clear();
  ui->profileCombo->addItems(config->profileList());
  onProfileChanged(config->profileIndex());
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onDatabaseError()
{
  QMessageBox::critical(this, "Database error", database->message());
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onStatusChanged(int s)
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onCmdSendOk()
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onCmdSendError()
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onSerialReceived(const QString&)
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onCurrentStretch(double)
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onNoParticle()
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onMeasure(double strength, int number, int nextCell)
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onGiftSize(double size)
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onNextCasseteRequest()
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onEndOfMeasuring()
{
}
/*----------------------------------------------------------------------------*/
