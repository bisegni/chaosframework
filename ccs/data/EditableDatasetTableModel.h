#ifndef EditableDatasetTableModel_H
#define EditableDatasetTableModel_H

#include "ChaosAbstractTableModel.h"

#include <chaos/common/data/structured/DatasetAttribute.h>

class EditableDatasetTableModel:
        public ChaosAbstractTableModel {
public:
    EditableDatasetTableModel(QObject *parent=0);
    void addNewDatasetAttribute();
    void editDatasetAttributeAtIndex(unsigned int dsattr_index);
    void removeElementFromDatasetAtIndex(unsigned int var_index);
    void setDatasetAttributeList(chaos::common::data::structured::DatasetAttributeList *master_attribute_list);
protected:
    void addNewElemenToToDataset(const chaos::common::data::structured::DatasetAttribute& new_dataset_attribute);
    void removeElementFromDataset(const QString& attribute_name,
                                  const chaos::DataType::DataSetAttributeIOAttribute direction);

    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    bool isCellEditable(const QModelIndex &index) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    bool isCellSelectable(const QModelIndex &index) const;
private:
    //! reference to attribute list
    chaos::common::data::structured::DatasetAttributeList *attribute_list;
};

#endif // EditableDatasetTableModel_H
