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

/*----------------------------------------------------------------------------*/
struct DDAUser
{
  int id;
  QString name;
};
typedef QList<DDAUser> DDAUserList;
/*----------------------------------------------------------------------------*/
class QSqlQuery;
class DDADatabase : public QObject
{
  Q_OBJECT
protected:
  bool m_isError;
  QString m_message;
  bool error(const QSqlQuery &q);
public:
  DDADatabase(QObject *parent = 0);
  ~DDADatabase();
  bool isError() const {return m_isError;}
  QString message() {return m_message;}
  QStringList meshList();
  QStringList gostList();
  DDAUserList userList();
  void userAdd(QString name, QString passw);
  void userDel(int id);
  bool checkPassword(int id, QString passw);
  void setPassword(int id, QString passw);

signals:
  void dbError(const QString &error);
};

extern DDADatabase *database;
/*----------------------------------------------------------------------------*/
#endif /*DATABASE_H_1348809604*/

