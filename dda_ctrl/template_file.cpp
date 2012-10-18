/*----------------------------------------------------------------------------*/
/**
* @pkg template_file
*/
/**
* Template file handle.
*
* (C) T&T, Kiev, Ukraine 2011.<br>
* started 06.09.2011 17:30:45<br>
* @pkgdoc template_file
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "template_file.h"
#include <QFile>
#include <QtDebug>
#include <QRegExp>
#include <QTextCodec>
#include <QStringList>
/*----------------------------------------------------------------------------*/
TemplateSet :: TemplateSet(const QString& fileName)
{
  readFile(fileName);
}
/*----------------------------------------------------------------------------*/
TemplateSet :: ~TemplateSet()
{
}
/*----------------------------------------------------------------------------*/
void TemplateSet :: readFile(const QString& fileName)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Error open file " << fileName;
    return;
  }

  while (!file.atEnd())
  {
    QString line = QString::fromUtf8(file.readLine());
    parceString(line);
  }

}
/*----------------------------------------------------------------------------*/
QString TemplateSet :: resolve(const QString& name, const QString& format, QVariantMap* dict)
{
  QVariant result;
  QString resultStr;
  bool found = false;

  if(dict != NULL)
  {
    QVariantMap::const_iterator it = dict->find(name);

    if(it != dict->end())
    {
      result = it.value();
      found = true;
    }
  }

  if(!found)
  {
    StringMap::iterator it = strHash.find(name);
    if(it != strHash.end())
    {
      result = it.value();
    }
    else
      qDebug() << "Variable" << name << "not found";
  }

  if(!format.isEmpty())
  {
    QString formatStr;
    if(format.at(0).toAscii() != '%')
      formatStr = "%";
    formatStr += format;
    switch(formatStr.at(formatStr.size() - 1).toAscii())
    {
    case 'u':
    case 'X':
    case 'x':
      resultStr = QString().sprintf(formatStr.toAscii().constData(), result.toUInt());
      break;
    case 'd':
      resultStr = QString().sprintf(formatStr.toAscii().constData(), result.toInt());
      break;
    case 'f':
      resultStr = QString().sprintf(formatStr.toAscii().constData(), result.toDouble());
      break;
    case 's':
      resultStr = QString().sprintf(formatStr.toAscii().constData(), result.toString().toAscii().constData());
      break;
    default:
      qDebug() << "Format specifier not supported" << format;
      resultStr = result.toString();
      break;
    }
  }
  else
    resultStr = result.toString();
  return resultStr;
}
/*----------------------------------------------------------------------------*/
QString TemplateSet :: getString(const QString& name, QVariantMap* dict)
{
//  QString charset = resolve("charset", "", dict);
//  QTextCodec* codec = QTextCodec::codecForName(charset.toAscii().constData());

  int maxPass = 1024;

  QString str = resolve(name, "", dict);
  while(maxPass)
  {
    //src [^\\](\{)[^\{\} \t]+(\})
    QRegExp re("(\\{)[^\\{\\} \\t]+(\\})");
//    qDebug() << str;
//    qDebug() << re.indexIn(str);
//    qDebug() << re.pos(0) << re.pos(1) << re.pos(2);
    if(re.indexIn(str) < 0 || re.pos(1) < 0 || re.pos(2) < 0)
      break;
    QString macro = str.mid(re.pos(1) + 1, re.pos(2)-re.pos(1) - 1);
    QStringList lst = macro.split(':');
    QString nm,fm;
    if(lst.count() > 0)
      nm = lst[0];
    if(lst.count() > 1)
      fm = lst[1];
    QString value = resolve(nm, fm, dict);
    str.replace(re.pos(1), re.pos(2) - re.pos(1) + 1, value);
    maxPass --;
  }

  if(!maxPass)
  {
    qDebug() << "Max pass riched while handle sting:" << str;
  }

//  if(codec != NULL)
//    str = codec->fromUnicode(str);
  return str;
}
/*----------------------------------------------------------------------------*/
void TemplateSet :: parceString(const QString& src)
{
  int pos = src.indexOf(QRegExp("^[a-zA-Z_@][a-zA-Z_@0-9\.]*:"));

  int size = src.size();

  QString name;
  QString value;
  QString *s = &name;

  if(pos < 0 && lastVar.isEmpty())
    return;

  if(pos < 0)
  {
    name = lastVar;
    value = strHash[name] + "\n";
    s = &value;
    pos = 0;
  }

  for(int i = pos; i < size; i++)
  {
    switch(src.at(i).toAscii())
    {
    case ':':
    case ' ':
    case '\t':
      if(s == &name)
        s = &value;
      else
        s->append(src.at(i));
      break;
/*
    case '#':
      if(!quote) //comment
        i = size;
      else
        s->append(src.at(i));
      break;
    case '"':
      if(quote)
        i = size; //break
      quote = !quote;
      break;
    case '\r':
    case '\n':
      break;
    case '\\':
      if(i < size - 1)
      {
        switch(src.at(++i).toAscii())
        {
        case 'a':
          s->append('\a');
          break;
        case 'b':
          s->append('\b');
          break;
        case 'f':
          s->append('\f');
          break;
        case 'n':
          s->append('\n');
          break;
        case 'r':
          s->append('\r');
          break;
        case 't':
          s->append('\t');
          break;
        case 'v':
          s->append('\v');
          break;
        case 'x': //hex code
        case 'X': //hex code
          s->append(QChar(src.mid(i+1, 2).toUInt(NULL, 16)));
          i += 3;
          break;
        case '0': //octal code
          s->append(QChar(src.mid(i, 3).toUInt(NULL, 8)));
          i += 3;
          break;
        case 'o':
        case 'O':
          s->append(QChar(src.mid(i+1, 3).toUInt(NULL, 8)));
          i += 4;
          break;
        default:
          s->append(src.at(i));
          break;

        }
      }
      break;
*/
    default:
      s->append(src.at(i));
      break;
    }
  }
  strHash[name] = value;
  lastVar = name;
}
/*----------------------------------------------------------------------------*/
void TemplateSet :: setString(const QString& name, const QString& value)
{
  strHash[name] = value;
}
/*----------------------------------------------------------------------------*/
