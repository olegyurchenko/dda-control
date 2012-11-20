/*----------------------------------------------------------------------------*/
/**
* @pkg version
*/
/**
* Short description of version.
*
* Long description of version.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.10.2012 16:02:45<br>
* @pkgdoc version
* @author 
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "version.h"
/*----------------------------------------------------------------------------*/
#define VERSION "0.18.b"
QString versionStr()
{
  return QString("v%1 (build date %2)").arg(VERSION).arg(__DATE__);
}
/*----------------------------------------------------------------------------*/

