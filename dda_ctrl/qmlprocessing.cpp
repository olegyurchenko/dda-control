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
  clrError();
}
/*----------------------------------------------------------------------------*/
bool DDAProcessing :: open(DDAMeasureSession *session)
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
  }
  return true;
}
/*----------------------------------------------------------------------------*/
