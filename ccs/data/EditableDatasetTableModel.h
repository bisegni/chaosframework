#ifndef EditableDatasetTableModel_H
#define EditableDatasetTableModel_H

#include "../data/ChaosAbstractTableModel.h"

#include <chaos_service_common/data/dataset/DatasetAttribute.h>

class EditableDatasetTableModel:
        public ChaosAbstractTableModel {
public:
    EditableDatasetTableModel(QObject *parent=0);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    bool setCellData(const QModelIndex &index, const QVariant &value);
    bool isCellEditable(const QModelIndex &index) const;

    QString decodeTypeToString(chaos::DataType::DataType type) const;
    chaos::DataType::DataType decodeStringToType(const QString &type_string);
private:
    //! reference to attribute list
    chaos::service_common::data::dataset::DatasetAttributeList *attribute_list;
};

#endif // EditableDatasetTableModel_H
