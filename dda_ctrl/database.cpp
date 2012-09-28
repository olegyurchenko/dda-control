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

  if(!q.exec("select id from users where id = 0"))
  {
    error(q);
    return;
  }

  if(!q.first())
  {
    q.prepare("insert into users(id, active, name) values(0, 0, ?)");
    q.addBindValue("root");
    if(!q.exec())
    {
      error(q);
      return;
    }
    setPassword(0, "");
  }

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
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.exec("select id, name from users where active <> 0 order by id"))
  {
    error(q);
    return DDAUserList();
  }

  DDAUserList lst;

  bool eof = !q.first();
  while(!eof)
  {
    DDAUser user;
    user.id = q.value(0).toInt();
    user.name = q.value(1).toString();
    lst.append(user);
    eof = !q.next();
  }
  return lst;
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: userAdd(QString name, QString passw)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.prepare("insert into users(active, name, pswd, salt) values(1, ?, ?, ?)"))
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
  q.addBindValue(hash.result().toHex());
  q.addBindValue(salt);
  if(!q.exec())
    error(q);
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: userDel(int id)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.prepare("update users set active = 0 where id = ?"))
  {
    error(q);
    return;
  }
  q.addBindValue(id);
  if(!q.exec())
    error(q);
}
/*----------------------------------------------------------------------------*/
bool DDADatabase :: checkPassword(int id, QString passw)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.prepare("select pswd, salt where id = ?"))
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
  QCryptographicHash hash(QCryptographicHash::Md5);
  hash.addData(q.value(1).toString().toAscii());
  hash.addData(passw.toUtf8());
  return q.value(0).toString() == hash.result().toHex();
}
/*----------------------------------------------------------------------------*/
void DDADatabase :: setPassword(int id, QString passw)
{
  QSqlQuery q(QSqlDatabase::database(CONNECTION_NAME));
  if(!q.prepare("update users set pswd = ?, salt = ? where id = ?"))
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

  q.addBindValue(hash.result().toHex());
  q.addBindValue(salt);
  q.addBindValue(id);
  if(!q.exec())
    error(q);
}
/*----------------------------------------------------------------------------*/

