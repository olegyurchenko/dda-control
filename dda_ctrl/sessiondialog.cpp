#include "sessiondialog.h"
#include "ui_sessiondialog.h"
#include <configuration.h>
/*----------------------------------------------------------------------------*/
SessionDialog::SessionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SessionDialog)
{
  ui->setupUi(this);
  m_userList = database->userList();
  int size = m_userList.size();
  for(int i = 0; i < size; i++)
    ui->userCombo->addItem(m_userList[i].name);

  ui->userCombo->setCurrentIndex(0);

  ui->meshCombo->addItems(database->meshList());
  ui->meshCombo->setCurrentIndex(config->profile().meshIndex);

  ui->gostCombo->addItems(database->gostList());
  ui->gostCombo->setCurrentIndex(config->profile().gostIndex);

  ui->giftCountSpin->setValue(config->profile().giftCount);
}
/*----------------------------------------------------------------------------*/
SessionDialog::~SessionDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void SessionDialog::onUserListIndexChanged(int)
{
}
/*----------------------------------------------------------------------------*/
void SessionDialog::onManageUsers()
{
}
/*----------------------------------------------------------------------------*/
void SessionDialog::onAccepted()
{
  DDAProfile profile = config->profile();
  profile.gostIndex = ui->gostCombo->currentIndex();
  profile.meshIndex = ui->meshCombo->currentIndex();
  profile.giftCount = ui->giftCountSpin->value();
  config->setProfile(profile);
}
/*----------------------------------------------------------------------------*/
