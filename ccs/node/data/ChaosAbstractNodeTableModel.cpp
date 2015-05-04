#include "ChaosAbstractNodeTableModel.h"

#include <QDebug>

ChaosAbstractNodeTableModel::ChaosAbstractNodeTableModel(QObject *parent)
    :QAbstractTableModel(parent) {
}

int ChaosAbstractNodeTableModel::rowCount(const QModelIndex& parent) const
{
    return getRowCount();
}

int ChaosAbstractNodeTableModel::columnCount(const QModelIndex& parent) const
{
    return getColumnCount();
}

QVariant ChaosAbstractNodeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return getHeaderForColumn(section);
        }
    }
    return QVariant();
}

QVariant ChaosAbstractNodeTableModel::getCellData(int row, int column)   const{
    return QVariant();
}

QVariant ChaosAbstractNodeTableModel::getFontForData(int row, int column)   const{
    return QVariant();
}

QVariant ChaosAbstractNodeTableModel::getBackgroundForData(int row, int column)  const{
    return QVariant();
}

QVariant ChaosAbstractNodeTableModel::getTextAlignForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractNodeTableModel::getCheckStateForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractNodeTableModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();
    QVariant result;
    // generate a log message when this method gets called
    qDebug() << QString("row %1, col%2, role %3")
                .arg(row).arg(col).arg(role);

    switch(role){
    case Qt::DisplayRole:
        result = getCellData(row, col);
        break;
    case Qt::FontRole:
        result = getFontForData(row, col);
        break;
    case Qt::BackgroundRole:
        result = getBackgroundForData(row, col);
        break;
    case Qt::TextAlignmentRole:
        result = getTextAlignForData(row, col);
        break;
    default:
        break;
    }
    return result;
}
