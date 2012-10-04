#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>
#include <database.h>

namespace Ui {
class SessionDialog;
}

/*----------------------------------------------------------------------------*/
class SessionDialog : public QDialog
{
  Q_OBJECT
protected:
  DDAUserList m_userList;
public:
  explicit SessionDialog(QWidget *parent = 0);
  ~SessionDialog();
  
private:
  Ui::SessionDialog *ui;

protected slots:
  void onUserListIndexChanged(int);
  void onManageUsers();
  void onAccepted();
  void onUsersChanged();

};
/*----------------------------------------------------------------------------*/
#endif // SESSIONDIALOG_H
