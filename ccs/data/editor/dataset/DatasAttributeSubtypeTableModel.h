#ifndef DATASATTRIBUTESUBTYPETABLEMODEL_H
#define DATASATTRIBUTESUBTYPETABLEMODEL_H

#include "../../ChaosAbstractTableModel.h"

#include <chaos_service_common/data/dataset/DatasetAttribute.h>

class DatasAttributeSubtypeTableModel:
public ChaosAbstractTableModel {
public:
    DatasAttributeSubtypeTableModel(QObject *parent=0);
    void setDatasetSubtypesList(chaos::service_common::data::dataset::DatasetSubtypeList *dataset_subtype_list);
    void addNew();
    void removeAtIndex(unsigned int idx);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getCheckeable(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    bool setCellData(const QModelIndex &index, const QVariant &value);
    bool isCellEditable(const QModelIndex &index) const;
    bool isCellCheckable(const QModelIndex &index) const;
    QVariant getTextAlignForData(int row, int column) const;
private:
    chaos::service_common::data::dataset::DatasetSubtypeList *dataset_subtype_list_ptr;
};

#endif // DATASATTRIBUTESUBTYPETABLEMODEL_H
