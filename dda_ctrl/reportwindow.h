#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

/*----------------------------------------------------------------------------*/
namespace Ui {
class ReportDialog;
}
/*----------------------------------------------------------------------------*/
class QPrinter;
class DDAMeasureSession;
class HtmlReport;
/*----------------------------------------------------------------------------*/
class ReportDialog : public QDialog
{
  Q_OBJECT
protected:
  QPrinter *m_printer;
  DDAMeasureSession *m_session;
  HtmlReport *m_report;
public:
  explicit ReportDialog(QWidget *parent, DDAMeasureSession *session);
  ~ReportDialog();
  
private slots:
  void onActionPrint();

  void onActionPrintPreview();

  void onActionExportToPdf();

  void onActionExit();

private:
  Ui::ReportDialog *ui;
};
/*----------------------------------------------------------------------------*/

#endif // MAINWINDOW_H
