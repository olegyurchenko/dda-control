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
#include <QAbstractTableModel>
#include <QVariantMap>

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
  void update();

  Q_INVOKABLE void addModel(QObject*);
  Q_INVOKABLE QObject *newTableModel();
  Q_INVOKABLE QObject *newCurveModel();
  Q_INVOKABLE QObject *newHistogrammModel();
  Q_INVOKABLE QObject *newGraphModel();

signals:
  void error(QString);
  void modelAdded(QObject*);
};
/*----------------------------------------------------------------------------*/
class TableModel : public QAbstractTableModel
{
  Q_OBJECT
  Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount)
  Q_PROPERTY(int columnCount READ columnCount WRITE setColumnCount)
protected:
  int m_rowCount;
  int m_columnCount;
  QVariantMap m_data;
public:
  TableModel(QObject *parent = 0);
  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &) const;
  int columnCount(const QModelIndex &) const;
public slots:
  int rowCount() {return m_rowCount;}
  int columnCount() {return m_columnCount;}
  void setRowCount(int c) {m_rowCount = c;}
  void setColumnCount(int c) {m_columnCount = c;}
  void clear();
  void update();
  QVariant data(int row, int col);
  void setData(int row, int col, const QVariant &val, int role = Qt::DisplayRole);
  void setHeaderData(int section, int orientation, const QVariant &value);
};
/*----------------------------------------------------------------------------*/
extern DDAProcessing *processing;
/*----------------------------------------------------------------------------*/
#endif /*QMLPROCESSING_H_1349933855*/

