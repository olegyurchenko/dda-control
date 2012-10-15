/*----------------------------------------------------------------------------*/
/**
* @pkg qmlprocessing
*/
/**
* QML data processing module.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 11.10.2012  8:37:35<br>
* @pkgdoc qmlprocessing
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "qmlprocessing.h"
/*----------------------------------------------------------------------------*/
#include <qdeclarative.h>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QtDebug>
#include <session.h>
#include <database.h>
#include <graphplotter.h>
#include <histogrammplotter.h>
#include <curveplotter.h>
/*----------------------------------------------------------------------------*/
DDAProcessing *processing = NULL;
/*----------------------------------------------------------------------------*/
DDAProcessing :: DDAProcessing(QObject *parent)
  : QObject(parent)
{
  qmlRegisterType<DDAExtension>("DDA", 1, 0, "DDAExtension");
  //qmlRegisterType<DDAMeasureSession>("DDA", 1, 0, "DDAMeasureSession");
  qmlRegisterUncreatableType<DDADatabase>("DDA", 1, 0, "DDADatabase", "Unable to create DDA object from QML");
  qmlRegisterUncreatableType<DDAMeasureSession>("DDA", 1, 0, "DDAMeasureSession", "Unable to create DDA object from QML");
  qmlRegisterType<GraphPlotter>("DDA", 1, 0, "GraphModel");
  qmlRegisterType<HistogrammPlotter>("DDA", 1, 0, "HistogrammModel");
  qmlRegisterType<CurvePlotter>("DDA", 1, 0, "CurveModel");
  qmlRegisterType<TableModel>("DDA", 1, 0, "TableModel");
  qmlRegisterUncreatableType<DDAProcessing>("DDA", 1, 0, "DataModel", "Unable to create DDA object from QML");
  qmlRegisterUncreatableType<AxisPlotter>("DDA", 1, 0, "AxisPlotter", "Unable to create DDA object from QML");
  qmlRegisterUncreatableType<Axis>("DDA", 1, 0, "Axis", "Unable to create DDA object from QML");


  m_engine = new QDeclarativeEngine(this);
  m_engine->setBaseUrl(QUrl::fromLocalFile(QDir::currentPath()));
  //m_engine->setBaseUrl(QUrl::fromLocalFile(qApp->applicationDirPath()));


  //m_engine->addImportPath(QUrl::fromLocalFile(QDir::currentPath()).toString());
  QStringList lst;
  lst << QDir::currentPath();
  lst << qApp->applicationDirPath();
  m_engine->setImportPathList(lst);

  m_component = NULL;

  m_engine->rootContext()->setContextProperty("currentDir", "file://" + QDir::currentPath());
  m_engine->rootContext()->setContextProperty("database", database);
  m_engine->rootContext()->setContextProperty("model", this);
  clrError();
}
/*----------------------------------------------------------------------------*/
bool DDAProcessing :: open(DDAMeasureSession *session, Role role)
{
  if(m_component != NULL)
    delete m_component;

  QFile f(":/qml/processing.qml");
  if(!f.open(QIODevice::ReadOnly))
  {
    m_isError = true;
    setError(tr("Error open %1").arg("processing.qml"));
    return false;
  }
  QString qml = f.readAll();

  m_session = session;
  m_engine->rootContext()->setContextProperty("session", m_session);
  m_component = new QDeclarativeComponent(m_engine);
  //m_component->loadUrl(QUrl("qrc:/qml/processing.qml"));
  m_component->setData(qml.toUtf8().constData(), QUrl::fromLocalFile(QDir::currentPath() + "/processing.qml"));

  if(m_component->isError())
  {
    setError(m_component->errorString());
    return false;
  }

  m_qml = qobject_cast<DDAExtension *>(m_component->create());
  if(m_qml != NULL)
  {
    const QMetaObject *us = m_qml->metaObject();
    for (int idx = DDAExtension::staticMetaObject.methodCount(); idx < us->methodCount(); ++idx)
    {
      QMetaMethod mm = us->method(idx);

      if (mm.methodType() != QMetaMethod::Slot)
        continue;

      QString slt = QMetaObject::normalizedSignature(mm.signature());
      qDebug() << "Object have slot:" << slt << " and type " << mm.typeName();
      //methodMap[slt.section('(', 0, 0)] = idx;
    }

    clrError();
    QVariant ret;
    if(!m_qml->metaObject()->invokeMethod(m_qml, "modelInit", Qt::DirectConnection, Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, role)))
    {
      setError(tr("Method '%1' not found").arg("modelInit"));
    }
    else
    {
      if(ret.type() == QVariant::Map)
      {
        QVariantMap m = ret.toMap();
        if(m["lineNumber"].toInt()) //Runtime error
        {
          char buffer[1024];
          snprintf(buffer, sizeof(buffer),
                   "%s:%d:%s",
                   m["fileName"].toString().toAscii().constData(),
                   m["lineNumber"].toInt(),
                   m["message"].toString().toAscii().constData()
                   );
          qDebug() << buffer;
          setError(buffer);
          return false;
        }
      }
    }
  }
  return true;
}
/*----------------------------------------------------------------------------*/
void DDAProcessing :: addModel(QObject *obj)
{
  if(qobject_cast<AxisPlotter *>(obj)
     || qobject_cast<QAbstractTableModel *>(obj))
    emit modelAdded(obj);
}
/*----------------------------------------------------------------------------*/
void DDAProcessing :: update(Role role)
{
  QVariantMap data;
  data["id"] = m_session->session().id;
  data["deviceSerial"] = m_session->session().deviceSerial;
  data["userId"] = m_session->session().userId;
  data["userName"] = database->userName(m_session->session().userId);
  data["start"] = m_session->session().start;
  data["end"] = m_session->session().end;
  data["lot"] = m_session->session().lot;
  data["statndardIndex"] = m_session->session().standard;
  data["standard"] = database->standardList()[m_session->session().standard];
  data["gritIndex"] = m_session->session().gritIndex;
  data["grit"] = database->gritList(m_session->session().standard)[m_session->session().gritIndex];
  data["mark"] = m_session->session().mark;

  QVariantList lst;
  int size = m_session->measureList().size();
  for(int i = 0; i < size; i++)
  {
    DDAMeasure meas = m_session->measureList().at(i);
    QVariantMap m;
    m["id"] = meas.id;
    m["ignored"] = meas.ignored;
    m["size"] = meas.size;
    m["elapsed"] = meas.elapsed;
    m["strength"] = meas.strenght;
    lst.append(m);
  }
  data["measures"] = lst;


  clrError();
  QVariant ret;
  if(m_qml && !m_qml->metaObject()->invokeMethod(m_qml, "modelUpdate", Qt::DirectConnection, Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, data), Q_ARG(QVariant, role)))
  {
    setError(tr("Method '%1' not found").arg("modelUpdate"));
  }
  else
  if(ret.type() == QVariant::Map)
  {
    QVariantMap m = ret.toMap();
    if(m["lineNumber"].toInt()) //Runtime error
    {
      char buffer[1024];
      snprintf(buffer, sizeof(buffer),
               "%s:%d:%s",
               m["fileName"].toString().toAscii().constData(),
               m["lineNumber"].toInt(),
               m["message"].toString().toAscii().constData()
               );
      qDebug() << buffer;
      setError(buffer);
    }
  }
}
/*----------------------------------------------------------------------------*/
QObject *DDAProcessing :: newCurveModel()
{
  QObject *obj = new CurvePlotter(this);
  addModel(obj);
  return obj;
}
/*----------------------------------------------------------------------------*/
QObject *DDAProcessing :: newHistogrammModel()
{
  QObject *obj = new HistogrammPlotter(this);
  addModel(obj);
  return obj;
}
/*----------------------------------------------------------------------------*/
QObject *DDAProcessing :: newGraphModel()
{
  QObject *obj = new GraphPlotter(this);
  addModel(obj);
  return obj;
}
/*----------------------------------------------------------------------------*/
QObject *DDAProcessing :: newTableModel()
{
 QObject *obj = new TableModel(this);
 addModel(obj);
 return obj;
}
/*----------------------------------------------------------------------------*/
//Table model
/*----------------------------------------------------------------------------*/
TableModel :: TableModel(QObject *parent)
  :QAbstractTableModel(parent)
{
  m_columnCount = 1;
  m_rowCount = 1;
}
/*----------------------------------------------------------------------------*/
QVariant TableModel :: data(const QModelIndex &index, int role) const
{
  int col = index.column();
  int row = index.row();
  if(row < 0 || row >= m_rowCount || col < 0 || col >= m_columnCount)
    return QVariant();

  QString key = QString("data.row(%1).col(%2).role(%3)").arg(row).arg(col).arg(role);
  QVariantMap::const_iterator it = m_data.find(key);
  if(it != m_data.end())
    return it.value();

  return QVariant();
}
/*----------------------------------------------------------------------------*/
Qt::ItemFlags TableModel :: flags(const QModelIndex &) const
{
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
/*----------------------------------------------------------------------------*/
QVariant TableModel :: headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role == Qt::DisplayRole)
 {
    QString key = QString("header(%1, %2)").arg(orientation).arg(section);
    QVariantMap::const_iterator it = m_data.find(key);
    if(it != m_data.end())
      return it.value();
  }

  return QVariant();
}
/*----------------------------------------------------------------------------*/
int TableModel :: rowCount(const QModelIndex &) const
{
  return m_rowCount;
}
/*----------------------------------------------------------------------------*/
int TableModel :: columnCount(const QModelIndex &) const
{
  return m_columnCount;
}
/*----------------------------------------------------------------------------*/
void TableModel :: clear()
{
  m_data.clear();
}
/*----------------------------------------------------------------------------*/
void TableModel :: update()
{
  reset();
  emit dataChanged(index(0,0), index(m_rowCount, m_columnCount));
}
/*----------------------------------------------------------------------------*/
QVariant TableModel :: data(int row, int col)
{
  return data(index(row, col), Qt::DisplayRole);
}
/*----------------------------------------------------------------------------*/
void TableModel :: setData(int row, int col, const QVariant &val, int role)
{
  if(row < 0 || row >= m_rowCount || col < 0 || col >= m_columnCount)
    return;

  QString key = QString("data.row(%1).col(%2).role(%3)").arg(row).arg(col).arg(role);
  m_data[key] = val;
  emit dataChanged(index(row, col), index(row, col));
}
/*----------------------------------------------------------------------------*/
void TableModel :: setHeaderData(int section, int orientation, const QVariant &value)
{
  QString key = QString("header(%1, %2)").arg(orientation).arg(section);
  m_data[key] = value;
}
/*----------------------------------------------------------------------------*/
