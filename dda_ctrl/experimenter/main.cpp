#include <QtGui/QApplication>
#include "measurewindow.h"
#include <configuration.h>
#include <database.h>
#include <editprofiledialog.h>
#include <profileselectdialog.h>
#include <QMessageBox>
#include <controller.h>
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  config = new DDAConfig(&a);
  if(config->fileExists() && config->isError())
  {
    QMessageBox::critical(NULL, QObject::tr("Error load config"), config->message());
  }

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

  controller = new DDAController(&a);

  MeasureWindow w;
  w.show();
  //w.showMaximized();
  return a.exec();
}
/*----------------------------------------------------------------------------*/
