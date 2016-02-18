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

Qt::CheckState ChaosAbstractListModel::getCheckableState(int row)const {
    return Qt::Unchecked;
}

int ChaosAbstractListModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return getRowCount();
}

bool ChaosAbstractListModel::isRowCheckable(int row) const {
    return false;
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
    case Qt::CheckStateRole:
            result = getCheckableState(row);
        break;
    default:
        break;
    }
    return result;
}

bool ChaosAbstractListModel::setRowData(const int row, const QVariant& value) {
    return false;
}

bool ChaosAbstractListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    bool result = false;
    if (index.isValid() &&
            (role == Qt::EditRole || role == Qt::CheckStateRole)) {
        if(result = setRowData(index.row(), value)){
            emit(dataChanged(index, index));
        }
    }
    return result;
}

Qt::ItemFlags ChaosAbstractListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (!index.isValid()) return flags;
    if(isRowCheckable(index.row())) {
        flags |= Qt::ItemIsUserCheckable;
    }
    return flags;
}
