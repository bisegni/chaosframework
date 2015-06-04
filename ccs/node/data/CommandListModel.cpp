#include "CommandListModel.h"

using namespace chaos::common::data;

CommandListModel::CommandListModel(QObject *parent):
    ChaosAbstractListModel(parent) {

}

void CommandListModel::updateData(const QSharedPointer<CDataWrapper>& _dataset) {
    beginResetModel();
    if(_dataset->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION)){
        QSharedPointer<CMultiTypeDataArrayWrapper> command_array(_dataset->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION));
        for(int idx = 0;
            idx < command_array->size();
            idx++){
            command_description_array.push_back( QSharedPointer<CDataWrapper>(command_array->getCDataWrapperElementAtIndex(idx)));
        }
    }
    endResetModel();
}

int CommandListModel::getRowCount() const {
    return command_description_array.size();
}

QString CommandListModel::getHeaderForSection(int section) const {
    return tr("Commands");
}

QVariant CommandListModel::getRowData(int row) const {
    if(command_description_array.size()==0) return QVariant();
    if(command_description_array[row]->hasKey(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS)) {
        return QString::fromStdString(command_description_array[row]->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS));
    }else {
        return tr("no alias found");
    }
}
