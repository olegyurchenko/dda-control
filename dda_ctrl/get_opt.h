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
#ifndef GET_OPT_H_1267886328
#define GET_OPT_H_1267886328
#include <QString>
#include <QChar>
#include <QVariant>
/*----------------------------------------------------------------------------*/
bool getOptSwitch(const QString& longOpt, QChar shortOpt);
QVariant getOptValue(const QString& longOpt, QChar shortOpt, const QVariant& defValue = QVariant());
/*----------------------------------------------------------------------------*/
#endif /*GET_OPT_H_1267886328*/

