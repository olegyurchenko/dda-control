#include <QtGui/QApplication>
#include "measurewindow.h"
#include <configuration.h>
#include <database.h>
#include <editprofiledialog.h>
#include <profileselectdialog.h>
#include <QMessageBox>
#include <controller.h>
#include <session.h>
#include <get_opt.h>
#include <QLibraryInfo>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <stdio.h>
#include <translator.h>
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



  QStringList profiles = config->profileList();
/*
  if(profiles.size() > 1)
  {
    ProfileSelectDialog dlg;
    if(dlg.exec() != QDialog::Accepted || dlg.selectedProfile() < 0 || dlg.selectedProfile() > profiles.size())
      return 1;
    config->setProfileIndex(dlg.selectedProfile());
  }
  else */
  if(profiles.isEmpty())
  {
    DDAProfile profile;
    config->defaultProfle(&profile);
    EditProfileDialog dlg;
    dlg.setProfile(profile);
    if(dlg.exec() != QDialog::Accepted)
      return 1;
    config->addProfile(dlg.profile());
    config->setProfileIndex(0);
  }

  int defaultProfile = getOptValue("profile", 'p', -1).toInt();
  if(defaultProfile > 0)
    config->setProfileIndex(defaultProfile - 1);


  session = new DDAMeasureSession(&a);

  if(getOptSwitch("demo", 'd') || getOptSwitch("demo-mode", 'D'))
    controller = new DemoController(&a);
  else
    controller = new DDAController(&a);

  QObject::connect(controller, SIGNAL(measure(double,double,int)), session, SLOT(addMeasure(double,double,int)));
  QObject::connect(controller, SIGNAL(serialReceived(QString)), session, SLOT(setSerial(QString)));
  QObject::connect(controller, SIGNAL(endOfMeasuring()), session, SLOT(onEndOfMeasuring()));
  QObject::connect(controller, SIGNAL(noParticle()), session, SLOT(onNoParticle()));

  QObject::connect(controller, SIGNAL(measure(double,double,int)), database, SLOT(measure(double,double,int)));
  QObject::connect(controller, SIGNAL(serialReceived(QString)), database, SLOT(setSerial(QString)));
  QObject::connect(controller, SIGNAL(endOfMeasuring()), database, SLOT(onEndOfMeasuring()));
  QObject::connect(controller, SIGNAL(noParticle()), database, SLOT(onNoParticle()));

  MeasureWindow w;
  w.show();
  //w.showMaximized();
  return a.exec();
}
/*----------------------------------------------------------------------------*/
