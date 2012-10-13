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
#include "database.h"
#include <configuration.h>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QRegExp>
#include <QVariant>
#include <QCryptographicHash>
#include <QDateTime>
/*----------------------------------------------------------------------------*/
DDADatabase *database = NULL;
/*----------------------------------------------------------------------------*/
#define CONNECTION_NAME "DDA"
/*----------------------------------------------------------------------------*/

DDADatabase :: DDADatabase(QObject *parent)
  : QObject(parent)
{
  m_session = -1;
  QString dbFileName = config->settings().databaseFileName;

  QSqlDatabase m_database = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
  m_database.setDatabaseName(dbFileName);
  if(!m_database.open())
  {
    m_isError = true;
    m_message = m_database.lastError().text();
    return;
  }

  QFile f(":/sql/create_database.sql");
  if(!f.open(QIODevice::ReadOnly))
  {
    m_isError = true;
    m_message = tr("Error open %1").arg("create_database.sql");
    return;
  }
  QString sql = f.readAll();
  QSqlQuery q(m_database);
  //    if(!q.prepare(sql) || !q.execBatch())

  sql.remove('\r');
  sql.replace('\n', " ");
  QStringList lst = sql.split(';', QString::SkipEmptyParts);
  QString s;
  QRegExp r("[A-Za-z]");

  foreach(s, lst)
  {
    if(s.indexOf(r) == -1)
      continue;

    if(!q.exec(s))
    {
      error(q);
      return;
    }
  }

  if(!q.exec("select count(*) from standards"))
  {
    error(q);
    return;
  }

  if(!q.first() || !q.value(0).toInt())
  {
    f.close();
    f.setFileName(":/sql/standards.txt");
    if(!f.open(QIODevice::ReadOnly))
    {
      m_isError = true;
      m_message = tr("Error open %1").arg("mesh.txt");
      return;
    }
    QString txt = f.readAll();
    txt.remove('\r');
    QStringList lst = txt.split('\n', QString::SkipEmptyParts);
    int size = lst.size();
    for(int i = 0; i < size; i++)
    {
      q.prepare("insert into standards(id, txt) values(?, ?)");
      q.addBindValue(i);
      q.addBindValue(lst[i]);
      if(!q.exec())
      {
        error(q);
        return;
      }
    }
  }

  if(!q.exec("select count(*) from gritSizes"))
  {
    error(q);
    return;
  }

  if(!q.first() || !q.value(0).toInt())
  {
    f.close();
    f.setFileName(":/sql/mesh.txt");
    if(!f.open(QIODevice::ReadOnly))
    {
      m_isError = true;
      m_message = tr("Error open %1").arg("mesh.txt");
      return;
    }
    QString txt = f.readAll();
    txt.remove('\r');
    QStringList lst = txt.split('\n', QString::SkipEmptyParts);
    int size = lst.size();
    for(int i = 0; i < size; i++)
    {
      q.prepare("insert into gritSizes(standard, gritIndex, txt) values(0, ?, ?)");
      q.addBindValue(i);
      q.addBindValue(lst[i]);
      if(!q.exec())
      {
        error(q);
        return;
      }
    }

    f.close();
    f.setFileName(":/sql/gost.txt");
    if(!f.open(QIODevice::ReadOnly))
    {
      m_isError = true;
      m_message = tr("Error open %1").arg("gost.txt");
      return;
    }
    txt = f.readAll();
    txt.remove('\r');
    lst = txt.split('\n', QString::SkipEmptyParts);
    size = lst.size();
    for(int i = 0; i < size; i++)
    {
      q.prepare("insert into gritSizes(standard, gritIndex, txt) values(1, ?, ?)");
      q.addBindValue(i);
      q.addBindValue(lst[i]);
      if(!q.exec())
      {
        error(q);
        return;
      }
    }

    f.close();
    f.setFileName(":/sql/dstu.txt");
    if(!f.open(QIODevice::ReadOnly))
    {
      m_isError = true;
      m_message = tr("Error open %1").arg("dstu.txt");
      return;
    }
    txt = f.readAll();
    txt.remove('\r');
    lst = txt.split('\n', QString::SkipEmptyParts);
    size = lst.size();
    for(int i = 0; i < size; i++)
    {
      q.prepare("insert into gritSizes(standard, gritIndex, txt) values(2, ?, ?)");
      q.addBindValue(i);
      q.addBindValue(lst[i]);
      if(!q.exec())
      {
        error(q);
        return;
      }
    }
  }

  q.prepare("select id from users where id = ?");
  q.addBindValue(UnknownUserId);
  if(!q.exec())
  {
    error(q);
    return;
  }

  if(!q.first())
  {
    q.prepare("insert into users(id, active, name, used) values(?, 1, ?, 1)");
    q.addBindValue(UnknownUserId);
    q.addBindValue(tr("Undefined user"));
    if(!q.exec())
    {
      error(q);
      return;
    }
  }

  q.prepare("select id from users where id = ?");
  q.addBindValue(SuperUserId);
  if(!q.exec())
  {
    error(q);
    return;
  }

  if(!q.first())
  {
    q.prepare("insert into users(id, active, name, used) values(?, 0, ?, 1)");
    q.addBindValue(SuperUserId);
    q.addBindValue("root");
    if(!q.exec())
    {
      error(q);
      return;
    }
    setPassword(SuperUserId, "");
    if(!checkPassword(SuperUserId, ""))
    {
      m_message = tr("Password check error");
      m_isError = true;
      emit dbError(m_message);
      return;
    }
  }

  connect(this, SIGNAL(userChanged(int)), this, SLOT(onUserChanged()));
  m_isError = false;
}
/*----------------------------------------------------------------------------*/
DDADatabase :: ~DDADatabase()
{
  QSqlDatabase::removeDatabase(CONNECTION_NAME);
}
/*----------------------------------------------------------------------------*/
bool DDADatabase :: error(const QSqlQuery &q)
{
  if(q.lastError().isValid())
  {
    m_message = QString("%1:\n'%2'").arg(q.lastError().text()).arg(q.lastQuery());
    m_isError = true;
    emit dbError(m_message);
    return true;
  }
  m_isError = false;
  return false;
}
/*----------------------------------------------------------------------------*/
QStringList DDADatabase :: standardList()
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.exec("select txt from standards order by id"))
  {
    error(q);
    return QStringList();
  }
  QStringList lst;

  bool eof = !q.first();
  while(!eof)
  {
    lst.append(q.value(0).toString());
    eof = !q.next();
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
QStringList DDADatabase :: gritList(int standard)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare("select txt from gritSizes where standard = ? order by gritIndex");
  q.addBindValue(standard);
  if(!q.exec())
  {
    error(q);
    return QStringList();
  }

  QStringList lst;

  bool eof = !q.first();
  while(!eof)
  {
    lst.append(q.value(0).toString());
    eof = !q.next();
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
DDAUserList DDADatabase :: userList(bool forFilter)
{
  DDAUserList lst;
  if(!m_userMap.empty() && !forFilter)
  {
    UserMap::Iterator end = m_userMap.end();
    for(UserMap::Iterator it = m_userMap.begin(); it != end; ++it)
    {
      DDAUser user;
      user.id = it.key();
      user.name = it.value();
      lst.append(user);
    }
    return lst;
  }

  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(forFilter)
  {
    if(!q.exec("select u.id, u.name from users u, sessions s where u.id = s.user group by u.id, u.name order by u.id"))
    {
      error(q);
      return DDAUserList();
    }
  }
  else
  {
    if(!q.exec("select id, name from users where active <> 0 order by id"))
    {
      error(q);
      return DDAUserList();
    }
  }

  bool eof = !q.first();
  while(!eof)
  {
    DDAUser user;
    user.id = q.value(0).toInt();
    user.name = q.value(1).toString();
    lst.append(user);
    if(!forFilter)
      m_userMap[user.id] = user.name;
    eof = !q.next();
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: userAdd(QString name, QString passw)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  int id = 0;
  if(!q.exec("select max(id) from users"))
  {
    error(q);
    return;
  }
  if(q.first())
    id = q.value(0).toInt();

  ++ id;
  if(!q.prepare("insert into users(id, active, name, used) values(?, 1, ?, 0)"))
  {
    error(q);
    return;
  }

  q.addBindValue(id);
  q.addBindValue(name);
  if(!q.exec())
    error(q);
  setPassword(id, passw);
  emit userChanged(id);
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: userDel(int id)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  int used = 0;
  q.prepare("select used from users where id = ?");
  q.addBindValue(id);
  if(!q.exec())
  {
    error(q);
    return;
  }
  if(q.first())
    used = q.value(0).toInt();
  if(used)
    q.prepare("update users set active = 0 where id = ?");
  else
    q.prepare("delete from users where id = ?");

  q.addBindValue(id);
  if(!q.exec())
    error(q);
  emit userChanged(id);
}
/*----------------------------------------------------------------------------*/
bool DDADatabase :: checkPassword(int id, QString passw)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.prepare("select pswd from users where id = ?"))
  {
    error(q);
    return false;
  }
  q.addBindValue(id);
  if(!q.exec())
    error(q);
  if(!q.first())
  {
    m_isError = true;
    m_message = tr("User id=%1 not found").arg(id);
    emit dbError(m_message);
    return false;
  }
  QString shash, salt;
  shash = q.value(0).toString();
  if(shash.isEmpty())
    return false;
  salt = shash.mid(32);
  shash.remove(32, shash.size() - 32);
  QCryptographicHash hash(QCryptographicHash::Md5);
  hash.addData(salt.toAscii());
  hash.addData(passw.toUtf8());
  QString dhash = hash.result().toHex();
  return shash == dhash;
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: setPassword(int id, QString passw)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.prepare("update users set pswd = ? where id = ?"))
  {
    error(q);
    return;
  }

  qsrand(QDateTime::currentDateTime().toTime_t());
  QString salt;
  for(int i = 0; i < 16; i++)
  {
    salt += QString::number((unsigned)qrand() % 256, 16);
  }

  QCryptographicHash hash(QCryptographicHash::Md5);
  hash.addData(salt.toAscii());
  hash.addData(passw.toUtf8());

  q.addBindValue(QString(hash.result().toHex()) + salt);
  q.addBindValue(id);
  if(!q.exec())
    error(q);
  emit userChanged(id);
}
/*----------------------------------------------------------------------------*/
int DDADatabase :: deviceId(const QString& serial)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare("select id from devices where serial = ?");
  q.addBindValue(serial);
  if(!q.exec())
  {
    error(q);
    return -1;
  }
  if(q.first())
    return q.value(0).toInt();
  if(!q.exec("select max(id) from devices"))
  {
    error(q);
    return -1;
  }
  int id = 0;
  if(q.first())
    id = q.value(0).toInt();
  ++id;
  q.prepare("insert into devices(id, serial) values(?, ?)");
  q.addBindValue(id);
  q.addBindValue(serial);
  if(!q.exec())
  {
    error(q);
    return -1;
  }
  return id;
}
/*----------------------------------------------------------------------------*/
QString DDADatabase :: deviceSerial(int id)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare("select serial from devices where id = ?");
  q.addBindValue(id);
  if(!q.exec())
  {
    error(q);
    return QString();
  }

  if(!q.first())
    return QString();

  return q.value(0).toString();
}
/*----------------------------------------------------------------------------*/
DDASerialList DDADatabase :: serialList(bool forFilter)
{
  DDASerialList lst;
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(forFilter)
  {
    if(!q.exec("select d.id, d.serial from devices d, sessions s where d.id = s.device group by d.id, d.serial order by d.id"))
    {
      error(q);
      return DDASerialList();
    }
  }
  else
  {
    if(!q.exec("select id, serial from devices order by id"))
    {
      error(q);
      return DDASerialList();
    }
  }

  bool eof = !q.first();
  while(!eof)
  {
    DDASerial s;
    s.id = q.value(0).toInt();
    s.serial = q.value(1).toString();
    lst.append(s);
    eof = !q.next();
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
int DDADatabase :: sessionAdd(const DDASession& session)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  int devId = deviceId(session.deviceSerial);

  if(!q.exec("select max(id) from sessions"))
  {
    error(q);
    return -1;
  }
  int id = 0;
  if(q.first())
    id = q.value(0).toInt();
  ++id;
  q.prepare(
        "insert into sessions(id, user,device,start,end, lot,standard, grit, mark)\n"
        "values(?,?,?,?,?,?,?,?,?)"
            );
  q.addBindValue(id);
  q.addBindValue(session.userId);
  q.addBindValue(devId);
  q.addBindValue(session.start);
  q.addBindValue(session.end);
  q.addBindValue(session.lot);
  q.addBindValue(session.standard);
  q.addBindValue(session.gritIndex);
  q.addBindValue(session.mark);
  if(!q.exec())
  {
    error(q);
    return -1;
  }
  m_session = id;
  q.prepare("update users set used = 1 where id = ?");
  q.addBindValue(session.userId);
  if(!q.exec())
    error(q);
  return id;
}
/*----------------------------------------------------------------------------*/
DDASession DDADatabase :: getSession(const QSqlQuery &q)
{
  DDASession session;
  session.id = q.value(0).toInt();
  session.userId = q.value(1).toInt();
  session.deviceSerial = deviceSerial(q.value(2).toInt());
  session.start = q.value(3).toDateTime();
  session.end = q.value(4).toDateTime();
  session.lot = q.value(5).toString();
  session.standard = q.value(6).toInt();
  session.gritIndex = q.value(7).toInt();
  session.mark = q.value(8).toString();

  return session;
}
/*----------------------------------------------------------------------------*/
DDASession DDADatabase :: lastSession()
{
  DDASession session;
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare(
        "select id, user,device,start,end, lot,standard,grit,mark\n"
        "from sessions\n"
        "where id = (select max(id) from sessions)"
            );

  if(!q.exec())
  {
    error(q);
    return session;
  }

  if(!q.first())
    return session;

  return getSession(q);
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: modifySession(const DDASession& session)
{
  if(session.id == InvalidId)
    return;
  int devId = deviceId(session.deviceSerial);
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare(
        "update sessions set user = ?, device = ?, start = ?, end = ?,\n"
        "lot = ?, standard = ?, grit = ?, mark = ?\n"
        "where id = ?"
            );
  q.addBindValue(session.userId);
  q.addBindValue(devId);
  q.addBindValue(session.start);
  q.addBindValue(session.end);
  q.addBindValue(session.lot);
  q.addBindValue(session.standard);
  q.addBindValue(session.gritIndex);
  q.addBindValue(session.mark);
  q.addBindValue(session.id);
  if(!q.exec())
  {
    error(q);
    return;
  }
}
/*----------------------------------------------------------------------------*/
int DDADatabase :: addMeasure(const DDAMeasure& measure)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.exec("select max(id) from measures"))
  {
    error(q);
    return -1;
  }

  int id = 0;
  if(q.first())
    id = q.value(0).toInt();
  ++id;
  q.prepare("insert into measures(id, session, size, strenght, elapsed, ignored) values(?, ?, ?, ?, ?, ?)");
  q.addBindValue(id);
  q.addBindValue(measure.sessionId);
  q.addBindValue(measure.size);
  q.addBindValue(measure.strenght);
  q.addBindValue(measure.elapsed);
  q.addBindValue((int)measure.ignored);
  if(!q.exec())
  {
    error(q);
    return -1;
  }
  return id;
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: modifyMeasure(const DDAMeasure& measure)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));

  q.prepare("update measures set size = ?, strenght = ?, elapsed = ?, ignored = ? where id = ?");
  q.addBindValue(measure.size);
  q.addBindValue(measure.strenght);
  q.addBindValue(measure.elapsed);
  q.addBindValue((int)measure.ignored);
  q.addBindValue(measure.id);
  if(!q.exec())
    error(q);
}
/*----------------------------------------------------------------------------*/
DDAMeasure DDADatabase :: getMeasure(const QSqlQuery &q)
{
  DDAMeasure measure;

  measure.id = q.value(0).toInt();
  measure.sessionId = q.value(1).toInt();
  measure.size = q.value(2).toDouble();
  measure.strenght = q.value(3).toDouble();
  measure.elapsed = q.value(4).toInt();
  measure.ignored = q.value(5).toInt();
  return measure;
}
/*----------------------------------------------------------------------------*/
DDAMeasure DDADatabase :: lastMeasure()
{
  DDAMeasure measure;
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare("select id, session, size, strenght, elapsed, ignored from measures\n"
            "where id = (select max(id) from measures)");

  if(!q.exec())
  {
    error(q);
    return measure;
  }

  if(!q.first())
    return measure;

  return getMeasure(q);
}
/*----------------------------------------------------------------------------*/
bool DDADatabase :: measureSession(int id, DDAMeasureSession *dst)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare(
        "select id, user,device,start,end, lot, standard, grit, mark\n"
        "from sessions\n"
        "where id = ?"
            );
  q.addBindValue(id);
  if(!q.exec())
  {
    error(q);
    return false;
  }

  if(!q.first())
    return false;

  dst->beginUpdate();
  dst->clear();
  dst->setSession(getSession(q));
  q.prepare("select id, session, size, strenght, elapsed, ignored from measures\n"
            "where session = ?");
  q.addBindValue(id);
  if(!q.exec())
  {
    error(q);
    return false;
  }

  if(!q.first())
    return true;

  do
  {
    DDAMeasure m = getMeasure(q);
    dst->addMeasure(m);
  } while(q.next());

  dst->endUpate();
  return true;
}
/*----------------------------------------------------------------------------*/
QString DDADatabase :: userName(int id)
{
  if(m_userMap.empty())
    userList();
  UserMap::Iterator it = m_userMap.find(id);
  if(it != m_userMap.end())
    return it.value();
  return tr("Unkown user");
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: setSerial(const QString& serial)
{
  if(m_serial != serial)
  {
    deviceId(serial);
    m_serial = serial;
  }
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: updateCurrentSession()
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare("update sessions set end = ? where id = ?");
  q.addBindValue(QDateTime::currentDateTime());
  q.addBindValue(m_session);
  if(!q.exec())
    error(q);
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: measure(double strength, double size, int)
{
  DDAMeasure m;
  m.strenght = strength;
  m.size = size;
  m.sessionId = m_session;
  m.elapsed = m_startMeasure.elapsed();
  addMeasure(m);
  updateCurrentSession();
  m_startMeasure = QTime();
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: onEndOfMeasuring()
{
  updateCurrentSession();
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: currentStretch(double)
{
  if(m_startMeasure.isNull())
    m_startMeasure.start();
}
/*----------------------------------------------------------------------------*/
QSqlQuery DDADatabase :: selectSessions(const SessionFilter& filter)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  QString sql;

  sql = "select s.id, s.start, d.serial, u.name, s.lot \n"
      "from sessions s, devices d, users u\n"
      "where u.id = s.user\n"
      "and d.id = s.device\n";

  if(filter.dateSet())
  {
    sql += "and start >= ?\n";
    sql += "and end <= ?\n";
  }

  if(filter.userSet())
    sql += "and s.user = ?\n";

  if(filter.serialSet())
    sql += "and s.device = ?\n";

  sql += "order by s.id desc\n";
  if(filter.limit() > 0)
  {
    sql += QString("limit %1\n").arg(filter.limit());
    if(filter.offset() > 0)
      sql += QString("offset %1").arg(filter.offset());
  }
  q.prepare(sql);
  if(filter.dateSet())
  {
    QDateTime d1(filter.date()), d2(filter.date());
    d1.setTime(QTime(0, 0));
    d2.setTime(QTime(23, 59, 59, 999));
    q.addBindValue(d1);
    q.addBindValue(d2);
  }
  if(filter.userSet())
    q.addBindValue(filter.user());
  if(filter.serialSet())
    q.addBindValue(filter.serial());

  if(!q.exec())
    error(q);
  return q;
}
/*----------------------------------------------------------------------------*/
QList<QDate> DDADatabase :: sessionDateList()
{
  QList<QDate> lst;
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.exec("select distinct DATE(start) from sessions order by start desc"))
  {
    error(q);
    return lst;
  }
  bool eof = !q.first();
  while(!eof)
  {
    lst.append(q.value(0).toDate());
    eof = !q.next();
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
