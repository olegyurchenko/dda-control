#include "selsessionbox.h"
#include "ui_selsessionbox.h"
#include <QDateTime>
#include <database.h>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QPainter>
/*----------------------------------------------------------------------------*/
SelSessionBox::SelSessionBox(QWidget *parent) :
  QGroupBox(parent),
  ui(new Ui::SelSessionBox)
{
  ui->setupUi(this);
  m_session = new DDAMeasureSession(this);

  ui->dateEdit->setDate(QDate::currentDate());

  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->dateCheck, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->dateEdit, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->serialLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->serialCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->userLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->userCombo, SLOT(setEnabled(bool)));


  connect(database, SIGNAL(userChanged(int)), this, SLOT(onUsersChanged()));

  onUsersChanged();

  sessionModel = new SelSessionModel(this);
  ui->sessionView->setModel(sessionModel);
  ui->sessionView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->sessionView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  sessionModel->setQuery(database->selectSessions(m_filter));
  if(database->isError())
    QMessageBox::critical(this, tr("Database error"), database->message());

  ui->sessionView->setColumnWidth(0, ui->sessionView->fontMetrics().boundingRect(tr("yyyy/MM/dd hh:mm", "Session model date format")).width());
  ui->sessionView->setColumnWidth(1, ui->sessionView->fontMetrics().boundingRect("000000").width());
  ui->sessionView->setColumnWidth(2, ui->sessionView->fontMetrics().boundingRect("000000000").width());
  //ui->sessionView->resizeColumnsToContents();
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
  m_users = database->userList(true);
  ui->userCombo->clear();
  ui->userCombo->addItem("");
  int size = m_users.size();
  for(int i = 0; i < size; i++)
  {
    ui->userCombo->addItem(QIcon(":/user-identity.png"), m_users[i].name);
  }
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onSessionChanged(QModelIndex)
{
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onFilterChanged(bool)
{
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onDateChechChanged(bool)
{
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onDateChanged(QDate)
{
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onUserChanged(int)
{
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onSerialChanged(int)
{
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
      return rec.value(START_COL).toDateTime().toString(tr("yyyy/MM/dd hh:mm", "Session model date format"));
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
