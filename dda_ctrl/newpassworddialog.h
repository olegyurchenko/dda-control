#ifndef NEWPASSWORDDIALOG_H
#define NEWPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class NewPasswordDialog;
}

/*----------------------------------------------------------------------------*/
class NewPasswordDialog : public QDialog
{
  Q_OBJECT
  
  QString m_password;

public:
  explicit NewPasswordDialog(QWidget *parent = 0);
  ~NewPasswordDialog();
  QString password() {return m_password;}
  
private:
  Ui::NewPasswordDialog *ui;
protected slots:
  void onPasswordChanged();
};
/*----------------------------------------------------------------------------*/

#endif // NEWPASSWORDDIALOG_H
