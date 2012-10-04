#include "usermanagedialog.h"
#include "ui_usermanagedialog.h"
#include <QInputDialog>
#include <QIcon>
#include <QListWidgetItem>
#include <QMessageBox>
#include <newpassworddialog.h>
/*----------------------------------------------------------------------------*/
UserManageDialog::UserManageDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::UserManageDialog)
{
  ui->setupUi(this);
  onUsersChanged();
  connect(database, SIGNAL(userChanged(int)), this, SLOT(onUsersChanged()));
}
/*----------------------------------------------------------------------------*/
UserManageDialog::~UserManageDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void UserManageDialog::onUserAdd()
{
  bool ok;
  QString text = QInputDialog::getText(this, tr("Enter user name"),
                                       tr("User name:"), QLineEdit::Normal,
                                       "", &ok);
  if (!ok && text.isEmpty())
    return;

  database->userAdd(text);
  ui->userList->setCurrentRow(ui->userList->count() - 1);
}
/*----------------------------------------------------------------------------*/
void UserManageDialog::onUserDel()
{
  int index = ui->userList->currentRow();
  int oldRow = index;
  int id = -100;
  QString name;
  int size = m_userList.size();
  for(int i = 0; i < size; i++)
  {
    if(m_userList[i].id > 0)
    {
      if(!index)
      {
        id = m_userList[i].id;
        name = m_userList[i].name;
        break;
      }
      index --;
    }
  }
  if(QMessageBox::question(this, tr("Delete user"),
                           tr("Are you shure to delte user '%1' ?").arg(name),
                           QMessageBox::Yes | QMessageBox::No,
                           QMessageBox::No
                           ) != QMessageBox::Yes)
    return;

  database->userDel(id);
  ui->userList->setCurrentRow(oldRow - 1);
}
/*----------------------------------------------------------------------------*/
void UserManageDialog::onUserPassword()
{
  int index = ui->userList->currentRow();
  int oldRow = index;
  int id = -100;
  QString name;
  int size = m_userList.size();
  for(int i = 0; i < size; i++)
  {
    if(m_userList[i].id > 0)
    {
      if(!index)
      {
        id = m_userList[i].id;
        name = m_userList[i].name;
        break;
      }
      index --;
    }
  }
  NewPasswordDialog dlg;
  if(dlg.exec() == QDialog::Accepted)
  database->setPassword(id, dlg.password());
  ui->userList->setCurrentRow(oldRow);
}
/*----------------------------------------------------------------------------*/
void UserManageDialog::onUsersChanged()
{
  ui->userList->clear();
  m_userList = database->userList();
  int size = m_userList.size();
  for(int i = 0; i < size; i++)
  {
    if(m_userList[i].id > 0)
      ui->userList->addItem(new QListWidgetItem(QIcon(":/user-identity.png"), m_userList[i].name, ui->userList));
  }
  if(ui->userList->count())
    ui->userList->setCurrentRow(0);
  else
  {
    ui->delUserBtn->setEnabled(false);
    ui->pswrdBtn->setEnabled(false);
  }
}
/*----------------------------------------------------------------------------*/
void UserManageDialog::onSelectedUserChanged(int index)
{
  if(index >= 0)
  {
    ui->delUserBtn->setEnabled(true);
    ui->pswrdBtn->setEnabled(true);
  }
  else
  {
    ui->delUserBtn->setEnabled(false);
    ui->pswrdBtn->setEnabled(false);
  }
}
/*----------------------------------------------------------------------------*/
bool passwordCheck(QWidget *parent, int id)
{
  if(database->checkPassword(id, ""))
    return true;
  QString userName = database->userName(id);
  if(id == SuperUserId)
    userName = "";
  bool ok;
  QString text = QInputDialog::getText(parent, QObject::tr("Enter password"),
                                       QObject::tr("User %1 password:").arg(userName), QLineEdit::Password,
                                       "", &ok);
  if (!ok && text.isEmpty())
    return false;
  if(!database->checkPassword(id, text))
  {
    QMessageBox::critical(parent, QObject::tr("Error"), QObject::tr("Invalid password"));
    return false;
  }
  return true;
}
/*----------------------------------------------------------------------------*/
