#include "selsessionbox.h"
#include "ui_selsessionbox.h"
#include <QDateTime>
#include <database.h>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QPainter>
/*----------------------------------------------------------------------------*/
static QString timeFormat = "yyyy-MM-dd hh:mm";
static QString dateFormat = "yyyy-MM-dd";
/*----------------------------------------------------------------------------*/
enum Columns
{
  IdCol = 0,
  StartCol,
  //SerialCol,
  UserCol,
  ProductCol,
  LotCol,
  GritCol,
  MarkCol,
  ColCount

};
/*----------------------------------------------------------------------------*/
SelSessionBox::SelSessionBox(QWidget *parent) :
  QGroupBox(parent),
  ui(new Ui::SelSessionBox)
{
  ui->setupUi(this);
  timeFormat = QObject::tr("yyyy-MM-dd hh:mm", "Session model date-time format");
  dateFormat = QObject::tr("yyyy-MM-dd", "Session model date format");

  m_manualMode = true;
  m_session = new DDAMeasureSession(this);

  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->dateLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->dateCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->serialLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->serialCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->userLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->userCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->productLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->productCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->gritLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->gritCombo, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->markLabel, SLOT(setEnabled(bool)));
  connect(ui->filterCheck, SIGNAL(toggled(bool)), ui->markCombo, SLOT(setEnabled(bool)));


  connect(database, SIGNAL(userChanged(int)), this, SLOT(onUsersChanged()));


  sessionModel = new SelSessionModel(this);
  ui->sessionView->setModel(sessionModel);
  ui->sessionView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->sessionView->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(ui->sessionView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(onSessionChanged(QModelIndex)));

  sessionModel->setQuery(database->selectSessions(m_filter));
  if(database->isError())
    QMessageBox::critical(this, tr("Database error"), database->message());

  for(int i = 0; i < ColCount - 1; i++)
  {
    QString txt = sessionModel->headerData(i, Qt::Horizontal).toString();
    ui->sessionView->setColumnWidth(i, ui->sessionView->fontMetrics().boundingRect(txt).width() + 20);
  }

  ui->sessionView->setColumnWidth(StartCol - 1, ui->sessionView->fontMetrics().boundingRect(timeFormat).width());
  //ui->sessionView->setColumnWidth(UserCol, ui->sessionView->fontMetrics().boundingRect("000000000").width());
  //ui->sessionView->resizeColumnsToContents();

  onSessionsChanged();
  onUsersChanged();
  onSerialsChanged();
  m_manualMode = false;

  ui->sessionView->selectRow(0);
  onSessionChanged(sessionModel->index(0, 0));
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

  oldIndex = ui->productCombo->currentIndex();
  m_products = database->productList(true);
  ui->productCombo->clear();
  ui->productCombo->addItem("");
  ui->productCombo->addItems(m_products);
  ui->productCombo->setCurrentIndex(oldIndex);

  oldIndex = ui->markCombo->currentIndex();
  m_marks = database->markList(true);
  ui->markCombo->clear();
  ui->markCombo->addItem("");
  ui->markCombo->addItems(m_marks);
  ui->markCombo->setCurrentIndex(oldIndex);

  oldIndex = ui->gritCombo->currentIndex();
  m_grits = database->gritList(true);
  size = m_grits.size();
  ui->gritCombo->clear();
  ui->gritCombo->addItem("");
  for(int i = 0; i < size; i++)
    ui->gritCombo->addItem(QString("%1 %2").arg(database->standardList()[m_grits[i].standard]).arg(database->gritList(m_grits[i].standard)[m_grits[i].gritIndex]));
  ui->gritCombo->setCurrentIndex(oldIndex);

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
    ui->markCombo->setCurrentIndex(0);
    ui->productCombo->setCurrentIndex(0);
    ui->gritCombo->setCurrentIndex(0);

    m_filter.dateSet(false);
    m_filter.userSet(false);
    m_filter.serialSet(false);
    m_filter.markSet(false);
    m_filter.productSet(false);
    m_filter.gritSet(false);
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
void SelSessionBox::onProductChanged(int index)
{
  if(!ui->filterCheck->checkState() || m_manualMode)
    return;
  if(index <= 0)
    m_filter.productSet(false);
  else
    m_filter.setProduct(ui->productCombo->currentText());
  sessionModel->setQuery(database->selectSessions(m_filter));
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onMarkChanged(int index)
{
  if(!ui->filterCheck->checkState() || m_manualMode)
    return;
  if(index <= 0)
    m_filter.markSet(false);
  else
    m_filter.setMark(ui->markCombo->currentText());
  sessionModel->setQuery(database->selectSessions(m_filter));
}
/*----------------------------------------------------------------------------*/
void SelSessionBox::onGritChanged(int index)
{
  if(!ui->filterCheck->checkState() || m_manualMode)
    return;
  if(index <= 0)
    m_filter.gritSet(false);
  else
  {
    m_filter.setGrit(m_grits[index - 1]);
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
  case StartCol:
    if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
      return rec.value(StartCol).toDateTime().toString(timeFormat);
    break;
  default:
    if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
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

    if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
      switch(section + 1)
      {
      case StartCol:
        return tr("Date");
      //case SerialCol:
      //  return tr("Serial");
      case UserCol:
        return tr("User");
      case ProductCol:
        return tr("Product");
      case LotCol:
        return tr("Lot");
      case GritCol:
        return tr("Grit");
      case MarkCol:
        return tr("Mark");
      }
    }
  }
  return QVariant();//QSqlQueryModel::headerData(section, orientation, role);
}
/*----------------------------------------------------------------------------*/
int SelSessionModel :: columnCount(const QModelIndex &) const
{
  return ColCount - 1;
}
/*----------------------------------------------------------------------------*/
