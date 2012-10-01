#ifndef EDITPROFILEDIALOG_H
#define EDITPROFILEDIALOG_H

#include <QDialog>
#include <configuration.h>

namespace Ui {
class EditProfileDialog;
}

/*----------------------------------------------------------------------------*/
class EditProfileDialog : public QDialog
{
  Q_OBJECT
protected:
  DDAProfile m_profile;
public:
  explicit EditProfileDialog(QWidget *parent = 0);
  ~EditProfileDialog();
  
private:
  Ui::EditProfileDialog *ui;

public:
  DDAProfile profile() {return m_profile;}
  void setProfile(const DDAProfile& p);

protected slots:
  void onAccepted();
};
/*----------------------------------------------------------------------------*/

#endif // EDITPROFILEDIALOG_H
