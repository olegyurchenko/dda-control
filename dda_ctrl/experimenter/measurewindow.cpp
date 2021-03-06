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
#include <newpassworddialog.h>
#include <histogrammplotter.h>
#include <curveplotter.h>
#include <optionsdialog.h>
#include <version.h>
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
  m_newMeasure = true;
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
  connect(controller, SIGNAL(sizeReceived(double)), this, SLOT(onSizeReceived(double)));

  connect(session, SIGNAL(sessionChanged()), this, SLOT(onSessionChanged()));
  connect(session, SIGNAL(measureListChanged()), this, SLOT(onMeasureListChanged()));

  ui->sessionBox->setSession(session);

  measureModel = new MeasureModel(this);
  ui->measureTable->setModel(measureModel);
  //connect(session, SIGNAL(measureListChanged()), measureModel, SLOT(update()));

  histogrammPlotter = new HistogrammPlotter();
  //  ui->currentHPlotWidget->setPlotter(histogrammPlotter);
  curvePlotter = new CurvePlotter();
  curvePlotter->x.setShowValues(false);
  curvePlotter->x.setText(tr("[s]"));
  curvePlotter->x.setDecimals(0);
  curvePlotter->x.setSteps(3);
  curvePlotter->x.setMax(3000);


  curvePlotter->y.setShowValues(true);
  curvePlotter->y.setText(tr("[N]"));
  curvePlotter->y.setDecimals(1);

  curvePlotter->setFiled(false);
  curvePlotter->setPointSize(4);

  curvePlotter->setStyle(AxisPlotter::StrokeStyle);


  ui->currentValuePlotWidget->setPlotter(curvePlotter);
  intervalCount = 6;

  controller->start();
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
    ui->actionResumeMeasuring->setEnabled(false);
    //ui->actionSingleStepMode->setEnabled(false);
    ui->actionOptions->setEnabled(true);
    break;
  case DDAController::Idle:
    ui->actionStartSession->setEnabled(true);
    ui->actionResumeMeasuring->setEnabled(session->session().id != InvalidId);
    //ui->actionSingleStepMode->setEnabled(session->session().id != InvalidId);
    ui->actionOptions->setEnabled(true);
    break;
  case DDAController::Calibrating:
    ui->actionStartSession->setEnabled(false);
    ui->actionResumeMeasuring->setEnabled(true);
    //ui->actionSingleStepMode->setEnabled(true);
    ui->actionOptions->setEnabled(false);
    break;
  case DDAController::Measuring:
    ui->actionStartSession->setEnabled(false);
    ui->actionResumeMeasuring->setEnabled(false);
    //ui->actionSingleStepMode->setEnabled(true);
    ui->actionOptions->setEnabled(false);
    break;
  case DDAController::NoParticle:
    ui->actionStartSession->setEnabled(false);
    ui->actionResumeMeasuring->setEnabled(false);
    ui->actionOptions->setEnabled(false);
    break;
  case DDAController::NextCasseteWaiting:
    ui->actionStartSession->setEnabled(false);
    ui->actionResumeMeasuring->setEnabled(true);
    ui->actionOptions->setEnabled(false);
    break;
  case DDAController::EndOfMeasuring:
    ui->actionStartSession->setEnabled(false);
    ui->actionResumeMeasuring->setEnabled(true);
    ui->actionOptions->setEnabled(false);
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
  ui->actionStartSession->setEnabled(true);
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onCurrentStretch(double d)
{
  ui->currentValueLabel->setText(QString::number(d, 'f', 1));
  if(m_newMeasure)
  {
    curvePlotter->data.clear();
    m_measureTime.start();
    m_newMeasure = false;
    curvePlotter->x.setMax(3000);
  }
  int x = m_measureTime.elapsed();

  QPointF p;
  p.setX(x);
  p.setY(d);
  curvePlotter->data.append(p);

  double maxy = 0;
  int size = curvePlotter->data.size();
  for(int i = 0; i < size; i++)
    if(maxy < curvePlotter->data[i].y())
      maxy = curvePlotter->data[i].y();
  curvePlotter->y.setMax(maxy * 1.3);

  while(x >= curvePlotter->x.max())
    curvePlotter->x.setMax(curvePlotter->x.max() + 1000);

  curvePlotter->x.setSteps(curvePlotter->x.max()/1000);

  ui->currentValuePlotWidget->update();
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onSizeReceived(double size)
{
  ui->currentSizeLabel->setText(QString::number(size, 'f', 1));
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onNoParticle()
{
  m_newMeasure = true;
  m_measureTime = QTime();
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onMeasure(double strength, double, int)
{
  onCurrentStretch(strength);

  m_newMeasure = true;
  m_measureTime = QTime();
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
  session->clear();
  SessionDialog dialog;
  if(dialog.exec() == QDialog::Accepted)
  {
    ui->stackedWidget->setCurrentIndex(1);
    controller->setMode(
          database->meshIndex(session->session().standard, session->session().gritIndex),
          session->session().particles,
          ui->actionAutoMode->isChecked() ? DDAController::Auto : DDAController::Manual);

    controller->resume();
  }
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onSessionChanged()
{
  ui->actionEditCurrentSession->setEnabled(session->session().id != InvalidId);
}
/*----------------------------------------------------------------------------*/
double getValue(const DDAMeasure& m)
{
  return m.strenght;
}

/*----------------------------------------------------------------------------*/
void MeasureWindow::onMeasureListChanged()
{
  //Buid histogramm
  DDAMeasureList measureList = session->measureList();
  int size = measureList.size();
  for(int i = 0; i < size; i++)
  {
    if(measureList[i].ignored)
    {
      measureList.erase(measureList.begin() + i);
      i --;
      size --;
    }
  }

  if(size < 5)
  {
    ui->currentHPlotWidget->setPlotter(NULL);
    return;
  }

  double min, max, step, value;
  QList<double> hist;
  value = getValue(measureList[0]);
  min = max = value;

  for(int i = 0; i < size; i++)
  {
    value = getValue(measureList[i]);
    if(min > value)
      min = value;
    if(max < value)
      max = value;
  }

  if(qAbs<double>(min - max) < 0.01)
  {
    ui->currentHPlotWidget->setPlotter(NULL);
    return;
  }

  max += (max - min) / intervalCount;
  min -= (max - min) / intervalCount;

  ui->currentHPlotWidget->setPlotter(histogrammPlotter);
  histogrammPlotter->y.setMin(0);
  histogrammPlotter->y.setMax(100);
  histogrammPlotter->y.setSteps(4);
  histogrammPlotter->y.setText("[%]");
  histogrammPlotter->x.setMin(min);
  histogrammPlotter->x.setMax(max);
  histogrammPlotter->x.setSteps(intervalCount);
  histogrammPlotter->x.setText(tr("[N]"));

  step = (max - min) / intervalCount;

  for(int i = 0; i < intervalCount; i++)
    hist.append(0);

  for(int i = 0; i < size; i++)
  {
    value = getValue(measureList[i]);
    int column = (value - min) / step;
    if(column < intervalCount)
    {
      hist[column] += 100./size;
    }
  }
  histogrammPlotter->data = hist;
  ui->currentHPlotWidget->update();
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
void MeasureWindow::onSetAdminPassword()
{
  if(!passwordCheck(this, SuperUserId))
    return;
  NewPasswordDialog dialog;
  if(dialog.exec() == QDialog::Accepted)
  {
    database->setPassword(SuperUserId, dialog.password());
  }
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onEditCurrentSession()
{
  SessionDialog dialog;
  dialog.exec();
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onResumeMeasuring()
{
  controller->resume();
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onSingleStepMode()
{
  controller->manualMode();
  ui->actionSingleStepMode->setChecked(true);
  ui->actionAutoMode->setChecked(false);
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onAutoMode()
{
  controller->autoMode();
  ui->actionSingleStepMode->setChecked(false);
  ui->actionAutoMode->setChecked(true);
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onOptions()
{
  OptionsDialog dlg;
  if(dlg.exec() == QDialog::Accepted)
  {
    ui->retranslateUi(this);
    disconnect(database, SIGNAL(dbError(QString)));
    connect(database, SIGNAL(dbError(QString)), this, SLOT(onDatabaseError()));
    onProfileChanged(ui->profileCombo->currentIndex());
    onStatusChanged(controller->status());
  }
}
/*----------------------------------------------------------------------------*/
void MeasureWindow::onHelpAbout()
{
  QString contens;

  contens += QString("<p>experimenter %1</p>").arg(versionStr());
  contens +=  tr(
      "<p>The <b>experimenter</b> "
      "is part of a software-hardware system "
      "for testing the static strength of diamond powder.</p>"
      );
  contens +=  tr(
      "<p>Experimenter from device receive "
      "test data and stores them in a database "
      "for later analysis.</p>"
      );

  QMessageBox::about(this, tr("About experimenter"), contens);

}
/*----------------------------------------------------------------------------*/
