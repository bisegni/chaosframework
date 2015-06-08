#include "CommandListModel.h"
#include "../../widget/list/delegate/TwoLineInformationItem.h"

#include <chaos/common/chaos_constants.h>
using namespace chaos::common::data;

CommandListModel::CommandListModel(QObject *parent):
    ChaosAbstractListModel(parent) {

}

void CommandListModel::updateData(const QSharedPointer<CDataWrapper>& _dataset) {
    beginResetModel();
    //remove last updated data
    command_description_array.clear();
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

QVariant CommandListModel::getRowData(int row) const {
    if(command_description_array.size()==0) return QVariant();
    QSharedPointer<TwoLineInformationItem> cmd_desc(new TwoLineInformationItem(QString::fromStdString(command_description_array[row]->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS)),
                                                                               QString::fromStdString(command_description_array[row]->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION)),
                                                                               command_description_array[row]));
    return QVariant::fromValue(cmd_desc);
}
