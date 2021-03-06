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
#ifndef TEMPLATE_FILE_H_1315319445
#define TEMPLATE_FILE_H_1315319445
/*----------------------------------------------------------------------------*/
#include <QVariant>
#include <QMap>

class TemplateSet
{
  typedef QMap<QString, QString> StringMap;
  StringMap strHash;
  QString lastVar;
protected:
  void parceString(const QString& src);
  void readFile(const QString& fileName);
  QString resolve(const QString& name, const QString& format, QVariantMap* dict);
public:
  TemplateSet(const QString& fileName);
  ~TemplateSet();
  /**Get reult string with resolve {var} from dictionary*/
  QString getString(const QString& name, QVariantMap* dict = NULL);
  void setString(const QString& name, const QString& value);
};

/*----------------------------------------------------------------------------*/
#endif /*TEMPLATE_FILE_H_1315319445*/
