#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextDocument>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QtDebug>
#include <QFileDialog>
#include <QPrintDialog>
/*----------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(onActionPrint()));
  connect(ui->actionPrint_preview, SIGNAL(triggered()), this, SLOT(onActionPrintPreview()));
  connect(ui->actionExport_to_pdf, SIGNAL(triggered()), this, SLOT(onActionExportToPdf()));
  connect(ui->actionE_xit, SIGNAL(triggered()), this, SLOT(close()));

  m_printer = new QPrinter();
  m_printer->setPaperSize(QPrinter::A4);
  m_printer->setOrientation(QPrinter::Portrait);
  m_printer->setPageMargins(20.0,10.0,10.0,10.0,QPrinter::Millimeter);

  ui->webView->load(QUrl::fromLocalFile("/home/oleg/work/dda/html/templ.html"));

}
/*----------------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
  delete ui;
  delete m_printer;
}
/*----------------------------------------------------------------------------*/
void MainWindow::onActionPrint()
{
  QPrintDialog dialog(m_printer, this);
  if(dialog.exec() ==  QDialog::Accepted)
  {
    ui->webView->print(m_printer);
  }
}
/*----------------------------------------------------------------------------*/
void MainWindow::onActionPrintPreview()
{
  QPrintPreviewDialog printDialog(m_printer);
  connect(&printDialog, SIGNAL(paintRequested(QPrinter *)), ui->webView, SLOT(print(QPrinter *)));
  printDialog.exec();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onActionExportToPdf()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                             "",
                             tr("Pdf files (*.pdf)"));
  if(fileName.isEmpty())
    return;
  QPrinter printer;
  printer.setPaperSize(QPrinter::A4);
  printer.setOrientation(QPrinter::Portrait);
  printer.setPageMargins(20.0,10.0,10.0,10.0,QPrinter::Millimeter);
  printer.setOutputFileName(fileName);
  printer.setOutputFormat(QPrinter::PdfFormat);
  ui->webView->print(&printer);
  printer.newPage();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onActionExit()
{
}
/*----------------------------------------------------------------------------*/
