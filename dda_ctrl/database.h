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

  bool error(const QSqlQuery &q);
  DDASession getSession(const QSqlQuery &q);
  DDAMeasure getMeasure(const QSqlQuery &q);
  int deviceId(const QString& serial);
  QString deviceSerial(int id);
  void updateCurrentSession();
public:
  DDADatabase(QObject *parent = 0);
  ~DDADatabase();
  bool isError() const {return m_isError;}
  QString message() {return m_message;}
  QStringList standardList();
  QStringList gritList(int standard);
  DDAUserList userList();
  void userAdd(QString name, QString passw = QString());
  void userDel(int id);
  bool checkPassword(int id, QString passw);
  void setPassword(int id, QString passw);
  int sessionAdd(const DDASession& session);
  DDASession lastSession();
  void modifySession(const DDASession& session);
  int addMeasure(const DDAMeasure& measure);
  DDAMeasure lastMeasure();
  bool measureSession(int id, DDAMeasureSession *dst);
  QString userName(int id);

  int currentSessionId(){return m_session;}

public slots:
  void setSerial(const QString&);
  void measure(double strength, double size, int number);
  void onEndOfMeasuring();
  void currentStretch(double);
protected slots:
  void onUserChanged() {m_userMap.clear();}


signals:
  void dbError(const QString &error);
  void userChanged(int id);
};

extern DDADatabase *database;
/*----------------------------------------------------------------------------*/
#endif /*DATABASE_H_1348809604*/

