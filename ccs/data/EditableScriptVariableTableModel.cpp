#include "EditableScriptVariableTableModel.h"
#include "editor/script/EditableScriptVariableTableModelEditDialog.h"

using namespace chaos::service_common::data::dataset;

EditableScriptVariableTableModel::EditableScriptVariableTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    variable_list(NULL){}

void EditableScriptVariableTableModel::addNewVariable() {
    EditableScriptVariableTableModelEditDialog new_variable_dialog;
    if(new_variable_dialog.exec() == QDialog::Accepted) {
        //we have a new element to add
        beginResetModel();
        variable_list->push_back(new_variable_dialog.getVariable());
        endResetModel();
    }
}

void EditableScriptVariableTableModel::editVariableAtIndex(unsigned int var_index) {
    if(var_index >= (*variable_list).size()) return;

    EditableScriptVariableTableModelEditDialog new_var_dialog;
    new_var_dialog.setVariable((*variable_list)[var_index]);
    if(new_var_dialog.exec() == QDialog::Accepted) {
        //we have a new element to add
        beginResetModel();
        (*variable_list)[var_index] = new_var_dialog.getVariable();
        endResetModel();
    }
}

void EditableScriptVariableTableModel::removeVariableAtIndex(unsigned int var_index) {
    beginResetModel();
    variable_list->erase(variable_list->begin()+var_index);
    endResetModel();
}

void EditableScriptVariableTableModel::setVariableList(chaos::service_common::data::dataset::AlgorithmVariableList *master_variable_list) {
    beginResetModel();
    variable_list = master_variable_list;
    endResetModel();
}

int EditableScriptVariableTableModel::getRowCount() const {
    return (variable_list?variable_list->size():0);
}

int EditableScriptVariableTableModel::getColumnCount() const {
    return 4;
}

QString EditableScriptVariableTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch (column) {
    case 0:
        result = QString("Name");
        break;
    case 1:
        result = QString("Dir");
        break;
    case 2:
        result = QString("Type");
        break;
    case 3:
        result = QString("Description");
        break;
    }
    return result;
}


QVariant EditableScriptVariableTableModel::getCellData(int row, int column) const {
    QVariant result;
    AlgorithmVariable &var_ref = (*variable_list)[row];
    switch (column) {
    case 0:
        result = QString::fromStdString(var_ref.name);
        break;
    case 1:
        switch(var_ref.direction){
        case chaos::DataType::Input:
            return tr("Input");
            break;
        case chaos::DataType::Output:
            return tr("Output");
            break;
        case chaos::DataType::Bidirectional:
            return tr("Bidirectional");
            break;
        }

        break;
    case 2:
        result = QString::fromStdString(chaos::DataType::typeDescriptionByCode(var_ref.type));
        break;
    case 3:
        result = QString::fromStdString(var_ref.description);
        break;
    }
    return result;
}

bool EditableScriptVariableTableModel::isCellEditable(const QModelIndex &index) const {
    return false;
}

QVariant EditableScriptVariableTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}

QVariant EditableScriptVariableTableModel::getTextAlignForData(int row, int column) const {
    return QVariant();
}

bool EditableScriptVariableTableModel::isCellSelectable(const QModelIndex &index) const {
    return true;
}
