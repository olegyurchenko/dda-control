#ifndef ANALYSWINDOW_H
#define ANALYSWINDOW_H

#include <QMainWindow>

namespace Ui {
class AnalysWindow;
}
/*----------------------------------------------------------------------------*/
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QGridLayout;
class QStackedLayout;
class AnalysWindow : public QMainWindow
{
  Q_OBJECT
protected:
  int m_columnCount;
  QVBoxLayout *dataLayout;
public:
  explicit AnalysWindow(QWidget *parent = 0);
  ~AnalysWindow();
  
private:
  Ui::AnalysWindow *ui;

protected slots:
  void onSessionChanged();
  void onModelAdded(QObject *m);
};
/*----------------------------------------------------------------------------*/

#endif // ANALYSWINDOW_H
