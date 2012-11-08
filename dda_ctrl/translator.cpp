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
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

static unsigned crc32(const unsigned char *buffer, int len);
/*----------------------------------------------------------------------------*/
Translator :: Translator(QObject *parent)
  : QTranslator(parent)
{
  root = new QDomDocument;
  m_lang = "C";
}
/*----------------------------------------------------------------------------*/
Translator :: ~Translator()
{
  delete root;
}
/*----------------------------------------------------------------------------*/
QString Translator :: translate(const char *, const char *sourceText, const char *) const
{
  if(m_lang == "C")
    return QString::fromUtf8(sourceText);

  unsigned h = hash(sourceText);
  MessageTable::ConstIterator it = m_messageTable.find(h);
  if(it != m_messageTable.end())
    return it.value();
  return QString();
}
/*----------------------------------------------------------------------------*/
bool Translator :: load(const QString& filename, const QString& directory)
{
  QDir dir;

  if(directory.isEmpty())
    dir.setPath(qApp->applicationDirPath());
  else
    dir.setPath(directory);

  QFile confFile(dir.filePath(filename));

  if(!confFile.open(QIODevice::ReadOnly))
  {
    m_errorString = confFile.errorString();
    return false;
  }

  QString errMsg;
  int errorLine, errorColumn;
  if(!root->setContent(&confFile, &errMsg, &errorLine, &errorColumn))
  {
    m_errorString = QString("Error %1:%2:%3: %4").arg(confFile.fileName()).arg(errorLine).arg(errorColumn).arg(errMsg);
    return false;
  }
  return true;
}
/*----------------------------------------------------------------------------*/
QStringList Translator :: languages()
{
  QStringList lst;
  QDomElement el = root->firstChildElement("translator");
  for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    if(n.nodeName() == "languages")
    {
      for(QDomNode nn = n.firstChild(); !nn.isNull(); nn = nn.nextSibling())
      {
        if(nn.nodeName() == "language")
          lst.append(nn.toElement().text());
      }
    }
  }
  return lst;
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
  m_messageTable.clear();

  QDomElement el = root->firstChildElement("translator");
  for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    if(n.nodeName() == "message")
    {
      unsigned h = 0;
      for(QDomNode nn = n.firstChild(); !nn.isNull(); nn = nn.nextSibling())
      {
        if(nn.nodeName() == "source")
        {
          h = hash(nn.toElement().text().toUtf8().constData());
        }
        else
        if(nn.nodeName() == m_lang)
        {
          m_messageTable[h] = nn.toElement().text();
          break;
        }
      }
    }
  }
  return true;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
static unsigned reflect(unsigned ref, char ch)
{
  // Used only by init table.
  unsigned value = 0;
  int i;

  // Swap bit 0 for bit 7
  // bit 1 for bit 6, etc.
  for(i = 1; i < (ch + 1); i++)
  {
    if(ref & 1)
      value |= 1 << (ch - i);
    ref >>= 1;
  }
  return value;
}
/*----------------------------------------------------------------------------*/
/*CRC32 algorithm*/
unsigned Translator :: hash(const char *text)
{
  static unsigned *crc_table = NULL;
  unsigned  crc = 0xffffffff;

  if(crc_table == NULL)
  {
    static const unsigned polynomial = 0x04c11db7;
    int i,j;

    crc_table = new unsigned [256];

    // 256 values representing ASCII character codes.
    for(i = 0; i < 256; i++)
    {
      crc_table[i] = reflect((unsigned)i, 8) << 24;
      for (j = 0; j < 8; j++)
        crc_table[i] = (crc_table[i] << 1) ^ (crc_table[i] & (1 << 31) ? polynomial : 0);
      crc_table[i] = reflect(crc_table[i], 32);
    }
  }

  while(*text)
    crc = (crc >> 8) ^ crc_table[(crc & 0xFF) ^ *text++];
  // Exclusive OR the result with the beginning value.
  return crc ^ 0xffffffff;
}
/*----------------------------------------------------------------------------*/
