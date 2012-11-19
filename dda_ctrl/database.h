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
struct DDAGrit
{
  int standard;
  int gritIndex;
};
typedef QList<DDAGrit> DDAGritList;
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
  bool m_markSet;
  bool m_productSet;
  bool m_gritSet;

  QDate m_date;
  int m_user;
  int m_serial;
  int m_limit;
  int m_offset;
  QString m_mark;
  QString m_product;
  DDAGrit m_grit;
public:
  SessionFilter()
  {
    m_dateSet = false;
    m_userSet = false;
    m_serialSet = false;
    m_markSet = false;
    m_productSet = false;
    m_gritSet = false;
    m_limit = -1;
    m_offset = -1;
  }

  bool isDateSet() const {return m_dateSet;}
  bool isUserSet() const {return m_userSet;}
  bool isSerialSet() const {return m_serialSet;}
  bool isMarkSet() const {return m_markSet;}
  bool isProductSet() const {return m_productSet;}
  bool isGritSet() const {return m_gritSet;}

  void dateSet(bool s) {m_dateSet = s;}
  void userSet(bool s) {m_userSet = s;}
  void serialSet(bool s) {m_serialSet = s;}
  void markSet(bool s) {m_markSet = s;}
  void productSet(bool s) {m_productSet = s;}
  void gritSet(bool s) {m_gritSet = s;}

  QDate date() const {return m_date;}
  int user() const {return m_user;}
  int serial() const {return m_serial;}
  int limit() const {return m_limit;}
  int offset() const {return m_offset;}
  QString mark() const {return m_mark;}
  QString product() const {return m_product;}
  int standard() const {return m_grit.standard;}
  int gritIndex() const {return m_grit.gritIndex;}

  void setDate(const QDate& d){m_date = d; m_dateSet = true;}
  void setUser(int u) {m_user = u; m_userSet = true;}
  void setSerial(int s) {m_serial = s; m_serialSet = true;}
  void setLimit(int l) {m_limit = l;}
  void setOffset(int o) {m_offset = o;}
  void setMark(const QString& m) {m_mark =m; m_markSet = true;}
  void setProduct(const QString& p) {m_product = p; m_productSet = true;}
  void setGrit(const DDAGrit& g) {m_grit = g; m_gritSet = true;}
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
  QStringList markList(bool forFilter = false);
  QStringList productList(bool forFilter = false);
  DDAGritList gritList(bool forFilter = false);
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

