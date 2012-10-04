#include "measurewindow.h"
#include "ui_measurewindow.h"
#include <configuration.h>
#include <editprofiledialog.h>
#include <profileselectdialog.h>
#include <database.h>
#include <controller.h>
#include <QMessageBox>
#include <sessiondialog.h>
#include <measuremodel.h>
#include <usermanagedialog.h>
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
  connect(controller, SIGNAL(measure(double,double,int)), this, SLOT(onMeasure(double,double,int)));
  connect(controller, SIGNAL(nextCasseteRequest()), this, SLOT(onNextCasseteRequest()));
  connect(controller, SIGNAL(noParticle()), this, SLOT(onNoParticle()));
  connect(controller, SIGNAL(serialReceived(QString)), this, SLOT(onSerialReceived(QString)));
  connect(controller, SIGNAL(statusChanged(int)), this, SLOT(onStatusChanged(int)));

  connect(session, SIGNAL(sessionChanged()), this, SLOT(onSessionChanged()));
  connect(session, SIGNAL(measureListChanged()), this, SLOT(onMeasureListChanged()));

  measureModel = new MeasureModel(this);
  ui->measureTable->setModel(measureModel);
  connect(session, SIGNAL(measureListChanged()), measureModel, SLOT(update()));
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
  QString str = DDAController::statusString(s);
  ui->statusLabel->setText(str);
  ui->statusLabel_2->setText(str);
  switch(s)
  {
  case DDAController::Offline:
    ui->actionStartSession->setEnabled(false);
    break;
  case DDAController::Idle:
    ui->actionStartSession->setEnabled(true);
    break;
  case DDAController::Calibrating:
    ui->actionStartSession->setEnabled(false);
    break;
  case DDAController::Measuring:
    ui->actionStartSession->setEnabled(false);
    break;
  case DDAController::NoParticle:
    ui->actionStartSession->setEnabled(false);
    break;
  case DDAController::NextCasseteWaiting:
    ui->actionStartSession->setEnabled(false);
    break;
  case DDAController::EndOfMeasuring:
    ui->actionStartSession->setEnabled(false);
    break;
  }
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
void MeasureWindow::onSerialReceived(const QString& str)
{
  ui->serialLabel->setText(str);
  ui->serialLabel_2->setText(str);
  ui->actionStartSession->setEnabled(true);
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onCurrentStretch(double d)
{
  ui->currentValueLabel->setText(QString::number(d, 'f', 1));
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onNoParticle()
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onMeasure(double strength, double size, int number)
{
  ui->currentValueLabel->setText(QString::number(strength, 'f', 1));
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
void MeasureWindow::onStartSession()
{
  SessionDialog dialog;
  session->clear();
  if(dialog.exec() == QDialog::Accepted)
  {
    ui->stackedWidget->setCurrentIndex(1);
    controller->setMode(session->session().meshIndex, session->session().giftCount);
    controller->start();
  }
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onSessionChanged()
{
  DDASession s = session->session();
  ui->userLabel->setText(database->userName(s.userId));
  ui->lotLabel->setText(s.lot);
  ui->meshLabel->setText(database->meshList()[s.meshIndex]);
  ui->gostLabel->setText(database->gostList()[s.gostIndex]);
  ui->giftCountLabel->setText(QString::number(s.giftCount));
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onMeasureListChanged()
{
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onManageUsers()
{
  if(!passwordCheck(this, SuperUserId))
    return;
  UserManageDialog dialog;
  dialog.exec();
}
/*----------------------------------------------------------------------------*/
