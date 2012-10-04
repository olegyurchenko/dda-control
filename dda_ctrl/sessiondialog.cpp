#include "sessiondialog.h"
#include "ui_sessiondialog.h"
#include <configuration.h>
#include <session.h>
#include <controller.h>
#include <usermanagedialog.h>
/*----------------------------------------------------------------------------*/
SessionDialog::SessionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SessionDialog)
{
  ui->setupUi(this);

  connect(database, SIGNAL(userChanged(int)), this, SLOT(onUsersChanged()));

  onUsersChanged();
  //ui->userCombo->setCurrentIndex(0);

  ui->meshCombo->addItems(database->meshList());
  ui->meshCombo->setCurrentIndex(config->profile().meshIndex);

  ui->gostCombo->addItems(database->gostList());
  ui->gostCombo->setCurrentIndex(config->profile().gostIndex);

  ui->giftCountSpin->setValue(config->profile().giftCount);

  if(session->session().id != InvalidId)
  {
    ui->meshCombo->setCurrentIndex(session->session().meshIndex);
    ui->gostCombo->setCurrentIndex(session->session().gostIndex);
    ui->lotEdit->setText(session->session().lot);
    ui->markEdit->setEnabled(true);
    ui->markEdit->setText(session->session().mark);
  }
}
/*----------------------------------------------------------------------------*/
SessionDialog::~SessionDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void SessionDialog::onUserListIndexChanged(int index)
{
  if(index <= 0 || index >= m_userList.size())
    return;

  if(m_userList[index].id == session->session().userId)
    return;
  if(m_userList[index].id < 0 || passwordCheck(this, m_userList[index].id))
  {
    session->setUserId(m_userList[index].id);
  }
  else
  {
    int index = 0;
    int size = m_userList.size();
    for(int i = 0; i < size; i++)
      if(m_userList[i].id == session->session().userId)
      {
        index = i;
        break;
      }
    ui->userCombo->setCurrentIndex(index);
  }
}
/*----------------------------------------------------------------------------*/
void SessionDialog::onManageUsers()
{
  if(!passwordCheck(this, SuperUserId))
    return;
  UserManageDialog dialog;
  dialog.exec();
}
/*----------------------------------------------------------------------------*/
void SessionDialog::onAccepted()
{
  DDAProfile profile = config->profile();
  profile.gostIndex = ui->gostCombo->currentIndex();
  profile.meshIndex = ui->meshCombo->currentIndex();
  profile.giftCount = ui->giftCountSpin->value();
  config->setProfile(profile);

  DDASession s = session->session();
  s.gostIndex = ui->gostCombo->currentIndex();
  s.meshIndex = ui->meshCombo->currentIndex();
  s.lot = ui->lotEdit->text();
  s.userId = m_userList[ui->userCombo->currentIndex()].id;
  if(s.id != InvalidId)
  {
    s.mark = ui->markEdit->text();
    database->modifySession(s);
    session->setSession(s);
  }
  else
  {
    s.giftCount = ui->giftCountSpin->value();
    s.id = database->sessionAdd(s);
    session->setSession(s);
  }
}
/*----------------------------------------------------------------------------*/
void SessionDialog::onUsersChanged()
{
  m_userList = database->userList();
  int size = m_userList.size();
  ui->userCombo->clear();
  for(int i = 0; i < size; i++)
    ui->userCombo->addItem(m_userList[i].name);

  int userIndex = 0;
  for(int i = 0; i < size; i++)
    if(session->session().userId == m_userList[i].id)
    {
      userIndex = i;
      break;
    }
  ui->userCombo->setCurrentIndex(userIndex);
}
/*----------------------------------------------------------------------------*/
