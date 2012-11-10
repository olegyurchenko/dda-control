#ifndef LANGUAGEDIALOG_H
#define LANGUAGEDIALOG_H

#include <QDialog>
#include <message_file.h>
#include <QStringList>
/*----------------------------------------------------------------------------*/
namespace Ui {
class LanguageDialog;
}

/*----------------------------------------------------------------------------*/
class LanguageDialog : public QDialog
{
  Q_OBJECT
protected:
  MessageFile *m_messageFile;
  QString m_lang;
  QStringList m_countryList;
  QStringList m_langList;
public:
  explicit LanguageDialog(QWidget *parent, MessageFile *m);
  ~LanguageDialog();
  void setPrompt(const QString& p);
  QString lang() {return m_lang;}

private:
  Ui::LanguageDialog *ui;
protected slots:
  void onLangChanged(int);
  void onCountryChanged(int);
  void onLocaleChanged(QString);
};
/*----------------------------------------------------------------------------*/

#endif // LANGUAGEDIALOG_H
