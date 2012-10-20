#include <QtGui/QApplication>
#include "analyswindow.h"
#include <configuration.h>
#include <database.h>
#include <editprofiledialog.h>
#include <profileselectdialog.h>
#include <QMessageBox>
#include <controller.h>
#include <session.h>
#include <get_opt.h>
#include <qmlprocessing.h>
#include <QLibraryInfo>
#include <QTranslator>
#include <QLocale>
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  a.addLibraryPath(a.applicationDirPath() + "/plugins");

  config = new DDAConfig(&a);
  if(config->fileExists() && config->isError())
  {
    QMessageBox::critical(NULL, QObject::tr("Error load config"), config->message());
  }

  QTranslator trans;
  QString locale;
  locale = config->settings().localeName;
  if(locale.isEmpty())
    locale = QLocale::system().name();

  if( locale.length() >= 2
    && locale.left(2) != "en"
    && !trans.load("dda-" + locale)
    && !trans.load("dda-" + locale, QCoreApplication::applicationDirPath())
    && !trans.load("dda-" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
  {
    QString error = QString("Error load language file for `%1` locale").arg(locale);
    QMessageBox::critical(NULL, QObject::tr("Error translation"), error);
    if(config->settings().localeName.isEmpty())
    {
      DDASettings s = config->settings();
      s.localeName = "en";
      config->setSettings(s);
    }
  }
  else
  {
    if(config->settings().localeName.isEmpty())
    {
      DDASettings s = config->settings();
      s.localeName = QLocale::system().name().left(2);
      config->setSettings(s);
    }
  }

  a.installTranslator(&trans);
  translator = &trans;

  database = new DDADatabase(&a);
  if(database->isError())
  {
    QMessageBox::critical(NULL, QObject::tr("Error open database"), database->message());
  }

  AnalysWindow w;
  w.showMaximized();
  
  return a.exec();
}
/*----------------------------------------------------------------------------*/
