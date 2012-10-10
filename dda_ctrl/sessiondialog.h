#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>
#include <database.h>

namespace Ui {
class SessionDialog;
}

/*----------------------------------------------------------------------------*/
class DDAMeasureSession;
class SessionDialog : public QDialog
{
  Q_OBJECT
protected:
  DDAUserList m_userList;
  DDAMeasureSession *m_session;
public:
  explicit SessionDialog(QWidget *parent = 0, class DDAMeasureSession *session = 0);
  ~SessionDialog();
  
private:
  Ui::SessionDialog *ui;

protected slots:
  void onUserListIndexChanged(int);
  void onManageUsers();
  void onAccepted();
  void onUsersChanged();
  void onStandardChanged(int);
};
/*----------------------------------------------------------------------------*/
#endif // SESSIONDIALOG_H
