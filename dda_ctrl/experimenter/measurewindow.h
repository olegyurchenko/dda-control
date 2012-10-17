#ifndef MEASUREWINDOW_H
#define MEASUREWINDOW_H

#include <QMainWindow>
#include <QTime>

namespace Ui {
class MeasureWindow;
}

class MeasureModel;
class HistogrammPlotter;
class CurvePlotter;
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
  MeasureModel *measureModel;
  HistogrammPlotter *histogrammPlotter;
  CurvePlotter *curvePlotter;
  int intervalCount;

  bool m_newMeasure;
  QTime m_measureTime;

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
  void onMeasure(double strength, double size, int number);
  void onNextCasseteRequest();
  void onEndOfMeasuring();
  void onStartSession();
  void onSessionChanged();
  void onMeasureListChanged();
  void onManageUsers();
  void onSetAdminPassword();
  void onEditCurrentSession();
  void onResumeMeasuring();
  void onSingleStepMode();
  void onOptions();
};
/*----------------------------------------------------------------------------*/

#endif // MEASUREWINDOW_H
