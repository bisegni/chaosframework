#include "ChaosAbstractListModel.h"

ChaosAbstractListModel::ChaosAbstractListModel(QObject *parent):
    QAbstractListModel(parent) {

}

QVariant ChaosAbstractListModel::getRowData(int row) const {
    return QVariant();
}

QVariant ChaosAbstractListModel::getTooltipTextForData(int row) const {
    return QVariant();
}

int ChaosAbstractListModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return getRowCount();
}

QVariant  ChaosAbstractListModel::data(const QModelIndex &index, int role) const {
    int row = index.row();

    QVariant result;
    if (!index.isValid())
        return result;
    if (index.row() >= getRowCount())
          return result;


    switch(role){
    case Qt::DisplayRole:
        result = getRowData(row);
        break;
    case Qt::ToolTipRole:
        result = getTooltipTextForData(row);
        break;
    case Qt::UserRole:
            result = getUserData(row);
        break;
    default:
        break;
    }
    return result;
}
