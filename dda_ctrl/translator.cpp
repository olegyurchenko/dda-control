/*----------------------------------------------------------------------------*/
/**
* @pkg translator
*/
/**
* Short description of translator.
*
* Long description of translator.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 08.11.2012 11:17:49<br>
* @pkgdoc translator
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "translator.h"
#include <QDir>
#include <QCoreApplication>
#include <message_file.h>

/*----------------------------------------------------------------------------*/
Translator :: Translator(QObject *parent)
  : QTranslator(parent)
{
  m_messageFile = new MessageFile;
  m_lang = "C";
}
/*----------------------------------------------------------------------------*/
Translator :: ~Translator()
{
  saveModified();
  delete m_messageFile;
}
/*----------------------------------------------------------------------------*/
void Translator :: saveModified()
{
  if(m_messageFile->isModified())
  {
    int p = m_fileName.indexOf('.');
    if(p < 0)
      return;
    QString fileName = QString("%1.%2.%3").arg(m_fileName.left(p), "new", m_fileName.mid(p + 1));
    QDir dir;
    dir.setPath(m_directory);
    m_messageFile->save(dir.filePath(fileName));
  }
}
/*----------------------------------------------------------------------------*/
QString Translator :: translate(const char *, const char *sourceText, const char *) const
{
  QString src = QString::fromUtf8(sourceText);
  if(m_lang == "C")
    return src;

  QString result = m_messageFile->message(src, m_lang);
  if(result.isEmpty())
    m_messageFile->addMessage(src, "", m_lang);
  return result;
}
/*----------------------------------------------------------------------------*/
bool Translator :: load(const QString& filename, const QString& directory)
{
  QDir dir;

  if(directory.isEmpty())
    dir.setPath(qApp->applicationDirPath());
  else
    dir.setPath(directory);

  if(!m_messageFile->load(dir.filePath(filename)))
  {
    m_errorString = m_messageFile->errorString();
    return false;
  }

  m_fileName = filename;
  m_directory = dir.path();
  return true;
}
/*----------------------------------------------------------------------------*/
QStringList Translator :: languages()
{
  return m_messageFile->langList();
}
/*----------------------------------------------------------------------------*/
bool Translator :: setLang(const QString &lang)
{
  if(lang.isEmpty() || lang == "C")
  {
    m_lang ="C";
    return true;
  }

  QStringList langs = languages();
  int index = langs.indexOf(lang);
  if(index == -1)
  {
    int size = langs.size();
    for(int i = 0; i < size; i++)
    {
      if(langs[i].indexOf(lang) == 0)
      {
        index = i;
        break;
      }
    }

  }

  if(index == -1)
  {
    m_errorString = QString("Language '%1' not found").arg(lang);
    return false;
  }

  m_lang = langs[index];

  return true;
}
/*----------------------------------------------------------------------------*/
