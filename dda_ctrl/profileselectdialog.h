#ifndef PROFILESELECTDIALOG_H
#define PROFILESELECTDIALOG_H

#include <QDialog>

namespace Ui {
class ProfileSelectDialog;
}
/*----------------------------------------------------------------------------*/
class ProfileSelectDialog : public QDialog
{
  Q_OBJECT
protected:
  int m_selectedProfile;
public:
  explicit ProfileSelectDialog(QWidget *parent = 0);
  ~ProfileSelectDialog();
  int selectedProfile() {return m_selectedProfile;}

private:
  Ui::ProfileSelectDialog *ui;
protected slots:
  void onProfileChanged(int);
  void onEditBtnClicked();
  void onNewBtnClicked();
  void onDebBtnClicked();

};
/*----------------------------------------------------------------------------*/

#endif // PROFILESELECTDIALOG_H
