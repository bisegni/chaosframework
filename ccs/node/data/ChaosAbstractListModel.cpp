#include "ChaosAbstractListModel.h"

ChaosAbstractListModel::ChaosAbstractListModel(QObject *parent):
    QAbstractListModel(parent) {

}

QVariant ChaosAbstractListModel::getRowData(int row) const {
    return QVariant();
}

QVariant ChaosAbstractListModel::getFontForData(int row) const {
    return QVariant();
}

QVariant ChaosAbstractListModel::getBackgroundForData(int row) const {
    return QVariant();
}

QVariant ChaosAbstractListModel::getTextColorForData(int row) const {
    return QVariant();
}

QVariant ChaosAbstractListModel::getTextAlignForData(int row) const {
    return QVariant();
}

QVariant ChaosAbstractListModel::getCheckStateForData(int row) const {
    return QVariant();
}

QVariant ChaosAbstractListModel::getTooltipTextForData(int row) const {
    return QVariant();
}

bool ChaosAbstractListModel::setCellData(const QModelIndex &index, const QVariant &value) {
    return false;
}

bool ChaosAbstractListModel::isCellEditable(const QModelIndex &index) const {
    return false;
}

int ChaosAbstractListModel::rowCount(const QModelIndex& parent) const {
    return getRowCount();
}

QVariant  ChaosAbstractListModel::data(const QModelIndex &index, int role) const {
    int row = index.row();

    QVariant result;
    if (!index.isValid())
        return QVariant();
    switch(role){
    case Qt::DisplayRole:
        result = getRowData(row);
        break;
    case Qt::FontRole:
        result = getFontForData(row);
        break;
    case Qt::BackgroundRole:
        result = getBackgroundForData(row);
        break;
    case Qt::TextColorRole:
        result = getTextColorForData(row);
        break;
    case Qt::TextAlignmentRole:
        result = getTextAlignForData(row);
        break;
    case Qt::ToolTipRole:
        result = getTooltipTextForData(row);
    default:
        break;
    }
    return result;
}

QVariant ChaosAbstractListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    return getHeaderForSection(section);
}

Qt::ItemFlags ChaosAbstractListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (!index.isValid()) return flags;
    if(isCellEditable(index)) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}


bool ChaosAbstractListModel::setData(const QModelIndex &index,
                                     const QVariant &value,
                                     int role) {
    bool result = false;
    if (index.isValid() && role == Qt::EditRole) {
        if(result = setCellData(index, value)){
            emit(dataChanged(index, index));
        }
    }
    return result;
}
