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
#include <QUrl>
#include <QImage>
#include <QTemporaryFile>
#include <QPainter>
/*----------------------------------------------------------------------------*/
HtmlReport :: HtmlReport(QObject *parent, DDAMeasureSession *session)
  : QObject(parent)
{
  m_session = session;
  m_processing = new DDAProcessing(this);
  connect(m_processing, SIGNAL(modelAdded(QObject*)), this, SLOT(onModelAdded(QObject*)));

  m_dict["currentDir"] = QUrl::fromLocalFile(QDir::currentPath());

  QDir templ = QDir::current();
  templ.setPath(templ.filePath("template"));
  m_dict["templateDir"] = QUrl::fromLocalFile(templ.absolutePath());
  m_templ = new TemplateSet(templ.filePath("report.templ"));
  m_tableCount = 0;
  m_imageCount = 0;
  m_viewCount = 0;
}
/*----------------------------------------------------------------------------*/
HtmlReport :: ~HtmlReport()
{
  delete m_templ;
  QFile *f;
  foreach(f, m_fileList)
    delete f;
}
/*----------------------------------------------------------------------------*/
void HtmlReport :: addPlotter(Plotter *model)
{
  int w = m_dict["plotWidth"].toInt();
  int h = m_dict["plotHeight"].toInt();

  if(w == 0)
    w = 500;
  if(h == 0)
    h = 400;

  QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
  image.fill(QColor("white"));

  model->paint(&image);
  QTemporaryFile *file = new QTemporaryFile(QDir::temp().filePath("reportXXXXXX.png"));
  file->setAutoRemove(true);
  image.save(file, "PNG");
  m_fileList.append(file);

  QVariantMap dict = m_dict;
  dict["fileName"] = QUrl::fromLocalFile(file->fileName());
  QString img = m_templ->getString("image", &dict);
  //qDebug() << "Image:" << img;
  m_dict[QString("image%1").arg(++m_imageCount)] = img;
  m_dict[QString("view%1").arg(++m_viewCount)] = img;
}
/*----------------------------------------------------------------------------*/
void HtmlReport :: addTable(TableModel *model)
{
  int rowCount = model->rowCount();
  int colCount = model->columnCount();

  QString content, table;
  QVariantMap dict = m_dict;
  dict["width"] = 100/colCount;

//  for(int col = 0; col < colCount; col++)
//    table += m_templ->getString("tableColumn", &dict);

  for(int col = 0; col < colCount; col++)
  {
    dict["align"] = "CENTER";
    QVariant a = model->headerData(col, Qt::Horizontal, Qt::TextAlignmentRole);
    if(a.isValid())
    {
      switch(a.toInt())
      {
      case Qt::AlignRight:
        dict["align"] = "RIGHT";
        break;
      case Qt::AlignHCenter:
        dict["align"] = "CENTER";
        break;
      default:
        break;
      }
    }

    dict["content"] = model->headerData(col, Qt::Horizontal, Qt::DisplayRole);
    content += m_templ->getString("column", &dict);
  }
  dict["content"] = content;
  table += m_templ->getString("row", &dict);

  for(int row = 0; row < rowCount; row++)
  {
    content.clear();
    for(int col = 0; col < colCount; col++)
    {
      dict["align"] = "CENTER";
      QVariant a = model->data(model->index(row, col), Qt::TextAlignmentRole);
      if(a.isValid())
      {
        switch(a.toInt())
        {
        case Qt::AlignRight:
          dict["align"] = "RIGHT";
          break;
        case Qt::AlignHCenter:
          dict["align"] = "CENTER";
          break;
        default:
          break;
        }
      }
      dict["content"] = model->data(model->index(row, col), Qt::DisplayRole);
      content += m_templ->getString("column", &dict);
    }
    dict["content"] = content;
    table += m_templ->getString("row", &dict);
  }

  dict["width"] = 80;
  dict["content"] = table;
  table = m_templ->getString("table", &dict);

  m_dict[QString("table%1").arg(++m_tableCount)] = table;
  m_dict[QString("view%1").arg(++m_viewCount)] = table;

}
/*----------------------------------------------------------------------------*/
void HtmlReport :: onModelAdded(QObject* o)
{
  m_modelList.append(o);
}
/*----------------------------------------------------------------------------*/
bool HtmlReport :: generateReport()
{
  m_processing->setDictionary(m_dict);
  if(!m_processing->open(m_session, DDAProcessing::ReportRole))
  {
    m_message = m_processing->message();
    return false;
  }
  m_processing->update(DDAProcessing::ReportRole);
  if(m_processing->isError())
  {
    m_message = m_processing->message();
    return false;
  }
  m_dict = m_processing->dictionary();

  QObject *o;
  foreach(o, m_modelList)
  {
    Plotter *p = qobject_cast<Plotter*>(o);
    if(p)
      addPlotter(p);
    TableModel *t = qobject_cast<TableModel*>(o);
    if(t)
      addTable(t);
  }

  m_content = m_templ->getString("doc", &m_dict);
  //qDebug() << m_content;
  return true;
}
/*----------------------------------------------------------------------------*/

