/*----------------------------------------------------------------------------*/
/**
* @pkg translator
*/
/**
* Class translator to multilanguage support.
*
* Work with xml message files.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 08.11.2012 11:17:49<br>
* @pkgdoc translator
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef TRANSLATOR_H_1352366269
#define TRANSLATOR_H_1352366269
/*----------------------------------------------------------------------------*/
#include <QTranslator>
#include <QHash>

class MessageFile;
class Translator : public QTranslator
{
protected:
  MessageFile *m_messageFile;
  QString m_errorString;
  QString m_lang;
  QString m_directory;
  QString m_fileName;
  void saveModified();
public:
  Translator(QObject *parent = 0);
  virtual ~Translator();
  virtual QString translate (const char *context, const char *sourceText, const char *disambiguation = 0) const;
  bool load(const QString& filename, const QString& directory = QString());
  QStringList languages();
  bool setLang(const QString &lang);
  QString lang() {return m_lang;}
  QString errorString() {return m_errorString;}
};

/*----------------------------------------------------------------------------*/
#endif /*TRANSLATOR_H_1352366269*/

