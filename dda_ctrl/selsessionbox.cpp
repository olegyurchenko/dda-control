#include "selsessionbox.h"
#include "ui_selsessionbox.h"
#include <QDateTime>
#include <database.h>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QPainter>
/*----------------------------------------------------------------------------*/
const QString timeFormat = QObject::tr("yyyy-MM-dd hh:mm", "Session model date-time format");
const QString dateFormat = QObject::tr("yyyy-MM-dd", "Session model date format");
/*----------------------------------------------------------------------------*/
SelSessionBox::SelSessionBox(QWidget *parent) :
  QGroupBox(parent),
  ui(new Ui::SelSessionBox)
{
  ui->setupUi(this);
  m_manualMode = true;
  m_session = new DDAMeasureSession(this);


  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->dateLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->dateCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->serialLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->serialCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->userLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->userCombo, SLOT(setEnabled(bool)));


  connect(database, SIGNAL(userChanged(int)), this, SLOT(onUsersChanged()));


  sessionModel = new SelSessionModel(this);
  ui->sessionView->setModel(sessionModel);
  ui->sessionView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->sessionView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  sessionModel->setQuery(database->selectSessions(m_filter));
  if(database->isError())
    QMessageBox::critical(this, tr("Database error"), database->message());

  ui->sessionView->setColumnWidth(0, ui->sessionView->fontMetrics().boundingRect(timeFormat).width());
  ui->sessionView->setColumnWidth(1, ui->sessionView->fontMetrics().boundingRect("000000").width());
  ui->sessionView->setColumnWidth(2, ui->sessionView->fontMetrics().boundingRect("000000000").width());
  //ui->sessionView->resizeColumnsToContents();

  onSessionsChanged();
  onUsersChanged();
  onSerialsChanged();
  m_manualMode = false;
}
/*----------------------------------------------------------------------------*/
SelSessionBox::~SelSessionBox()
{
  delete ui;
  delete m_session;
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onUsersChanged()
{
  m_manualMode = true;
  int oldIndex = ui->userCombo->currentIndex();
  m_users = database->userList(true);
  ui->userCombo->clear();
  ui->userCombo->addItem("");
  int size = m_users.size();
  for(int i = 0; i < size; i++)
  {
    ui->userCombo->addItem(QIcon(":/user-identity.png"), m_users[i].name);
  }
  ui->userCombo->setCurrentIndex(oldIndex);
  m_manualMode = false;
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onSerialsChanged()
{
  m_manualMode = true;
  int oldIndex = ui->serialCombo->currentIndex();
  m_serials = database->serialList(true);
  ui->serialCombo->clear();
  ui->serialCombo->addItem("");
  int size = m_serials.size();
  for(int i = 0; i < size; i++)
  {
    ui->serialCombo->addItem(QIcon(":/dda.png"), m_serials[i].serial);
  }
  ui->serialCombo->setCurrentIndex(oldIndex);
  m_manualMode = false;
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onSessionsChanged()
{
  m_manualMode = true;
  int oldIndex = ui->dateCombo->currentIndex();
  m_dates = database->sessionDateList();
  ui->dateCombo->clear();
  ui->dateCombo->addItem("");
  int size = m_dates.size();
  for(int i = 0; i < size; i++)
  {
    ui->dateCombo->addItem(QIcon(":/view-calendar-day.png"), m_dates[i].toString(dateFormat));
  }
  ui->dateCombo->setCurrentIndex(oldIndex);
  m_manualMode = false;
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onSessionChanged(QModelIndex idx)
{
  QSqlQuery q = sessionModel->query();
  q.seek(idx.row());
  int id = q.value(0).toInt();
  database->measureSession(id, m_session);
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onFilterChanged(bool check)
{
  m_manualMode = true;
  if(!check)
  {
    ui->dateCombo->setCurrentIndex(0);
    ui->userCombo->setCurrentIndex(0);
    ui->serialCombo->setCurrentIndex(0);
    m_filter.dateSet(false);
    m_filter.userSet(false);
    m_filter.serialSet(false);
  }
  sessionModel->setQuery(database->selectSessions(m_filter));
  m_manualMode = false;
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onDateChanged(int index)
{
  if(!ui->filterCheck->checkState() || m_manualMode)
    return;
  if(index <= 0)
    m_filter.dateSet(false);
  else
    m_filter.setDate(m_dates[index - 1]);
  sessionModel->setQuery(database->selectSessions(m_filter));
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onUserChanged(int index)
{
  if(!ui->filterCheck->checkState() || m_manualMode)
    return;
  if(index <= 0)
    m_filter.userSet(false);
  else
  {
    m_filter.setUser(m_users[index - 1].id);
  }
  sessionModel->setQuery(database->selectSessions(m_filter));
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onSerialChanged(int index)
{
  if(!ui->filterCheck->checkState() || m_manualMode)
    return;
  if(index <= 0)
    m_filter.serialSet(false);
  else
  {
    m_filter.setSerial(m_serials[index - 1].id);
  }
  sessionModel->setQuery(database->selectSessions(m_filter));
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onRefresh()
{
  onUsersChanged();
  onSerialsChanged();
  onSessionsChanged();
  sessionModel->setQuery(database->selectSessions(m_filter));
}
/*----------------------------------------------------------------------------*/
//SelSessionModel
/*----------------------------------------------------------------------------*/
#define ID_COL 0
#define START_COL 1
#define SERIAL_COL 2
#define USER_COL 3
#define LOT_COL 4
/*----------------------------------------------------------------------------*/
SelSessionModel :: SelSessionModel(QObject *parent)
  : QSqlQueryModel(parent)
{
}
/*----------------------------------------------------------------------------*/
QVariant SelSessionModel :: data(const QModelIndex &idx, int role) const
{
  int col = idx.column();
  query().seek(idx.row());
  QSqlRecord rec = query().record();
  switch(col + 1)
  {
  case START_COL:
    if(role == Qt::DisplayRole)
      return rec.value(START_COL).toDateTime().toString(timeFormat);
    break;
  case SERIAL_COL:
  case USER_COL:
  case LOT_COL:
    if(role == Qt::DisplayRole)
      return rec.value(col + 1);
    break;
  }
  return QVariant();
}
/*----------------------------------------------------------------------------*/
QVariant SelSessionModel :: headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Vertical)
  {
    if(role == Qt::DisplayRole)
      return QSqlQueryModel::data(index(section, 0), role);
  }
  else
  {
    if(Qt::SizeHintRole)
    {
    }

    return QSqlQueryModel::headerData(section + 1, orientation, role);
  }
  return QSqlQueryModel::headerData(section, orientation, role);
}
/*----------------------------------------------------------------------------*/
int SelSessionModel :: columnCount(const QModelIndex &) const
{
  return 4;
}
/*----------------------------------------------------------------------------*/
