#include "ChaosAbstractTableModel.h"

#include <QDebug>

ChaosAbstractTableModel::ChaosAbstractTableModel(QObject *parent)
    :QAbstractTableModel(parent) {
}

int ChaosAbstractTableModel::rowCount(const QModelIndex& parent) const
{
    return getRowCount();
}

int ChaosAbstractTableModel::columnCount(const QModelIndex& parent) const
{
    return getColumnCount();
}

Qt::ItemFlags ChaosAbstractTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

QVariant ChaosAbstractTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return getHeaderForColumn(section);
        }
    }
    return QVariant();
}

QVariant ChaosAbstractTableModel::getCellData(int row, int column)   const{
    return QVariant();
}

QVariant ChaosAbstractTableModel::getFontForData(int row, int column)   const{
    return QVariant();
}

QVariant ChaosAbstractTableModel::getBackgroundForData(int row, int column)  const{
    return QVariant();
}

QVariant ChaosAbstractTableModel::getTextAlignForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::getCheckStateForData(int row, int column) const {
    return QVariant();
}

QVariant ChaosAbstractTableModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();
    QVariant result;
    if (!index.isValid())
             return QVariant();
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
