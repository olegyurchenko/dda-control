#ifndef MEASUREWINDOW_H
#define MEASUREWINDOW_H

#include <QMainWindow>

namespace Ui {
class MeasureWindow;
}

/*----------------------------------------------------------------------------*/
class MeasureWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MeasureWindow(QWidget *parent = 0);
  ~MeasureWindow();
  
private:
  Ui::MeasureWindow *ui;
  bool m_created;

protected slots:
  void onProfileChanged(int);
  void onManageProfile();
  void onEditProfile();
  void onDatabaseError();
  void onStatusChanged(int s);
  void onCmdSendOk();
  void onCmdSendError();
  void onSerialReceived(const QString&);
  void onCurrentStretch(double);
  void onNoParticle();
  void onMeasure(double strength, int number, int nextCell);
  void onGiftSize(double size);
  void onNextCasseteRequest();
  void onEndOfMeasuring();
  void onStartSession();
};
/*----------------------------------------------------------------------------*/

#endif // MEASUREWINDOW_H
