#include "editdatadialog.h"
#include "ui_editdatadialog.h"
#include <usermanagedialog.h>
#include <QCheckBox>
#include <QApplication>
/*----------------------------------------------------------------------------*/
EditDataDialog::EditDataDialog(QWidget *parent, DDAMeasureSession *session) :
  QDialog(parent),
  ui(new Ui::EditDataDialog)
{
  ui->setupUi(this);
  m_session = session;
  m_measureList = m_session->measureList();

  ui->standardCombo->addItems(database->standardList());
  ui->standardCombo->setCurrentIndex(m_session->session().standard);
  ui->gritCombo->addItems(database->gritList(m_session->session().standard));
  ui->gritCombo->setCurrentIndex(m_session->session().gritIndex);

  ui->productCombo->addItems(database->productList());
  ui->productCombo->setEditText(m_session->session().product);

  ui->lotEdit->setText(m_session->session().lot);

  ui->markCombo->addItems(database->markList());
  ui->markCombo->setEditText(m_session->session().mark);

  EditMeasureModel *model = new EditMeasureModel(this, &m_measureList);
  ui->tableView->setModel(model);
  ui->tableView->setItemDelegate(new EditMeasureDelegate(this, model));

  //ui->tableView->setColumnWidth(0, ui->tableView->fontMetrics().boundingRect(tr("Ignored")).width());
  ui->tableView->setColumnWidth(1, ui->tableView->fontMetrics().boundingRect(tr("Size [um]")).width());
  ui->tableView->setColumnWidth(2, ui->tableView->fontMetrics().boundingRect(tr("Stretch [N]")).width());

}
/*----------------------------------------------------------------------------*/
EditDataDialog::~EditDataDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void EditDataDialog::onAccepted()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  m_session->setStandard(ui->standardCombo->currentIndex());
  m_session->setGritIndex(ui->gritCombo->currentIndex());
  m_session->setProduct(ui->productCombo->currentText());
  m_session->setLot(ui->lotEdit->text());
  m_session->setMark(ui->markCombo->currentText());
  database->modifySession(m_session->session());

  int size = m_measureList.size();
  for(int i = 0; i < size; i++)
  {
    DDAMeasure tmp = m_session->measureList().at(i);
    if(tmp.ignored != m_measureList.at(i).ignored)
      database->modifyMeasure(m_measureList.at(i));
  }

  QApplication::restoreOverrideCursor();
  accept();
}
/*----------------------------------------------------------------------------*/
void EditDataDialog::onStandardChanged(int index)
{
  int cur = ui->gritCombo->currentIndex();
  ui->gritCombo->clear();
  ui->gritCombo->addItems(database->gritList(index));
  ui->gritCombo->setCurrentIndex(cur);
}
/*----------------------------------------------------------------------------*/
// EditMeasureModel
/*----------------------------------------------------------------------------*/
EditMeasureModel :: EditMeasureModel(QObject *parent, DDAMeasureList *measuleList)
  : QAbstractTableModel(parent)
{
  m_measureList = measuleList;
}
/*----------------------------------------------------------------------------*/
QVariant EditMeasureModel :: data(const QModelIndex &index, int role) const
{
  int col = index.column();
  int row = index.row();
  if(row < 0 || row >= m_measureList->size())
    return QVariant();

  DDAMeasure measure = m_measureList->at(row);
  if(role == Qt::DisplayRole)
  {
    switch(col)
    {
    case 0:
      return measure.ignored ? tr("true") : tr("false");
    case 1:
      return QString::number(measure.size, 'f', 1);
    case 2:
      return QString::number(measure.strenght, 'f', 1);
    }
  }
/*
  if(role == Qt::CheckStateRole)
  {
    switch(col)
    {
    case 0:
      return measure.ignored ? Qt::Checked : Qt::Unchecked;

    default:
      return QVariant();
    }
  }
*/
  if(role == Qt::EditRole)
  {
    switch(col)
    {
    case 0:
      return measure.ignored;
    case 1:
      return measure.size;
    case 2:
      return measure.strenght;
    }
  }

  if(role == Qt::TextAlignmentRole)
  {
    switch(col)
    {
    case 0:
      return QVariant((int)(Qt::AlignHCenter | Qt::AlignVCenter));
    case 1:
    case 2:
      return QVariant((int)(Qt::AlignRight | Qt::AlignVCenter));
    }
  }

  if(role == Qt::BackgroundColorRole)
  {
    if(col)
      return measure.ignored ? QColor("lightgrey") : QColor("palegreen");
    else
      return QColor("white");
  }

  if(role == Qt::UserRole)
    return m_row;

  return QVariant();
}
/*----------------------------------------------------------------------------*/
bool EditMeasureModel :: setData(const QModelIndex &idx, const QVariant &value, int role)
{
  int col = idx.column();
  int row = idx.row();
  if(row < 0 || row >= m_measureList->size())
    return false;
  DDAMeasure &measure = (*m_measureList)[row];
  if(role == Qt::UserRole)
    m_row = value.toInt();

  if(role == Qt::EditRole)
  {
    switch(col)
    {
    case 0:
      measure.ignored = value.toBool();
      emit(dataChanged(index(row, 0), index(row, 3)));
      return true;
    case 1:
      measure.size = value.toDouble();
      emit(dataChanged(index(row, 0), index(row, 3)));
      return true;
    case 2:
      measure.strenght = value.toDouble();
      emit(dataChanged(index(row, 0), index(row, 3)));
      return true;
    }
  }
  return QAbstractTableModel::setData(idx, value, role);
}
/*----------------------------------------------------------------------------*/
Qt::ItemFlags EditMeasureModel :: flags(const QModelIndex &index) const
{
  if (!index.isValid())
      return Qt::ItemIsEnabled;

  int col = index.column();
  int row = index.row();
  if(row < 0 || row >= m_measureList->size())
    return Qt::ItemIsEnabled;

  Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  if(!col /*|| !m_measureList->at(row).ignored*/)
    f |= Qt::ItemIsEditable;
  return f;
}
/*----------------------------------------------------------------------------*/
QVariant EditMeasureModel :: headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role == Qt::DisplayRole)
  {
    if(orientation == Qt::Vertical)
    {
      return section + 1;
    }
    else
    {
      switch(section)
      {
      case 0:
        return tr("Ignored");
      case 1:
        return tr("Size [um]");
      case 2:
        return tr("Stretch [N]");
      default:
        return QVariant();
      }
    }
  }

  if(role == Qt::CheckStateRole && orientation == Qt::Vertical)
  {
    return m_measureList->at(section).ignored ? Qt::Checked : Qt::Unchecked;
  }

  return QVariant();
}
/*----------------------------------------------------------------------------*/
int EditMeasureModel :: rowCount(const QModelIndex &) const
{
  return m_measureList->size();
}
/*----------------------------------------------------------------------------*/
int EditMeasureModel :: columnCount(const QModelIndex &) const
{
  return 3;
}
/*----------------------------------------------------------------------------*/
void EditMeasureModel :: update()
{
  emit dataChanged(index(0,0), index(m_measureList->size(), 3));
}
/*----------------------------------------------------------------------------*/
//EditMeasureDelegate
/*----------------------------------------------------------------------------*/
EditMeasureDelegate :: EditMeasureDelegate(QObject *parent, QAbstractTableModel *model)
  : QItemDelegate(parent)
{
  m_model = model;
}
/*----------------------------------------------------------------------------*/
QWidget *EditMeasureDelegate :: createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  int col = index.column();
  switch(col)
  {
  case 0:
  {
    QCheckBox *box = new QCheckBox(parent);
    connect(box, SIGNAL(clicked(bool)), this, SLOT(onIgnoredChanged(bool)));
    return box;
  }
  case 1:
  case 2:
    break;
  }
  return QItemDelegate::createEditor(parent, option, index);
}
/*----------------------------------------------------------------------------*/
void EditMeasureDelegate :: setEditorData(QWidget *editor, const QModelIndex &idx) const
{
  int col = idx.column();
  switch(col)
  {
  case 0:
  {
    QCheckBox *box = qobject_cast<QCheckBox *>(editor);
    box->setChecked(idx.model()->data(idx, Qt::EditRole).toBool());
    m_model->setData(idx, idx.row(), Qt::UserRole);
    return;
  }
  case 1:
  case 2:
    break;
  }
  QItemDelegate::setEditorData(editor, idx);
}
/*----------------------------------------------------------------------------*/
void EditMeasureDelegate :: setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  int col = index.column();
  switch(col)
  {
  case 0:
  {
    QCheckBox *box = qobject_cast<QCheckBox *>(editor);
    model->setData(index, box->checkState(), Qt::EditRole);
    return;
  }
  case 1:
  case 2:
    break;
  }
  QItemDelegate::setModelData(editor, model, index);
}
/*----------------------------------------------------------------------------*/
void EditMeasureDelegate :: updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QItemDelegate::updateEditorGeometry(editor, option, index);
}
/*----------------------------------------------------------------------------*/
void EditMeasureDelegate :: onIgnoredChanged(bool s)
{
  int row = m_model->data(m_model->index(0,0), Qt::UserRole).toInt();
  m_model->setData(m_model->index(row, 0), s);
}
/*----------------------------------------------------------------------------*/
