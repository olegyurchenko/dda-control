#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <message_file.h>
#include <languagedialog.h>
#include <QInputDialog>
#include <QCloseEvent>
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
bool MainWindow::midifiedQuestion()
{
  if(m_messageFile->isModified())
  {
    switch(QMessageBox::question(this,
                                  tr("Data modified"),
                                  tr("Data was modified. Do you whant to save message file ?"),
                                  QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard,
                                  QMessageBox::Cancel))
    {
    case QMessageBox::Save:
      onFileSave();
      return !m_messageFile->isModified();
    case QMessageBox::Discard:
      m_messageFile->setModified(false);
      return true;
    case QMessageBox::Cancel:
    default:
      return false;
    }
    return false;
  }
  return true;
}
/*----------------------------------------------------------------------------*/
void MainWindow::closeEvent(QCloseEvent *e)
{
  if(!midifiedQuestion())
  {
    e->ignore();
    return;
  }
  m_messageFile->setModified(false);
  QMainWindow::closeEvent(e);
}
/*----------------------------------------------------------------------------*/
void MainWindow::updateMessages()
{
  m_langList = m_messageFile->langList();
  m_sourceList = m_messageFile->sourceList();

  ui->sourceList->clear();
  ui->sourceList->addItems(m_sourceList);
  ui->langCombo->clear();

  int size = m_langList.size();
  for(int i = 0; i < size; i++)
    ui->langCombo->addItem(m_messageFile->locale(m_langList[i]));
  ui->translateEdit->setText("");
  ui->translateList->clear();

  ui->actionCopyLanguage->setEnabled(!m_langList.empty());
  ui->actionDeleteLanguage->setEnabled(!m_langList.empty());
  ui->actionDeleteSource->setEnabled(!m_sourceList.empty());
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileNew()
{
  if(!midifiedQuestion())
    return;
  delete m_messageFile;
  m_messageFile = new MessageFile;
  m_fileName.clear();
  updateMessages();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileOpen()
{
  if(!midifiedQuestion())
    return;
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
  if(ui->sourceList->count())
    ui->sourceList->setCurrentRow(0);
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
  if(ui->sourceList->currentRow() >= 0 && ui->langCombo->currentIndex() >= 0)
  {
    QString source = m_sourceList[ui->sourceList->currentRow()];
    QString lang = m_langList[ui->langCombo->currentIndex()];
    ui->translateEdit->setText(m_messageFile->message(source, lang));
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onTranslateChanged(QString txt)
{
  if(ui->sourceList->currentRow() >= 0 && ui->langCombo->currentIndex() >= 0)
  {
    QString source = m_sourceList[ui->sourceList->currentRow()];
    QString lang = m_langList[ui->langCombo->currentIndex()];
    if(ui->translateEdit->isModified())
    {
      m_messageFile->setMessage(source, txt, lang);
      ui->translateEdit->setModified(false);
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onSourceChanged(int)
{
  if(ui->sourceList->currentRow() >= 0 && ui->langCombo->currentIndex() >= 0)
  {
    QString source = m_sourceList[ui->sourceList->currentRow()];
    QString lang = m_langList[ui->langCombo->currentIndex()];
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
  if(txt.isEmpty())
  {
    ui->actionFindNextSource->setEnabled(false);
    ui->actionFindPreviosSouce->setEnabled(false);
    return;
  }
  else
  {
    ui->actionFindNextSource->setEnabled(true);
    ui->actionFindPreviosSouce->setEnabled(true);
  }

  int index = ui->sourceList->currentRow();
  int size = m_sourceList.size();
  if(index < 0)
    index = 0;
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
  if(ui->sourceList->currentRow() < 0 || ui->langCombo->currentIndex() < 0)
    return;

  int row = ui->translateList->currentRow();
  QString source = m_sourceList[ui->sourceList->currentRow()];
  QString lang = m_langList[row];
  QString txt = m_messageFile->message(source, lang);
  ui->translateEdit->setModified(true);
  ui->translateEdit->setText(txt);

  lang = m_langList[ui->langCombo->currentIndex()];
  m_messageFile->setMessage(source, txt, lang);

}
/*----------------------------------------------------------------------------*/
void MainWindow::onSourceSearchDown()
{
  int index = ui->sourceList->currentRow();
  if(index < 0)
    index = 0;
  QString txt = ui->searchEdit->text();
  int size = m_sourceList.size();
  for(int i = index + 1; i < size; i++)
  {
    if(m_sourceList[i].indexOf(txt, 0, Qt::CaseInsensitive) != -1)
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onSourceSearchUp()
{
  int index = ui->sourceList->currentRow();
  if(index < 0)
    index = m_sourceList.size();
  QString txt = ui->searchEdit->text();
  for(int i = index - 1; i >= 0; i--)
  {
    if(m_sourceList[i].indexOf(txt, 0, Qt::CaseInsensitive) != -1)
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onNewLanguage()
{
  LanguageDialog dlg(this, m_messageFile);
  dlg.setPrompt(tr("Select locale for create new language"));
  if(dlg.exec() == QDialog::Accepted)
  {
    m_messageFile->addLang(dlg.lang());
    updateMessages();
    m_messageFile->setModified(true);
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onCopyLanguage()
{
  LanguageDialog dlg(this, m_messageFile);
  dlg.setPrompt(tr("Select locale for copy %1 locale").arg(ui->langCombo->currentText()));
  if(dlg.exec() == QDialog::Accepted)
  {
    QString srcLang = m_langList[ui->langCombo->currentIndex()];
    QString lang = dlg.lang();
    int size = m_sourceList.size();
    for(int i = 0; i < size; i++)
    {
      QString s = m_messageFile->message(m_sourceList[i], srcLang);
      m_messageFile->setMessage(m_sourceList[i], s, lang);
    }
    updateMessages();
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onDelLanguage()
{
  QString lang = m_langList[ui->langCombo->currentIndex()];
  m_messageFile->deleteLang(lang);
  updateMessages();
  m_messageFile->setModified(false);
}
/*----------------------------------------------------------------------------*/
void MainWindow::onNextSource()
{
  int index = ui->sourceList->currentRow();
  index ++;
  if(index > m_sourceList.size())
    index = 0;
  ui->sourceList->setCurrentRow(index);
}
/*----------------------------------------------------------------------------*/
void MainWindow::onPrevSource()
{
  int index = ui->sourceList->currentRow();
  index --;
  if(index < 0)
    index = m_sourceList.size() - 1;
  ui->sourceList->setCurrentRow(index);
}
/*----------------------------------------------------------------------------*/
void MainWindow::onNextLanguage()
{
  int index = ui->langCombo->currentIndex();
  index ++;
  if(index >= m_langList.size())
    index = 0;
  ui->langCombo->setCurrentIndex(index);
}
/*----------------------------------------------------------------------------*/
void MainWindow::onPrevLanguage()
{
  int index = ui->langCombo->currentIndex();
  index --;
  if(index < 0)
    index = m_langList.size() - 1;
  ui->langCombo->setCurrentIndex(index);
}
/*----------------------------------------------------------------------------*/
void MainWindow::onNewSource()
{
  QString text = QInputDialog::getText(this, tr("New source"),
                                       tr("Source text:"));
  if(!text.isEmpty())
  {
    m_messageFile->addSource(text);
    m_sourceList.append(text);
    ui->sourceList->addItem(text);
    ui->actionDeleteSource->setEnabled(!m_sourceList.empty());
    m_messageFile->setModified(true);
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onDelSource()
{
  int index = ui->sourceList->currentRow();
  if(index >= 0)
  {
    m_messageFile->deleteSource(m_sourceList[index]);
    ui->sourceList->model()->removeRow(index);
    m_sourceList.erase(m_sourceList.begin() + index);
    ui->actionDeleteSource->setEnabled(!m_sourceList.empty());
    m_messageFile->setModified(true);
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onSearchTranslatedChanged(QString txt)
{
  if(ui->sourceList->currentRow() < 0 || ui->langCombo->currentIndex() < 0)
    return;

  if(txt.isEmpty())
  {
    ui->actionSearchNextTranslation->setEnabled(false);
    ui->actionSearchPreviosTranslation->setEnabled(false);
    return;
  }
  else
  {
    ui->actionSearchNextTranslation->setEnabled(true);
    ui->actionSearchPreviosTranslation->setEnabled(true);
  }

  int index = ui->sourceList->currentRow();
  int size = m_sourceList.size();
  for(int i = index; i < size; i++)
  {
    QString trans = m_messageFile->message(m_sourceList[i], m_langList[ui->langCombo->currentIndex()]);
    if(trans.indexOf(txt, 0, Qt::CaseInsensitive) != -1)
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onTranslatedSearchDown()
{
  if(ui->langCombo->currentIndex() < 0)
    return;

  int index = ui->sourceList->currentRow();
  int size = m_sourceList.size();
  if(index >= size - 1)
    index = -1;

  QString txt = ui->translateSearchEdit->text();
  for(int i = index + 1; i < size; i++)
  {
    QString trans = m_messageFile->message(m_sourceList[i], m_langList[ui->langCombo->currentIndex()]);
    if(trans.indexOf(txt, 0, Qt::CaseInsensitive) != -1)
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onTranslatedSearchUp()
{
  if(ui->langCombo->currentIndex() < 0)
    return;

  int index = ui->sourceList->currentRow();
  int size = m_sourceList.size();

  if(index == 0)
    index = size;

  QString txt = ui->translateSearchEdit->text();
  for(int i = index - 1; i >= 0; i--)
  {
    QString trans = m_messageFile->message(m_sourceList[i], m_langList[ui->langCombo->currentIndex()]);
    if(trans.indexOf(txt, 0, Qt::CaseInsensitive) != -1)
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onNextUntranslated()
{
  if(ui->langCombo->currentIndex() < 0)
    return;

  int index = ui->sourceList->currentRow();
  int size = m_sourceList.size();
  if(index >= size - 1)
    index = -1;

  for(int i = index + 1; i < size; i++)
  {
    QString trans = m_messageFile->message(m_sourceList[i], m_langList[ui->langCombo->currentIndex()]);
    if(trans.isEmpty())
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onPreviosUntranslated()
{
  if(ui->langCombo->currentIndex() < 0)
    return;

  int index = ui->sourceList->currentRow();
  int size = m_sourceList.size();

  if(index == 0)
    index = size;

  QString txt = ui->translateSearchEdit->text();
  for(int i = index - 1; i >= 0; i--)
  {
    QString trans = m_messageFile->message(m_sourceList[i], m_langList[ui->langCombo->currentIndex()]);
    if(trans.isEmpty())
    {
      ui->sourceList->setCurrentRow(i);
      break;
    }
  }
}
/*----------------------------------------------------------------------------*/
