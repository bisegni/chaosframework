#include "LiveDatasetTableModel.h"
#include "../metatypes.h"

using namespace chaos::common::data;

LiveDatasetTableModel::LiveDatasetTableModel(QObject *parent):
    ChaosAbstractTableModel(parent){}

void LiveDatasetTableModel::setDataset(CDWShrdPtr live_dataset) {
    ChaosStringVector all_keys;
    live_dataset->getAllKey(all_keys);
    beginResetModel();
    dataset_values.clear();
    for(ChaosStringVectorIterator it = all_keys.begin(),
        end = all_keys.end();
        it != end;
        it++) {
        dataset_values.push_back(DatasetElement(QString::fromStdString(*it), live_dataset->getVariantValue(*it)));
    }
    endResetModel();
}

void LiveDatasetTableModel::clear() {
    beginResetModel();
    dataset_values.clear();
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
    case 0:
        result = "Attribute";
        break;
    case 1:
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
    case 0:
        result = dataset_values[row].first;
        break;
    case 1:
        result = QString::fromStdString(dataset_values[row].second.asString());
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
