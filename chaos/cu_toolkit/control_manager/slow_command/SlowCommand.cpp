/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include <string>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::alarm;
using namespace chaos::common::metadata_logging;

using namespace chaos::cu::control_manager;
using namespace chaos::cu::control_manager::slow_command;

#define SCLOG_HEAD_SL "[SlowCommand-" << getDeviceID() << "-" << getUID() << "] "
#define SCLAPP_ LAPP_ << SCLOG_HEAD_SL
#define SCLDBG_ LDBG_ << SCLOG_HEAD_SL
#define SCLERR_ LERR_ << SCLOG_HEAD_SL

// default constructor
SlowCommand::SlowCommand() {}

// default destructor
SlowCommand::~SlowCommand() {
    
}

const string & SlowCommand::getDeviceID() {
    return abstract_control_unit->getDeviceID();
}

void SlowCommand::setAutoBusy(bool new_auto_busy) {
    instance_custom_attribute["auto_busy"] = CDataVariant(new_auto_busy);
}

const bool SlowCommand::isAutoBusy() {
    return instance_custom_attribute["auto_busy"].asBool();
}

chaos::common::data::DatasetDB * const SlowCommand::getDeviceDatabase() {
    return abstract_control_unit;
}

AttributeSharedCacheWrapper * const SlowCommand::getAttributeCache() {
    return attribute_cache;
}

#pragma mark Alarm API
void SlowCommand::setStateVariableSeverity(StateVariableType variable_type,
                                           const common::alarm::MultiSeverityAlarmLevel state_variable_severity) {
    abstract_control_unit->setStateVariableSeverity(variable_type,
                                                    state_variable_severity);
}

bool SlowCommand::setStateVariableSeverity(StateVariableType variable_type,
                                           const std::string& state_variable_name,
                                           const MultiSeverityAlarmLevel state_variable_severity) {
    return abstract_control_unit->setStateVariableSeverity(variable_type,
                                                           state_variable_name,
                                                           state_variable_severity);
}

bool SlowCommand::setStateVariableSeverity(StateVariableType variable_type,
                                           const unsigned int state_variable_ordered_id,
                                           const chaos::common::alarm:: MultiSeverityAlarmLevel state_variable_severity) {
    return abstract_control_unit->setStateVariableSeverity(variable_type,
                                                           state_variable_ordered_id,
                                                           state_variable_severity);
}

bool SlowCommand::getStateVariableSeverity(StateVariableType variable_type,
                                           const std::string& state_variable_name,
                                           MultiSeverityAlarmLevel& state_variable_severity) {
    return abstract_control_unit->getStateVariableSeverity(variable_type,
                                                           state_variable_name,
                                                           state_variable_severity);
}

bool SlowCommand::getStateVariableSeverity(StateVariableType variable_type,
                                           const unsigned int state_variable_ordered_id,
                                           MultiSeverityAlarmLevel& state_variable_severity) {
    return abstract_control_unit->getStateVariableSeverity(variable_type,
                                                           state_variable_ordered_id,
                                                           state_variable_severity);
}

void SlowCommand::setBusyFlag(bool state) {
    if(isAutoBusy()) {return;}
    return abstract_control_unit->setBusyFlag(state);

}
const string & SlowCommand::getDeviceLoadParams(){
     if(abstract_control_unit==NULL)
        return std::string();
   
    return abstract_control_unit->getCUParam();
}

int SlowCommand::getDeviceLoadParams(chaos::common::data::CDataWrapper&parm){
    if(abstract_control_unit==NULL)
        return -1;
    return abstract_control_unit->getCUParam(parm);
    
}

bool SlowCommand::getBusyFlag() {
     if(abstract_control_unit==NULL)
        return false;
   
    return abstract_control_unit->getBusyFlag();
}

void SlowCommand::metadataLogging(const StandardLoggingChannel::LogLevel log_level,
                                  const std::string& message) {
    abstract_control_unit->metadataLogging(getAlias(),
                                           log_level,
                                           message);
}

void SlowCommand::startHandler() {
    BatchCommand::startHandler();
    SCLAPP_ << "Starting command";
    for(common::batch_command::BCInstantiationAttributeMapIterator it = instance_custom_attribute.begin(),
        end = instance_custom_attribute.end();
        it != end;
        it++) {
        SCLAPP_ << CHAOS_FORMAT("Custom property:%1% - Value:%2%", %it->first%it->second.asString());
    }
    
    if(isAutoBusy()) {
        abstract_control_unit->setBusyFlag(true);
    }
}

void SlowCommand::endHandler() {
    if(isAutoBusy()) {
        abstract_control_unit->setBusyFlag(false);
    }
    BatchCommand::endHandler();
    SCLAPP_ << "Finish command";
}
