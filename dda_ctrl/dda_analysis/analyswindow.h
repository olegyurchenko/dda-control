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
  QGridLayout *dataLayout;
  int m_colCount;
  int m_col;
  int m_row;
public:
  explicit AnalysWindow(QWidget *parent = 0);
  ~AnalysWindow();
  
private:
  Ui::AnalysWindow *ui;

protected slots:
  void onSessionChanged();
  void onModelAdded(QObject *m);
  void onEditData();
  void onGenerateReport();
};
/*----------------------------------------------------------------------------*/

#endif // ANALYSWINDOW_H
