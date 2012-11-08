#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

/*----------------------------------------------------------------------------*/
namespace Ui {
class MainWindow;
}
/*----------------------------------------------------------------------------*/
class MessageFile;
class MainWindow : public QMainWindow
{
  Q_OBJECT

protected:
  QString m_fileName;
  MessageFile *m_messageFile;
  QStringList m_langList;
  QStringList m_sourceList;
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private:
  Ui::MainWindow *ui;

protected slots:
  void onFileNew();
  void onFileOpen();
  void onFileSave();
  void onFileSaveAs();
  void onFileExit();
  void onLangChanged(int);
  void onTranslateChanged(QString);
  void onSourceChanged(int);
  void onSearchTextChanged(QString);
  void updateMessages();
  void onSourceListDblClick();

};
/*----------------------------------------------------------------------------*/

#endif // MAINWINDOW_H
