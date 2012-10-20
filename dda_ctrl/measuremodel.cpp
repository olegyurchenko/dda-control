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
#include "measuremodel.h"
#include <session.h>
enum MeasureColumn
{
  SizeCol,
  StrenthCol,
  ColCount
};

/*----------------------------------------------------------------------------*/
MeasureModel :: MeasureModel(QObject *parent, DDAMeasureSession *session)
  : QAbstractTableModel(parent)
{
  m_rowCount = 0;
  m_session = ::session;
  if(session != NULL)
    m_session = session;
  connect(m_session, SIGNAL(measureListChanged()), this, SLOT(update()));
}
/*----------------------------------------------------------------------------*/
QVariant MeasureModel :: data(const QModelIndex &index, int role) const
{
  int col = index.column();
  int row = index.row();
  if(row < 0 || row >= m_session->measureList().size())
    return QVariant();
  DDAMeasure measure = m_session->measureList()[row];
  if(role == Qt::DisplayRole)
  {
    switch(col)
    {
    case SizeCol:
      if(measure.ignored)
        return "--.-";
      return QString::number(measure.size, 'f', 1);
    case StrenthCol:
      if(measure.ignored)
        return "--.-";
      return QString::number(measure.strenght, 'f', 1);
    default:
      return QVariant();
    }
  }

  if(role == Qt::TextAlignmentRole)
  {
    return QVariant((int)(Qt::AlignRight | Qt::AlignVCenter));
  }

  return QVariant();
}
/*----------------------------------------------------------------------------*/
Qt::ItemFlags MeasureModel :: flags(const QModelIndex &) const
{
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled /*| Qt::ItemIsEditable*/;
}
/*----------------------------------------------------------------------------*/
QVariant MeasureModel :: headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role == Qt::DisplayRole)
  {
    if(orientation == Qt::Vertical)
    {
      return section + 1;
    }
    else
    {
      switch(section)
      {
      case SizeCol:
        return tr("Size [um]");
      case StrenthCol:
        return tr("Stretch [N]");
      default:
        return QVariant();
      }
    }
  }
  return QVariant();
}
/*----------------------------------------------------------------------------*/
int MeasureModel :: rowCount(const QModelIndex &) const
{
  return m_rowCount;
}
/*----------------------------------------------------------------------------*/
int MeasureModel :: columnCount(const QModelIndex &) const
{
  return ColCount;
}
/*----------------------------------------------------------------------------*/
void MeasureModel :: update()
{
  if(m_rowCount != m_session->measureList().size())
  {
    m_rowCount = m_session->measureList().size();
    reset();
  }
  emit dataChanged(index(0,0), index(m_rowCount, ColCount));
}
/*----------------------------------------------------------------------------*/
