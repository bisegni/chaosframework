#ifndef EDITABLESCRIPTVARIABLETABLEMODEL_H
#define EDITABLESCRIPTVARIABLETABLEMODEL_H

#include "ChaosAbstractTableModel.h"

#include <chaos_service_common/data/dataset/AlgorithmVariable.h>

class EditableScriptVariableTableModel:
        public ChaosAbstractTableModel {
public:
    EditableScriptVariableTableModel(QObject *parent=0);
    void addNewVariable();
    void editVariableAtIndex(unsigned int var_index);
    void removeVariableAtIndex(unsigned int var_index);
    void setVariableList(chaos::service_common::data::dataset::AlgorithmVariableList *master_variable_list);
protected:

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
    chaos::service_common::data::dataset::AlgorithmVariableList *variable_list;
};

#endif // EDITABLESCRIPTVARIABLETABLEMODEL_H
