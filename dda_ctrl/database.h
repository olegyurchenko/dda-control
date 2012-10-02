/*----------------------------------------------------------------------------*/
/**
* @pkg database
*/
/**
* DDA database.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 28.09.2012  8:20:04<br>
* @pkgdoc database
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DATABASE_H_1348809604
#define DATABASE_H_1348809604
/*----------------------------------------------------------------------------*/
#include <QObject>
#include <QList>
#include <QStringList>
#include <QDateTime>

/*----------------------------------------------------------------------------*/
struct DDAUser
{
  int id;
  QString name;
};
typedef QList<DDAUser> DDAUserList;
/*----------------------------------------------------------------------------*/
const int UnknownUserId = -1;
const int SuperUserId = 0;
const int InvalidId = -1;
/*----------------------------------------------------------------------------*/
struct DDASession
{
  int id;
  int userId;
  int deviceId;
  QDateTime start;
  QDateTime end;
  QString lot;
  int meshIndex;
  int gostIndex;
  QString mark;
  DDASession() {
    id = InvalidId;
    userId = InvalidId;
    deviceId = InvalidId;
    start = QDateTime::currentDateTime();
    end = QDateTime::currentDateTime();
    meshIndex = 0;
    gostIndex = 0;
  }
};
/*----------------------------------------------------------------------------*/
struct DDAMeasure
{
  enum MeasureFlags
  {
    sizeFlag = 1,
    strenghtFlag = 2
  };

  int id;
  int sessionId;
  double size;
  double strenght;
  int elapsed;
  unsigned mask;
  DDAMeasure() {
    id = InvalidId;
    sessionId = InvalidId;
    size = 0;
    strenght = 0;
    elapsed = 0;
    mask = 0;
  }
};
typedef QList<DDAMeasure> DDAMeasureList;
/*----------------------------------------------------------------------------*/
struct DDAMeasureSession
{
  DDASession session;
  DDAMeasureList measureList;
};
/*----------------------------------------------------------------------------*/
class QSqlQuery;
class DDADatabase : public QObject
{
  Q_OBJECT
public:
protected:
  bool m_isError;
  QString m_message;
  QString m_serial;
  DDAMeasureSession m_measureSession;

  bool error(const QSqlQuery &q);
  DDASession getSession(const QSqlQuery &q);
  DDAMeasure getMeasure(const QSqlQuery &q);

public:
  DDADatabase(QObject *parent = 0);
  ~DDADatabase();
  bool isError() const {return m_isError;}
  QString message() {return m_message;}
  QStringList meshList();
  QStringList gostList();
  DDAUserList userList();
  void userAdd(QString name, QString passw = QString());
  void userDel(int id);
  bool checkPassword(int id, QString passw);
  void setPassword(int id, QString passw);
  int deviceId(const QString& serial);
  int sessionAdd(const DDASession& session);
  DDASession lastSession();
  void modifySession(const DDASession& session);
  int addMeasure(const DDAMeasure& measure);
  DDAMeasure lastMeasure();
  DDAMeasureSession measureSession(int id);

  DDAMeasureSession& session() {return m_measureSession;}
  void setSession(const DDAMeasureSession& s) {m_measureSession = s;}

public slots:
  void setSerial(const QString&);
  void measure(double strength, int number, int nextCell);
  void giftSize(double size);
  void currentStretch(double);


signals:
  void dbError(const QString &error);
  void sessionChanged(int id);
};

extern DDADatabase *database;
/*----------------------------------------------------------------------------*/
#endif /*DATABASE_H_1348809604*/

