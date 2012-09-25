#include "profileselectdialog.h"
#include "ui_profileselectdialog.h"
#include <configuration.h>
#include <editprofiledialog.h>
/*----------------------------------------------------------------------------*/
ProfileSelectDialog::ProfileSelectDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ProfileSelectDialog)
{
  ui->setupUi(this);
  ui->listWidget->addItems(config->profileList());
  ui->listWidget->setCurrentRow(0);
  m_selectedProfile = 0;

}
/*----------------------------------------------------------------------------*/
ProfileSelectDialog::~ProfileSelectDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void ProfileSelectDialog::onProfileChanged(int index)
{
  m_selectedProfile = index;
}
/*----------------------------------------------------------------------------*/
void ProfileSelectDialog::onEditBtnClicked()
{
  config->setProfileIndex(m_selectedProfile);
  EditProfileDialog dlg(this);
  dlg.setProfile(config->profile());
  if(dlg.exec() == QDialog::Accepted)
  {
    config->setProfile(dlg.profile());
    ui->listWidget->clear();
    ui->listWidget->addItems(config->profileList());
    m_selectedProfile = config->profileIndex();
    ui->listWidget->setCurrentRow(m_selectedProfile);
  }
}
/*----------------------------------------------------------------------------*/
void ProfileSelectDialog::onNewBtnClicked()
{
  DDAProfile profile;
  config->defaultProfle(&profile);
  EditProfileDialog dlg(this);
  dlg.setProfile(profile);
  if(dlg.exec() == QDialog::Accepted)
  {
    config->addProfile(dlg.profile());
    ui->listWidget->clear();
    ui->listWidget->addItems(config->profileList());
    m_selectedProfile = config->profileIndex();
    ui->listWidget->setCurrentRow(m_selectedProfile);
  }
}
/*----------------------------------------------------------------------------*/
void ProfileSelectDialog::onDebBtnClicked()
{
  QStringList lst = config->profileList();
  if(lst.size() > 1)
  {
    config->rmProfile(m_selectedProfile);
    ui->listWidget->clear();
    ui->listWidget->addItems(config->profileList());
    m_selectedProfile = 0;
    config->setProfileIndex(m_selectedProfile);
    ui->listWidget->setCurrentRow(m_selectedProfile);
  }
}
/*----------------------------------------------------------------------------*/
