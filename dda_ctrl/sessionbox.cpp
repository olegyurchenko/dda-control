#include "sessionbox.h"
#include "ui_sessionbox.h"
#include <session.h>
#include <database.h>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
SessionBox::SessionBox(QWidget *parent) :
  QGroupBox(parent),
  ui(new Ui::SessionBox)
{
  ui->setupUi(this);
  m_session = NULL;
}
/*----------------------------------------------------------------------------*/
SessionBox::~SessionBox()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void SessionBox::setSession(DDAMeasureSession *s)
{
  if(m_session != NULL)
  {
    disconnect(m_session, SIGNAL(sessionChanged()));
    disconnect(m_session, SIGNAL(measureListChanged()));
  }
  m_session = s;
  if(m_session != NULL)
  {
    connect(m_session, SIGNAL(sessionChanged()), this, SLOT(onSessionChanged()));
    connect(m_session, SIGNAL(measureListChanged()), this, SLOT(onMeasuringChanged()));
  }
  onSessionChanged();
  if(m_session->session().id != InvalidId)
    onMeasuringChanged();
}
/*----------------------------------------------------------------------------*/
void SessionBox::onSessionChanged()
{
  qDebug() << "SessionBox::onSessionChanged()";
  DDASession s = m_session->session();
  ui->serialLabel->setText(s.deviceSerial);
  ui->userLabel->setText(database->userName(s.userId));
  ui->lotLabel->setText(s.lot);
  ui->standardLabel->setText(database->standardList()[s.standard]);
  ui->gritLabel->setText(database->gritList(s.standard)[s.gritIndex]);
  ui->particlesLabel->setText(QString::number(s.particles));
}
/*----------------------------------------------------------------------------*/
void SessionBox::onMeasuringChanged()
{
  ui->particlesLabel->setText(QString::number(m_session->measureList().size()));
}
/*----------------------------------------------------------------------------*/
