#include "sessiondialog.h"
#include "ui_sessiondialog.h"
#include <configuration.h>
#include <session.h>
#include <controller.h>
#include <usermanagedialog.h>
/*----------------------------------------------------------------------------*/
SessionDialog::SessionDialog(QWidget *parent, DDAMeasureSession *session) :
  QDialog(parent),
  ui(new Ui::SessionDialog)
{
  ui->setupUi(this);

  if(session != NULL)
    m_session = session;
  else
    m_session = ::session;

  connect(database, SIGNAL(userChanged(int)), this, SLOT(onUsersChanged()));

  onUsersChanged();
  //ui->userCombo->setCurrentIndex(0);

  ui->productCombo->addItems(database->productList());
  ui->productCombo->setEditText("");
  ui->markCombo->addItems(database->markList());
  ui->markCombo->setEditText("");

  ui->standardCombo->addItems(database->standardList());
  ui->standardCombo->setCurrentIndex(config->profile().standardIndex);

  ui->gritCombo->addItems(database->gritList(config->profile().standardIndex));
  ui->gritCombo->setCurrentIndex(config->profile().gritIndex);

  ui->particlesSpin->setValue(config->profile().particles);

  if(m_session->session().id != InvalidId)
  {
    ui->productCombo->setEditText(m_session->session().product);
    ui->standardCombo->setCurrentIndex(m_session->session().standard);
    ui->gritCombo->setCurrentIndex(m_session->session().gritIndex);
    ui->lotEdit->setText(m_session->session().lot);
    ui->markCombo->setEnabled(true);
    ui->markCombo->setEditText(m_session->session().mark);
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

  if(m_userList[index].id == m_session->session().userId)
    return;
  if(m_userList[index].id < 0 || passwordCheck(this, m_userList[index].id))
  {
    m_session->setUserId(m_userList[index].id);
  }
  else
  {
    int index = 0;
    int size = m_userList.size();
    for(int i = 0; i < size; i++)
      if(m_userList[i].id == m_session->session().userId)
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

  DDASession s = m_session->session();
  s.gritIndex = ui->gritCombo->currentIndex();
  s.standard = ui->standardCombo->currentIndex();
  s.lot = ui->lotEdit->text();
  s.userId = m_userList[ui->userCombo->currentIndex()].id;
  s.product = ui->productCombo->currentText();
  if(s.id != InvalidId)
  {
    s.mark = ui->markCombo->currentText();
    database->modifySession(s);
    m_session->setSession(s);
  }
  else
  {
    s.particles = ui->particlesSpin->value();
    s.id = database->sessionAdd(s);
    m_session->setSession(s);
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
    if(m_session->session().userId == m_userList[i].id)
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
