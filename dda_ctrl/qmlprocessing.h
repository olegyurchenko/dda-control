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
#ifndef QMLPROCESSING_H_1349933855
#define QMLPROCESSING_H_1349933855
/*----------------------------------------------------------------------------*/
#include <QObject>

class DDAExtension : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString message READ message WRITE setMessage)
  Q_PROPERTY(QString version READ version WRITE setVersion)
  Q_PROPERTY(int revision READ revision WRITE setRevision)
protected:
  QString m_message;
  QString m_version;
  int m_revision;
public:
  DDAExtension(QObject *parent = 0) : QObject(parent)
  {
    m_revision = 100;
  }

  QString message() {return m_message;}
  QString version() {return m_version;}
  int revision() {return m_revision;}

  void setMessage(const QString& msg) {m_message = msg;}
  void setVersion(const QString& v) {m_version = v;}
  void setRevision(int r) {m_revision = r;}
};
/*----------------------------------------------------------------------------*/
class QDeclarativeEngine;
class QDeclarativeComponent;
class DDAMeasureSession;
class DDAProcessing : public QObject
{
  Q_OBJECT
protected:
  QDeclarativeEngine *m_engine;
  QDeclarativeComponent *m_component;
  DDAMeasureSession *m_session;
  DDAExtension *m_qml;

  QString m_message;
  bool m_isError;

protected:
  void  setError(const QString &msg, bool err = true)
  {
    m_isError = err;
    m_message = msg;
  }

  void clrError() {m_isError = false;}

public:
  DDAProcessing(QObject *parent = 0);
  bool open(DDAMeasureSession *session);

public:
  bool isError() {return m_isError;}
  QString message() {return m_message;}
};

/*----------------------------------------------------------------------------*/
extern DDAProcessing *processing;
/*----------------------------------------------------------------------------*/
#endif /*QMLPROCESSING_H_1349933855*/

