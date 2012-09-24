#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QPrinter;
/*----------------------------------------------------------------------------*/
class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private slots:
  void onActionPrint();

  void onActionPrintPreview();

  void onActionExportToPdf();

  void onActionExit();

private:
  Ui::MainWindow *ui;
  QPrinter *m_printer;
};
/*----------------------------------------------------------------------------*/

#endif // MAINWINDOW_H
