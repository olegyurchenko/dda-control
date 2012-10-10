#ifndef SESSIONBOX_H
#define SESSIONBOX_H

#include <QGroupBox>

/*----------------------------------------------------------------------------*/
namespace Ui {
class SessionBox;
}
/*----------------------------------------------------------------------------*/
class DDAMeasureSession;
class SessionBox : public QGroupBox
{
  Q_OBJECT
protected:
  DDAMeasureSession *m_session;
public:
  explicit SessionBox(QWidget *parent = 0);
  ~SessionBox();
  void setSession(DDAMeasureSession *s);
  
private:
  Ui::SessionBox *ui;
protected slots:
  void onSessionChanged();
  void onMeasuringChanged();
};
/*----------------------------------------------------------------------------*/

#endif // SESSIONBOX_H
