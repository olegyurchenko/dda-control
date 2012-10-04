/*----------------------------------------------------------------------------*/
/**
* @pkg session
*/
/**
* Measure session.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 03.10.2012  8:46:33<br>
* @pkgdoc session
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef SESSION_H_1349243193
#define SESSION_H_1349243193
#include <QObject>
#include <QDateTime>
/*----------------------------------------------------------------------------*/
const int InvalidId = -1;
/*----------------------------------------------------------------------------*/
struct DDASession
{
  int id;
  int userId;
  QString deviceSerial;
  QDateTime start;
  QDateTime end;
  QString lot;
  int meshIndex;
  int gostIndex;
  int giftCount;
  QString mark;
  DDASession() {
    id = InvalidId;
    userId = InvalidId;
    start = QDateTime::currentDateTime();
    end = QDateTime::currentDateTime();
    meshIndex = 0;
    gostIndex = 0;
    giftCount = 0;
  }
};
/*----------------------------------------------------------------------------*/
struct DDAMeasure
{
  int id;
  int sessionId;
  double size;
  double strenght;
  int elapsed;
  DDAMeasure() {
    id = InvalidId;
    sessionId = InvalidId;
    size = 0;
    strenght = 0;
    elapsed = 0;
  }
};
typedef QList<DDAMeasure> DDAMeasureList;
/*----------------------------------------------------------------------------*/
class DDAMeasureSession : public QObject
{
  Q_OBJECT
protected:
  DDASession m_session;
  DDAMeasureList m_measureList;
public:
  DDAMeasureSession(QObject *parent = 0);
  const DDASession& session() {return m_session;}
  const DDAMeasureList& measureList() {return m_measureList;}
public slots:
  void setSession(const DDASession& s);
  void setSerial(const QString&);
  void addMeasure(double strength, double size, int number);
  void addMeasure(const DDAMeasure&);
  void setId(int i);
  void setUserId(int i);
  void setStart(const QDateTime&);
  void setEnd(const QDateTime&);
  void setLlot(const QString&);
  void setMeshIndex(int i);
  void setGostIndex(int i);
  void setGiftCount(int i);
  void setMark(const QString&);
  void onEndOfMeasuring();
  void clear();
signals:
  void sessionChanged();
  void measureListChanged();
};
/*----------------------------------------------------------------------------*/
extern DDAMeasureSession *session;
/*----------------------------------------------------------------------------*/
#endif /*SESSION_H_1349243193*/

