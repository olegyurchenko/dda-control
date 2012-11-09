#include "languagedialog.h"
#include "ui_languagedialog.h"

/*----------------------------------------------------------------------------*/
LanguageDialog::LanguageDialog(QWidget *parent, MessageFile *m) :
  QDialog(parent),
  ui(new Ui::LanguageDialog)
{
  ui->setupUi(this);
  m_messageFile = m;

  m_langList = m_messageFile->langsList();
  m_countryList = m_messageFile->countryList();

  QStringList tmp = m_langList;
  tmp.sort();

  ui->langCombo->addItems(tmp);

  tmp = m_countryList;
  tmp.sort();
  ui->countryCombo->addItems(tmp);
}
/*----------------------------------------------------------------------------*/
LanguageDialog::~LanguageDialog()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void LanguageDialog::onLangChanged(int)
{
  int lang = m_langList.indexOf(ui->langCombo->currentText());
  int cntr = m_countryList.indexOf(ui->countryCombo->currentText());
  m_lang = m_messageFile->mkLocale(lang, cntr);
  ui->localeEdit->setText(m_lang);
}
/*----------------------------------------------------------------------------*/
void LanguageDialog::onCountryChanged(int)
{
  int lang = m_langList.indexOf(ui->langCombo->currentText());
  int cntr = m_countryList.indexOf(ui->countryCombo->currentText());
  m_lang = m_messageFile->mkLocale(lang, cntr);
  ui->localeEdit->setText(m_lang);
}
/*----------------------------------------------------------------------------*/
void LanguageDialog::setPrompt(const QString& p)
{
  ui->promptLabel->setText(p);
}
/*----------------------------------------------------------------------------*/
