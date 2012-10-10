#ifndef SELSESSIONBOX_H
#define SELSESSIONBOX_H

#include <QGroupBox>
#include <session.h>
#include <database.h>
#include <QSqlQueryModel>

namespace Ui {
class SelSessionBox;
}
/*----------------------------------------------------------------------------*/
class SelSessionModel : public QSqlQueryModel
{
  Q_OBJECT
public:
  SelSessionModel(QObject *parent = 0);
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int columnCount(const QModelIndex &) const;
};
/*----------------------------------------------------------------------------*/
class SelSessionBox : public QGroupBox
{
  Q_OBJECT
protected:
  DDAMeasureSession *m_session;
  DDAUserList m_users;
  DDASerialList m_serials;
  QList<QDate> m_dates;
  SelSessionModel *sessionModel;
  SessionFilter m_filter;
  bool m_manualMode;
public:
  explicit SelSessionBox(QWidget *parent = 0);
  ~SelSessionBox();
  DDAMeasureSession *session() {return m_session;}

private:
  Ui::SelSessionBox *ui;

protected slots:
  void onUsersChanged();
  void onSerialsChanged();
  void onSessionsChanged();
  void onSessionChanged(QModelIndex);
  void onFilterChanged(bool);
  void onDateChanged(int);
  void onUserChanged(int);
  void onSerialChanged(int);
  void onRefresh();
};
/*----------------------------------------------------------------------------*/

#endif // SELSESSIONBOX_H
