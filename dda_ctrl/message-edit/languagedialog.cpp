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

  ui->langCombo->addItem("");
  ui->langCombo->addItems(tmp);

  tmp = m_countryList;
  tmp.sort();
  ui->countryCombo->addItem("");
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
  if(!ui->localeEdit->hasFocus())
    ui->localeEdit->setText(m_lang);
}
/*----------------------------------------------------------------------------*/
void LanguageDialog::onCountryChanged(int)
{
  int lang = m_langList.indexOf(ui->langCombo->currentText());
  int cntr = m_countryList.indexOf(ui->countryCombo->currentText());
  m_lang = m_messageFile->mkLocale(lang, cntr);
  if(!ui->localeEdit->hasFocus())
    ui->localeEdit->setText(m_lang);
}
/*----------------------------------------------------------------------------*/
void LanguageDialog::setPrompt(const QString& p)
{
  ui->promptLabel->setText(p);
}
/*----------------------------------------------------------------------------*/
void LanguageDialog::onLocaleChanged(QString txt)
{
  QString lang, country;
  int p = txt.indexOf('_');
  if(p >= 0)
  {
    lang = m_messageFile->language(txt.left(p));
    country = m_messageFile->country(txt.mid(p + 1));
  }
  else
    lang = m_messageFile->language(txt);

  int size = ui->langCombo->count();
  for(int i = 0; i < size; i++)
  {
    if(ui->langCombo->itemText(i) == lang)
    {
      ui->langCombo->setCurrentIndex(i);
      break;
    }
  }

  if(!country.isEmpty())
  {
    size = ui->countryCombo->count();
    for(int i = 0; i < size; i++)
    {
      if(ui->countryCombo->itemText(i) == country)
      {
        ui->countryCombo->setCurrentIndex(i);
        break;
      }
    }
  }
  m_lang = txt;
}
/*----------------------------------------------------------------------------*/
