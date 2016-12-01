#include "LiveDatasetTableModel.h"
#include "../metatypes.h"

#include <chaos/common/chaos_constants.h>

LiveDatasetTableModel::LiveDatasetTableModel(QObject *parent):
    ChaosAbstractTableModel(parent){}

void LiveDatasetTableModel::setDatasetAttributeList(chaos::common::data::CDataWrapper& live_dataset) {
    ChaosStringVector all_keys;
    live_dataset.getAllKey(all_keys);
    beginResetModel();
    for(ChaosStringVectorIterator it = all_keys.begin(),
        end = all_keys.end();
        it != end;
        it++) {
        dataset_values.push_back(DatasetElement(QString::fromStdString(*it), live_dataset.getVariantValue(*it)));
    }
    endResetModel();
}

int LiveDatasetTableModel::getRowCount() const {
    return dataset_values.size();
}

int LiveDatasetTableModel::getColumnCount() const {
    return 2;
}

QString LiveDatasetTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch (column) {
    case 1:
        result = "Attribute";
        break;
    case 2:
        result = "Values";
        break;
    default:
        break;
    }
    return result;
}

QVariant LiveDatasetTableModel::getCellData(int row, int column) const {
    QVariant result;
    switch (column) {
    case 1:
        result = dataset_values[row].first;
        break;
    case 2:
        result = dataset_values[row].second;
        break;
    default:
        break;
    }
    return result;
}

bool LiveDatasetTableModel::isCellEditable(const QModelIndex &index) const {
    return false;
}

QVariant LiveDatasetTableModel::getTooltipTextForData(int row, int column) const {
    return QVariant();
}

QVariant LiveDatasetTableModel::getTextAlignForData(int row, int column) const {
    return QVariant();
}

bool LiveDatasetTableModel::isCellSelectable(const QModelIndex &index) const {
    return false;
}
