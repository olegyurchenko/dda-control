#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QMap>
/*----------------------------------------------------------------------------*/
namespace Ui {
    class OptionsDialog;
}
/*----------------------------------------------------------------------------*/
class OptionsDialog : public QDialog
{
  Q_OBJECT
protected:
  QMap<QString, QString> langMap;
  QMap<QString, QString> countryMap;
public:
  explicit OptionsDialog(QWidget *parent = 0);
  ~OptionsDialog();

private:
  Ui::OptionsDialog *ui;

protected slots:
  void onAccept();
  void onOpenFile();
};
/*----------------------------------------------------------------------------*/
#endif // OPTIONSDIALOG_H
