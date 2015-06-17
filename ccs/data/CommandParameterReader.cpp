#include "CommandParameterReader.h"
using namespace chaos::common::data;
using namespace chaos::common::batch_command;

CommandParameterReader::CommandParameterReader(QSharedPointer<CDataWrapper> _command_parameter_description,
                                               QObject *parent) :
    QObject(parent),
    command_parameter_description(_command_parameter_description) {

}

QString CommandParameterReader::getParameterName() {
    if(!command_parameter_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME) ||
            !command_parameter_description->isStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME)) return QString();

    return QString::fromStdString(command_parameter_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME));
}

QString CommandParameterReader::getParameterDescription() {
    if(!command_parameter_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION) ||
            !command_parameter_description->isStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION)) return QString();

    return QString::fromStdString(command_parameter_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION));
}

int32_t CommandParameterReader::getParameterType() {
    if(!command_parameter_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE) ||
            !command_parameter_description->isStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)) return -1;

    return command_parameter_description->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE);
}

bool CommandParameterReader::isMandatory() {
    if(!command_parameter_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG) ||
            !command_parameter_description->isStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG)) return 0;
    int flags = command_parameter_description->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG);
    return (flags&BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY) ==
            BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY;
}
