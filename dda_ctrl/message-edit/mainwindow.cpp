#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <message_file.h>
/*----------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  m_messageFile = new MessageFile;
}
/*----------------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
  delete ui;
  delete m_messageFile;
}
/*----------------------------------------------------------------------------*/
void MainWindow::updateMessages()
{
  m_langList = m_messageFile->langList();
  m_sourceList = m_messageFile->sourceList();

  ui->sourceList->clear();
  ui->sourceList->addItems(m_sourceList);
  ui->langCombo->clear();
  ui->langCombo->addItems(m_langList);
  ui->translateEdit->setText("");
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileNew()
{
  delete m_messageFile;
  m_messageFile = new MessageFile;
  m_fileName.clear();
  updateMessages();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileOpen()
{
  QString fileName = QFileDialog::getOpenFileName(this,
       tr("Open File"), "", tr("XML Files (*.xml);;All files (*)"));
  if(fileName.isEmpty())
    return;

  delete m_messageFile;
  m_messageFile = new MessageFile;
  if(!m_messageFile->load(fileName))
  {
    QMessageBox::critical(this, tr("Error open file"), tr("Error open file '%1': %2").arg(fileName).arg(m_messageFile->errorString()));
    return;
  }
  m_fileName = fileName;
  updateMessages();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileSave()
{
  if(m_fileName.isEmpty())
  {
    QString fileName = QFileDialog::getSaveFileName(this,
             tr("Save File"), "", tr("XML Files (*.xml);;All files (*)"));
    if(fileName.isEmpty())
      return;

    m_fileName = fileName;
  }
  if(!m_messageFile->save(m_fileName))
  {
    QMessageBox::critical(this, tr("Error save file"), tr("Error save file '%1': %2").arg(m_fileName).arg(m_messageFile->errorString()));
    return;
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileSaveAs()
{
  m_fileName.clear();
  onFileSave();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileExit()
{
  close();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onLangChanged(int)
{
  if(ui->sourceList->currentRow() >= 0)
  {
    QString source = m_sourceList[ui->sourceList->currentRow()];
    QString lang = ui->langCombo->currentText();
    ui->translateEdit->setText(m_messageFile->message(source, lang));
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onTranslateChanged(QString txt)
{
  if(ui->sourceList->currentRow() >= 0)
  {
    QString source = m_sourceList[ui->sourceList->currentRow()];
    QString lang = ui->langCombo->currentText();
    m_messageFile->setMessage(source, txt, lang);
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onSourceChanged(int)
{
  if(ui->sourceList->currentRow() >= 0)
  {
    QString source = m_sourceList[ui->sourceList->currentRow()];
    QString lang = ui->langCombo->currentText();
    ui->translateEdit->setText(m_messageFile->message(source, lang));

    ui->translateList->clear();
    int size = m_langList.size();
    for(int i = 0; i < size; i++)
      ui->translateList->addItem(m_messageFile->message(source, m_langList[i]));
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onSearchTextChanged(QString txt)
{
  if(ui->sourceList->currentRow() < 0)
    return;


  int index = ui->sourceList->currentRow();
  int size = m_sourceList.size();
  for(int i = index; i < size; i++)
  {
    if(m_sourceList[i].indexOf(txt, 0, Qt::CaseInsensitive) != -1)
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onSourceListDblClick()
{
  if(ui->sourceList->currentRow() < 0)
    return;

  int row = ui->translateList->currentRow();
  QString source = m_sourceList[ui->sourceList->currentRow()];
  QString lang = m_langList[row];
  ui->translateEdit->setText(m_messageFile->message(source, lang));

}
/*----------------------------------------------------------------------------*/
