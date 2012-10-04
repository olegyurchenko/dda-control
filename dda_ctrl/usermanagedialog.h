#ifndef USERMANAGEDIALOG_H
#define USERMANAGEDIALOG_H

#include <QDialog>
#include <database.h>

namespace Ui {
class UserManageDialog;
}

/*----------------------------------------------------------------------------*/
class UserManageDialog : public QDialog
{
  Q_OBJECT
protected:
  DDAUserList m_userList;
public:
  explicit UserManageDialog(QWidget *parent = 0);
  ~UserManageDialog();
  
private:
  Ui::UserManageDialog *ui;

protected slots:
  void onUserAdd();
  void onUserDel();
  void onUserPassword();
  void onUsersChanged();
  void onSelectedUserChanged(int);
};
/*----------------------------------------------------------------------------*/
bool passwordCheck(QWidget *parent, int userId);
/*----------------------------------------------------------------------------*/

#endif // USERMANAGEDIALOG_H
