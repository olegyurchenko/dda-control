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

  QTranslator translator;
  QString locale;
  locale = config->settings().localeName;
  if(locale.isEmpty())
    locale = QLocale::system().name();

  if( locale.length() >= 2
    && locale.left(2) != "en"
    && !translator.load("dda-" + locale)
    && !translator.load("dda-" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
  {
    QString error = QString("Error load language file for `%1` locale").arg(locale);
    QMessageBox::critical(NULL, QObject::tr("Error translation"), error);
  }
  a.installTranslator(&translator);

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
