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
#include <translator.h>
#include <QLocale>
#include <QDir>
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

  translator = new Translator(&a);
  QString locale;
  locale = config->settings().localeName;
  if(locale.isEmpty())
    locale = QLocale::system().name();

  if(!translator->load("dda-messages.xml")
     && !translator->load("dda-messages.xml", QDir::currentPath())
     && !translator->load("dda-messages.xml", QLibraryInfo::location(QLibraryInfo::TranslationsPath))
     && locale != "C")
  {
    QString error = QString("Error load language file '%1': %2").arg("dda-messages.xml").arg(translator->errorString());
    QMessageBox::critical(NULL, "Error translation", error);
    if(config->settings().localeName.isEmpty())
    {
      DDASettings s = config->settings();
      s.localeName = "C";
      config->setSettings(s);
    }
  }
  else
  {
    if(!translator->setLang(locale))
    {
      QString error = QString("Error set language: %1").arg(translator->errorString());
      QMessageBox::critical(NULL, "Error translation", error);
      if(config->settings().localeName.isEmpty())
      {
        DDASettings s = config->settings();
        s.localeName = "C";
        config->setSettings(s);
      }
    }
    else
    if(config->settings().localeName.isEmpty())
    {
      DDASettings s = config->settings();
      s.localeName = QLocale::system().name();
      config->setSettings(s);
    }
  }

  a.installTranslator(translator);

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
