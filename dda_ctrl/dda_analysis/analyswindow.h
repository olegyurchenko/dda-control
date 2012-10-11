#ifndef ANALYSWINDOW_H
#define ANALYSWINDOW_H

#include <QMainWindow>

namespace Ui {
class AnalysWindow;
}
/*----------------------------------------------------------------------------*/
class AnalysWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit AnalysWindow(QWidget *parent = 0);
  ~AnalysWindow();
  
private:
  Ui::AnalysWindow *ui;

protected slots:
  void onSessionChanged();
};
/*----------------------------------------------------------------------------*/

#endif // ANALYSWINDOW_H
