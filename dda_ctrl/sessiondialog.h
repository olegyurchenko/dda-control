#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

#include <QDialog>

namespace Ui {
class SessionDialog;
}

class SessionDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit SessionDialog(QWidget *parent = 0);
  ~SessionDialog();
  
private:
  Ui::SessionDialog *ui;
};

#endif // SESSIONDIALOG_H
