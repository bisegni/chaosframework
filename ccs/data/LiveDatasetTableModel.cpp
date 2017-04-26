#include "LiveDatasetTableModel.h"
#include "../metatypes.h"

#include <chaos/common/chaos_constants.h>

#include <QDebug>

using namespace chaos::common::data;

LiveDatasetTableModel::LiveDatasetTableModel(QObject *parent):
    ChaosAbstractTableModel(parent){}

void LiveDatasetTableModel::setDataset(const QString& _node_uid,
                                       const QString& _dataset_key,
                                       CDWShrdPtr live_dataset) {
    ChaosStringVector all_keys;
    live_dataset->getAllKey(all_keys);
    beginResetModel();
    dataset_values.clear();
    node_uid = _node_uid;
    dataset_key = _dataset_key;
    qDebug() << QString("Fill table for node %11 and dataset kery %2").arg(node_uid).arg(dataset_key);
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

bool LiveDatasetTableModel::setCellData(const QModelIndex &index, const QVariant &value) {
    CDataVariant new_cvariant_value;
    bool result = true;
    switch (value.type()) {
    case QMetaType::Bool:
        new_cvariant_value = CDataVariant(value.toBool());
        break;
    case QMetaType::Int:
    case QMetaType::UInt:
        new_cvariant_value = CDataVariant(value.toInt());
        break;
    case QMetaType::Long:
    case QMetaType::ULong:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        new_cvariant_value = CDataVariant(value.toLongLong());
        break;
    case QMetaType::Double:
        new_cvariant_value = CDataVariant(value.toDouble());
        break;
    case QMetaType::QString:
        new_cvariant_value = CDataVariant(value.toString().toStdString());
        break;
    default:
        result = false;
        break;
    }
    if(result){dataset_values[index.row()].second = new_cvariant_value;}
    return result;
}

bool LiveDatasetTableModel::isCellEditable(const QModelIndex &index) const {
    bool can_update = index.isValid() && index.column() == 1;
    can_update = can_update && (dataset_values[index.row()].first.compare(chaos::DataPackCommonKey::DPCK_DEVICE_ID) != 0);
    can_update = can_update && (dataset_values[index.row()].first.compare(chaos::DataPackCommonKey::DPCK_SEQ_ID) != 0);
    can_update = can_update && (dataset_values[index.row()].first.compare(chaos::DataPackCommonKey::DPCK_TIMESTAMP) != 0);
    can_update = can_update && (dataset_values[index.row()].first.compare(chaos::DataPackCommonKey::DPCK_DATASET_TYPE) != 0);
    return can_update;
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
