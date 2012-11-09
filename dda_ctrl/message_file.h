/*----------------------------------------------------------------------------*/
/**
* @pkg message_file
*/
/**
* Short description of message_file.
*
* Long description of message_file.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 08.11.2012 15:10:26<br>
* @pkgdoc message_file
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef MESSAGE_FILE_H_1352380226
#define MESSAGE_FILE_H_1352380226
/*----------------------------------------------------------------------------*/
#include <QString>
#include <QHash>
#include <QStringList>

class MessageFile
{
protected:
  typedef QHash<QString, QString> StringHash;
  typedef QHash<QString, StringHash> MessageHash;

  QStringList m_langList;
  MessageHash m_messageHash;
  StringHash langHash;
  StringHash countryHash;

  QString m_lang;
  QString m_errorString;


public:
  MessageFile();
  bool load(const QString& fileName);
  bool save(const QString& fileName);
  void setLang(const QString& lang);
  void setMessage(const QString& source, const QString& msg, const QString& lang = QString());
  void addMessage(const QString& source, const QString& msg, const QString& lang = QString());
  QString lang() {return m_lang;}
  QString errorString() {return m_errorString;}
  QStringList langList() {return m_langList;}
  QStringList sourceList();
  QString message(const QString& source, const QString& lang = QString());
  void deleteSource(const QString& source);
  void addSource(const QString& source);

  void deleteLang(const QString& lang);
  void addLang(const QString& lang);

  QStringList countryList();
  QStringList langsList();
  QString mkLocale(int langIndex, int countryIndex);
  QString locale(QString lang);

};
/*----------------------------------------------------------------------------*/
#endif /*MESSAGE_FILE_H_1352380226*/

