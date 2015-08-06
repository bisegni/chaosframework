#include "DatasetAttributeListModel.h"

DatasetAttributeListModel::DatasetAttributeListModel(QObject *parent) :
    ChaosAbstractListModel(parent) {

}

void DatasetAttributeListModel::updateDataset(const QSharedPointer<DatasetReader> &_dataset_reader) {
    beginResetModel();
    dataset_reader = _dataset_reader;
    attribute_name_list = dataset_reader->getAttributeNameList();
    endResetModel();
}

int DatasetAttributeListModel::getRowCount() const {
    return attribute_name_list.size();
}

QVariant DatasetAttributeListModel::getUserData(int row) const {
    if(attribute_name_list.size()==0) return QVariant();
    QVariant result = QVariant::fromValue< QSharedPointer<DatasetAttributeReader> >(dataset_reader->getAttribute(attribute_name_list[row]));
    return result;
}

QVariant DatasetAttributeListModel::getRowData(int row) const {
    if(attribute_name_list.size()==0) return QVariant();
    return dataset_reader->getAttribute(attribute_name_list[row])->getName();
}
