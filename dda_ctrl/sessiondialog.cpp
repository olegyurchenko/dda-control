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

  ui->standardCombo->addItems(database->standardList());
  ui->standardCombo->setCurrentIndex(config->profile().standardIndex);

  ui->gritCombo->addItems(database->gritList(config->profile().standardIndex));
  ui->gritCombo->setCurrentIndex(config->profile().gritIndex);

  ui->particlesSpin->setValue(config->profile().particles);

  if(session->session().id != InvalidId)
  {
    ui->standardCombo->setCurrentIndex(session->session().standard);
    ui->gritCombo->setCurrentIndex(session->session().gritIndex);
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
  profile.gritIndex = ui->gritCombo->currentIndex();
  profile.standardIndex = ui->standardCombo->currentIndex();
  profile.particles = ui->particlesSpin->value();
  config->setProfile(profile);

  DDASession s = session->session();
  s.gritIndex = ui->gritCombo->currentIndex();
  s.standard = ui->standardCombo->currentIndex();
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
    s.particles = ui->particlesSpin->value();
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
void SessionDialog::onStandardChanged(int index)
{
  int cur = ui->gritCombo->currentIndex();
  ui->gritCombo->clear();
  ui->gritCombo->addItems(database->gritList(index));
  ui->gritCombo->setCurrentIndex(cur);
}
/*----------------------------------------------------------------------------*/
