#include "CommandListModel.h"
#include "../data/delegate/TwoLineInformationItem.h"
#include "../metatypes.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy::node;

CommandListModel::CommandListModel(QObject *parent):
    ChaosAbstractListModel(parent) {

}

void CommandListModel::updateData(const QSharedPointer<CDataWrapper>& _dataset) {
    beginResetModel();
    //remove last updated data
    command_description_array.clear();

    //scan commands
    if(!_dataset.isNull() &&
            _dataset->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION)){
        CMultiTypeDataArrayWrapperSPtr command_array = _dataset->getVectorValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION);
        for(int idx = 0;
            idx < command_array->size();
            idx++){
            QSharedPointer<CDataWrapper> command_description(command_array->getCDataWrapperElementAtIndex(idx));
            if(command_description->hasKey(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS)&&
                    command_description->hasKey(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION)&&
                    command_description->hasKey(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)){
                command_description_array.push_back(command_description);
            }
        }
    }
    endResetModel();
}

void CommandListModel::fillWithCommandUID(CommandUIDList& uid) {
    foreach(QSharedPointer<CDataWrapper> element, command_description_array) {
        uid.push_back(element->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID));
    }
}

int CommandListModel::getRowCount() const {
    return command_description_array.size();
}

QVariant CommandListModel::getUserData(int row) const {
    return QString::fromStdString(command_description_array[row]->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID));
}

QVariant CommandListModel::getRowData(int row) const {
    if(command_description_array.size()==0) return QVariant();
    QSharedPointer<chaos::common::data::CDataWrapper> data = command_description_array[row];
    QSharedPointer<TwoLineInformationItem> cmd_desc(new TwoLineInformationItem(QString::fromStdString(command_description_array[row]->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS)),
                                                                               QString::fromStdString(command_description_array[row]->getStringValue(chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION)),
                                                                               QVariant::fromValue(data)));
    return QVariant::fromValue(cmd_desc);
}
