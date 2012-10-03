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

  if(!q.exec("select count(*) from mesh"))
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
      q.prepare("insert into mesh(id, txt) values(?, ?)");
      q.addBindValue(i);
      q.addBindValue(lst[i]);
      if(!q.exec())
      {
        error(q);
        return;
      }
    }

  }

  if(!q.exec("select count(*) from gost"))
  {
    error(q);
    return;
  }

  if(!q.first() || !q.value(0).toInt())
  {
    f.close();
    f.setFileName(":/sql/gost.txt");
    if(!f.open(QIODevice::ReadOnly))
    {
      m_isError = true;
      m_message = tr("Error open %1").arg("gost.txt");
      return;
    }
    QString txt = f.readAll();
    txt.remove('\r');
    QStringList lst = txt.split('\n', QString::SkipEmptyParts);
    int size = lst.size();
    for(int i = 0; i < size; i++)
    {
      q.prepare("insert into gost(id, txt) values(?, ?)");
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
QStringList DDADatabase :: meshList()
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.exec("select txt from mesh order by id"))
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
QStringList DDADatabase :: gostList()
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.exec("select txt from gost order by id"))
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
DDAUserList DDADatabase :: userList()
{
  DDAUserList lst;
  if(!m_userMap.empty())
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
  if(!q.exec("select id, name from users where active <> 0 order by id"))
  {
    error(q);
    return DDAUserList();
  }

  bool eof = !q.first();
  while(!eof)
  {
    DDAUser user;
    user.id = q.value(0).toInt();
    user.name = q.value(1).toString();
    lst.append(user);
    m_userMap[user.id] = user.name;
    eof = !q.next();
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: userAdd(QString name, QString passw)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.prepare("insert into users(active, name, pswd, used) values(1, ?, ?, 0)"))
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

  q.addBindValue(name);
  q.addBindValue(hash.result().toHex() + salt);
  if(!q.exec())
    error(q);
  emit userChanged(-1);
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
  if(!q.prepare("select pswd where id = ?"))
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
  salt = shash.right(32);
  shash.remove(32, shash.size() - 32);
  QCryptographicHash hash(QCryptographicHash::Md5);
  hash.addData(salt.toAscii());
  hash.addData(passw.toUtf8());
  return shash == hash.result().toHex();
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
        "insert into sessions(id, user,device,start,end, lot,mesh,gost,mark)\n"
        "values(?,?,?,?,?,?,?,?,?)"
            );
  q.addBindValue(id);
  q.addBindValue(session.userId);
  q.addBindValue(devId);
  q.addBindValue(session.start);
  q.addBindValue(session.end);
  q.addBindValue(session.lot);
  q.addBindValue(session.meshIndex);
  q.addBindValue(session.gostIndex);
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
  session.deviceSerial = deviceId(q.value(2).toString());
  session.start = q.value(3).toDateTime();
  session.end = q.value(4).toDateTime();
  session.lot = q.value(5).toString();
  session.meshIndex = q.value(6).toInt();
  session.gostIndex = q.value(7).toInt();
  session.mark = q.value(8).toString();

  return session;
}
/*----------------------------------------------------------------------------*/
DDASession DDADatabase :: lastSession()
{
  DDASession session;
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare(
        "select id, user,device,start,end, lot,mesh,gost,mark\n"
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
        "lot = ?, mesh = ?, gost = ?, mark = ?\n"
        "where id = ?"
            );
  q.addBindValue(session.userId);
  q.addBindValue(devId);
  q.addBindValue(session.start);
  q.addBindValue(session.end);
  q.addBindValue(session.lot);
  q.addBindValue(session.meshIndex);
  q.addBindValue(session.gostIndex);
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
  q.prepare("insert into measures(id, session, size, strenght, elapsed) values(?, ?, ?, ?, ?)");
  q.addBindValue(id);
  q.addBindValue(measure.sessionId);
  q.addBindValue(measure.size);
  q.addBindValue(measure.strenght);
  q.addBindValue(measure.elapsed);
  if(!q.exec())
  {
    error(q);
    return -1;
  }
  return id;
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
  return measure;
}
/*----------------------------------------------------------------------------*/
DDAMeasure DDADatabase :: lastMeasure()
{
  DDAMeasure measure;
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare("select id, session, size, strenght, elapsed from measures\n"
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
        "select id, user,device,start,end, lot,mesh,gost,mark\n"
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
  dst->setSession(getSession(q));
  q.prepare("select id, session, size, strenght, elapsed from measures\n"
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
void DDADatabase :: measure(double strength, double size, int)
{
  DDAMeasure m;
  m.strenght = strength;
  m.size = size;
  m.sessionId = m_session;
  addMeasure(m);
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: onEndOfMeasuring()
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  q.prepare("update sessions set end = ? where id = ?");
  q.addBindValue(QDateTime::currentDateTime());
  q.addBindValue(m_session);
  if(!q.exec())
    error(q);
}
/*----------------------------------------------------------------------------*/
