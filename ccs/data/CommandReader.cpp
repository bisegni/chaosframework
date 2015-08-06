#include "CommandReader.h"

using namespace chaos::common::data;
using namespace chaos::common::batch_command;

CommandReader::CommandReader(QSharedPointer<chaos::common::data::CDataWrapper> _command_description,
                             QObject *parent) :
    QObject(parent),
    command_description(_command_description) {
    //we need to find and fill cache
    if(command_description->hasKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS) &&
            command_description->isVectorValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS) ) {

        //fetch the description of the value
        QSharedPointer<CMultiTypeDataArrayWrapper> param_vector(command_description->getVectorValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS));
        for(int idx = 0;
            idx < param_vector->size();
            idx++) {
            QSharedPointer<CDataWrapper> param_description(param_vector->getCDataWrapperElementAtIndex(idx));
            //we have found the description so we nend to cache and return it
            command_parameter_cache.insert(QString::fromStdString(param_description->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME)),
                                           param_description);
        }
    }
}

CommandReader::~CommandReader() {

}

QSharedPointer<CommandParameterReader> CommandReader::getCommandParameter(const QString& parameter_name) {
    if(command_parameter_cache.contains(parameter_name)){
        return QSharedPointer<CommandParameterReader>(new CommandParameterReader(command_parameter_cache[(parameter_name)]));
    }
    return QSharedPointer<CommandParameterReader>();
}

QList<QString> CommandReader::getParameterList() {
    return command_parameter_cache.keys();
}

QList< QSharedPointer<CommandParameterReader> > CommandReader::getCommandParameterList() {
    QList< QSharedPointer<CommandParameterReader> > result;
    foreach (QString k, command_parameter_cache.keys()) {
        result.push_back(getCommandParameter(k));
    }
    return result;
}
