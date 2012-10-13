#ifndef EDITDATADIALOG_H
#define EDITDATADIALOG_H
#include <QDialog>
#include <session.h>
#include <database.h>
#include <QAbstractTableModel>
#include <QItemDelegate>
/*----------------------------------------------------------------------------*/
namespace Ui {
class EditDataDialog;
}
/*----------------------------------------------------------------------------*/
class EditDataDialog : public QDialog
{
  Q_OBJECT
protected:
  DDAMeasureSession *m_session;
  DDAMeasureList m_measureList;
public:
  explicit EditDataDialog(QWidget *parent, DDAMeasureSession *session);
  ~EditDataDialog();
  
private:
  Ui::EditDataDialog *ui;
protected slots:
  void onAccepted();
  void onStandardChanged(int);
};
/*----------------------------------------------------------------------------*/
class EditMeasureModel : public QAbstractTableModel
{
  Q_OBJECT
protected:
  DDAMeasureList *m_measureList;
  int m_row;
public:
  EditMeasureModel(QObject *parent, DDAMeasureList *measuleList);
  QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &) const;
  int columnCount(const QModelIndex &) const;
public slots:
  void update();
};
/*----------------------------------------------------------------------------*/
class EditMeasureDelegate : public QItemDelegate
{
  Q_OBJECT
protected:
  QAbstractTableModel *m_model;
public:
 EditMeasureDelegate(QObject *parent, QAbstractTableModel *model);
 QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
 void setEditorData(QWidget *editor, const QModelIndex &index) const;
 void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
 void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected slots:
 void onIgnoredChanged(bool);
};
/*----------------------------------------------------------------------------*/
#endif // EDITDATADIALOG_H
