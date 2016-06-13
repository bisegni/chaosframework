#include "ChaosAbstractListModel.h"

#include <QDebug>

ChaosAbstractListModel::ChaosAbstractListModel(QObject *parent):
    QAbstractListModel(parent) {}

ChaosAbstractListModel::~ChaosAbstractListModel(){}

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

bool ChaosAbstractListModel::isRowEditable(int row) const {
    return false;
}

bool ChaosAbstractListModel::isRowSelectable(int row) const {
    return true;
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
        if(isRowCheckable(row)){
            result = getCheckableState(row);
        }
        break;
    default:
        break;
    }
    return result;
}

bool ChaosAbstractListModel::setRowData(const int row, const QVariant& value) {
    return false;
}

bool ChaosAbstractListModel::setRowCheckState(const int row, const QVariant& value) {
    return false;
}

bool ChaosAbstractListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    bool result = false;
    if (!index.isValid()) return false;
    switch(role) {
    case Qt::EditRole:
        result = setRowData(index.row(), value);
        break;

    case Qt::CheckStateRole:
        if(isRowCheckable(index.row())){
            result = setRowCheckState(index.row(), value);
        }
        break;
    default:
        break;
    }
    if(result) {
        emit(dataChanged(index, index));
    }
    return result;
}


Qt::ItemFlags ChaosAbstractListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (!index.isValid()) return flags;
    if(isRowCheckable(index.row())) {
        flags |= Qt::ItemIsUserCheckable;
    }
    if(isRowEditable(index.row())) {
        flags |= Qt::ItemIsEditable;
    }
    if(isRowSelectable(index.row())) {
        flags |= Qt::ItemIsSelectable;
    }
    return flags;
}
