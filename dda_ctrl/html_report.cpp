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
#include "html_report.h"
#include <curveplotter.h>
#include <histogrammplotter.h>
#include <graphplotter.h>
#include <qmlprocessing.h>
#include <template_file.h>
#include <QDir>
#include <QtDebug>
/*----------------------------------------------------------------------------*/
HtmlReport :: HtmlReport(QObject *parent, DDAMeasureSession *session)
  : QObject(parent)
{
  m_session = session;
  m_processing = new DDAProcessing(this);
  connect(m_processing, SIGNAL(modelAdded(QObject*)), this, SLOT(onModelAdded(QObject*)));

  QDir templ = QDir::current();
  templ.setPath(templ.filePath("template"));
  m_templ = new TemplateSet(templ.filePath("report.templ"));
}
/*----------------------------------------------------------------------------*/
HtmlReport :: ~HtmlReport()
{
  delete m_templ;
}
/*----------------------------------------------------------------------------*/
void HtmlReport :: addGraph(GraphPlotter *model)
{
}
/*----------------------------------------------------------------------------*/
void HtmlReport :: addHistogramm(HistogrammPlotter *model)
{
}
/*----------------------------------------------------------------------------*/
void HtmlReport :: addCurve(CurvePlotter *model)
{
}
/*----------------------------------------------------------------------------*/
void HtmlReport :: addTable(TableModel *model)
{
  int rowCount = model->rowCount();
  int colCount = model->columnCount();

  QString content, table;
  QVariantHash dict;
  dict["width"] = 100/colCount;

  for(int col = 0; col < colCount; col++)
    table += m_templ->getString("tableColumn", &dict);

  for(int row = 0; row < rowCount; row++)
  {
    content.clear();
    for(int col = 0; col < colCount; col++)
    {
      dict["align"] = "CENTER";
      dict["content"] = model->data(row, col);
      content += m_templ->getString("column", &dict);
    }
    dict["content"] = content;
    table += m_templ->getString("row", &dict);
  }

  dict["width"] = 80;
  dict["content"] = table;
  table = m_templ->getString("table", &dict);
  m_templ->setString("content", m_templ->getString("content", NULL) + table);
}
/*----------------------------------------------------------------------------*/
void HtmlReport :: onModelAdded(QObject* o)
{
  m_modelList.append(o);
}
/*----------------------------------------------------------------------------*/
bool HtmlReport :: generateReport()
{
  if(!m_processing->open(m_session))
  {
    m_message = m_processing->message();
    return false;
  }
  m_processing->update();
  if(m_processing->isError())
  {
    m_message = m_processing->message();
    return false;
  }

  QObject *o;
  foreach(o, m_modelList)
  {
    GraphPlotter *g = qobject_cast<GraphPlotter*>(o);
    if(g)
      addGraph(g);
    HistogrammPlotter *h = qobject_cast<HistogrammPlotter*>(o);
    if(h)
      addHistogramm(h);
    CurvePlotter *c = qobject_cast<CurvePlotter*>(o);
    if(c)
      addCurve(c);
    TableModel *t = qobject_cast<TableModel*>(o);
    if(t)
      addTable(t);
  }

  m_context = m_templ->getString("doc", &m_dict);
  qDebug() << m_context;
  return true;
}
/*----------------------------------------------------------------------------*/

