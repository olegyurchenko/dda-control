/*----------------------------------------------------------------------------*/
/**
* @pkg configuration
*/
/**
* Configuration for DDA.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 24.09.2012 10:58:13<br>
* @pkgdoc configuration
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef CONFIGURATION_H_1348473493
#define CONFIGURATION_H_1348473493
#include <QObject>
#include <QStringList>
/*----------------------------------------------------------------------------*/
class DDAConfig : public QObject
{
public:
  DDAConfig(QObject *parent = 0);
  QStringList profileList();
  void setProfile(int index);
};
/*----------------------------------------------------------------------------*/
extern DDAConfig *config;
/*----------------------------------------------------------------------------*/
#endif /*CONFIGURATION_H_1348473493*/

