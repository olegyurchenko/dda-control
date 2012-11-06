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
#include <QTime>
#include <session.h>
#include <QMap>
/*----------------------------------------------------------------------------*/
struct DDASerial
{
  int id;
  QString serial;
};
typedef QList<DDASerial> DDASerialList;
/*----------------------------------------------------------------------------*/
struct DDAUser
{
  int id;
  QString name;
};
typedef QList<DDAUser> DDAUserList;
typedef QMap<int, QString> UserMap;
/*----------------------------------------------------------------------------*/
const int UnknownUserId = -1;
const int SuperUserId = 0;
/*----------------------------------------------------------------------------*/
class SessionFilter
{
protected:
  bool m_dateSet;
  bool m_userSet;
  bool m_serialSet;
  QDate m_date;
  int m_user;
  int m_serial;
  int m_limit;
  int m_offset;
public:
  SessionFilter()
  {
    m_dateSet = false;
    m_userSet = false;
    m_serialSet = false;
    m_limit = -1;
    m_offset = -1;
  }

  bool dateSet() const {return m_dateSet;}
  bool userSet() const {return m_userSet;}
  bool serialSet() const {return m_serialSet;}

  void dateSet(bool s) {m_dateSet = s;}
  void userSet(bool s) {m_userSet = s;}
  void serialSet(bool s) {m_serialSet = s;}

  QDate date() const {return m_date;}
  int user() const {return m_user;}
  int serial() const {return m_serial;}
  int limit() const {return m_limit;}
  int offset() const {return m_offset;}

  void setDate(const QDate& d){m_date = d; m_dateSet = true;}
  void setUser(int u) {m_user = u; m_userSet = true;}
  void setSerial(int s) {m_serial = s; m_serialSet = true;}
  void setLimit(int l) {m_limit = l;}
  void setOffset(int o) {m_offset = o;}
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
  int m_session;
  UserMap m_userMap;
  QTime m_startMeasure;
  QString m_connectionName;
  QString m_fileName;

  bool error(const QSqlQuery &q);
  DDASession getSession(const QSqlQuery &q);
  DDAMeasure getMeasure(const QSqlQuery &q);
  int deviceId(const QString& serial);
  int mark(const QString& txt);
  int product(const QString& txt);
  QString deviceSerial(int id);
  void updateCurrentSession();
  bool versionGet(const QString& table, int *dst);
  bool versionSet(const QString& table, int src);
  bool upgrade();
  bool execSql(const QString& fileName);
public:
  DDADatabase(QObject *parent = 0);
  DDADatabase(QObject *parent, const QString& fileName);
  ~DDADatabase();
  bool isError() const {return m_isError;}
  QString message() {return m_message;}
  QStringList standardList();
  QStringList gritList(int standard);
  int meshIndex(int standard, int gritIndex);
  DDAUserList userList(bool forFilter = false);
  QList<QDate> sessionDateList();
  DDASerialList serialList(bool forFilter = false);
  QStringList markList();
  QStringList productList();
  void userAdd(QString name, QString passw = QString());
  void userDel(int id);
  bool checkPassword(int id, QString passw);
  void setPassword(int id, QString passw);
  int sessionAdd(const DDASession& session);
  DDASession lastSession();
  void modifySession(const DDASession& session);
  int addMeasure(const DDAMeasure& measure);
  void modifyMeasure(const DDAMeasure& measure);
  DDAMeasure lastMeasure();
  bool measureSession(int id, DDAMeasureSession *dst);
  QString userName(int id);

  int currentSessionId(){return m_session;}
  QSqlQuery selectSessions(const SessionFilter& filter);
  bool open(const QString& fileName);

public slots:
  void setSerial(const QString&);
  void measure(double strength, double size, int number);
  void onEndOfMeasuring();
  void currentStretch(double);
  void onNoParticle();
protected slots:
  void onUserChanged() {m_userMap.clear();}


signals:
  void dbError(const QString &error);
  void userChanged(int id);
};

extern DDADatabase *database;
/*----------------------------------------------------------------------------*/
#endif /*DATABASE_H_1348809604*/

