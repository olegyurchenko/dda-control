/*----------------------------------------------------------------------------*/
/**
* @pkg measuremodel
*/
/**
* Model for deliver measure data for measure table view.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 04.10.2012  9:03:24<br>
* @pkgdoc measuremodel
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef MEASUREMODEL_H_1349330604
#define MEASUREMODEL_H_1349330604
/*----------------------------------------------------------------------------*/
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

class MeasureModel : public QAbstractTableModel
{
  Q_OBJECT
protected:
  int m_rowCount;
public:
  MeasureModel(QObject *parent = 0);
  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &) const;
  int columnCount(const QModelIndex &) const;
public slots:
  void update();
};

/*----------------------------------------------------------------------------*/
#endif /*MEASUREMODEL_H_1349330604*/

