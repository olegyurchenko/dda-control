/*----------------------------------------------------------------------------*/
/**
* @pkg html_report
*/
/**
* HTML report.
*
* Generate HTML report from users modesl.<br>
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 14.10.2012 13:32:44<br>
* @pkgdoc html_report
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef HTML_REPORT_H_1350210764
#define HTML_REPORT_H_1350210764
/*----------------------------------------------------------------------------*/
#include <QObject>
#include <QVariantMap>
#include <QList>
#include <QFile>

class DDAProcessing;
class DDAMeasureSession;
class Plotter;
class TableModel;
class TemplateSet;
class HtmlReport :public QObject
{
  Q_OBJECT
protected:
  DDAProcessing *m_processing;
  DDAMeasureSession *m_session;
  QString m_content;
  QString m_message;
  QString m_templDir;
  QString m_imageDir;
  TemplateSet *m_templ;
  QVariantMap m_dict;
  QList<QObject *> m_modelList;
  QList<QFile*> m_fileList;
  int m_tableCount;
  int m_imageCount;
  int m_viewCount;

protected:
  void addPlotter(Plotter *model);
  void addTable(TableModel *model);

public:
  HtmlReport(QObject *parent, DDAMeasureSession *session);
  ~HtmlReport();
  bool generateReport();
  QString message() {return m_message;}
  const QString& content() {return m_content;}
protected slots:
  void onModelAdded(QObject*);
};
/*----------------------------------------------------------------------------*/
#endif /*HTML_REPORT_H_1350210764*/

