/*----------------------------------------------------------------------------*/
/**
* @pkg get_opt
*/
/**
* Получение опций командной строки.
*
* Разбирает опции командной строки вида:<br>
* <app_name> --long-opt -sS --long-val value -v value --long-val=value<br>
* (C) T&T, Kiev, Ukraine 2010.<br>
* started 06.03.2010 16:38:48<br>
* @pkgdoc get_opt
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "get_opt.h"
#include <QCoreApplication>
#include <QStringList>
#include <QRegExp>
/*----------------------------------------------------------------------------*/
bool getOptSwitch(const QString& longOpt, QChar shortOpt)
{
  QStringList args = QCoreApplication :: arguments();
  //Find in arguments: `--long-opt`
  if(args.indexOf(QString("--%1").arg(longOpt)) >= 0)
    return true;
  if(args.indexOf(QRegExp(QString("^--%1=.*").arg(longOpt))) >= 0)
    return true;
  return args.indexOf(QRegExp(QString("^-[a-zA-Z]*%1[a-zA-Z]*").arg(shortOpt))) >= 0;
}
/*----------------------------------------------------------------------------*/
QVariant findOptValue(const QStringList& args, const QString& opt)
{
  int c = args.indexOf(QRegExp(QString("%1=.*").arg(opt)));
  if(c >= 0)
  {
    int pos = opt.size() + 1;
    return QVariant(args.at(c).mid(pos));
  }
  else
  {
    c = args.indexOf(opt);
    if(c >= 0 && c < args.size() - 1)
      return QVariant(args.at(c+1));
  }
  return QVariant();
}
/*----------------------------------------------------------------------------*/
QVariant getOptValue(const QString& longOpt, QChar shortOpt, const QVariant& defValue)
{
  QStringList args = QCoreApplication :: arguments();
  QVariant v = findOptValue(QCoreApplication::arguments(), QString("--%1").arg(longOpt));
  if(v.isNull())
    v = findOptValue(QCoreApplication::arguments(), QString("-%1").arg(shortOpt));
  if(v.isNull())
    v = defValue;
  return v;
}
/*----------------------------------------------------------------------------*/
