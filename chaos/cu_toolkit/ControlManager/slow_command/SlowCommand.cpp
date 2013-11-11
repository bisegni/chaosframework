/*
 *	SlowCommand.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#include <string>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;

#define LOG_HEAD "[SlowCommand-" << unique_id << "] "
#define SCLAPP_ LAPP_ << LOG_HEAD
#define SCLDBG_ LDBG_ << LOG_HEAD
#define SCLERR_ LERR_ << LOG_HEAD

// default constructor
SlowCommand::SlowCommand() {
    
    already_setupped = false;
    
	//reset all lock flag
    lockFeaturePropertyFlag.reset();
	//setup feautere fields
	std::memset(&commandFeatures, 0,sizeof(features::Features));
	commandFeatures.featuresFlag = 0;
	
	//reset the timing flags
	std::memset(&timing_stats,0,sizeof(CommandTimingStats));
	
	//set default value for running property and submission flag
    runningProperty = RunningStateType::RS_Exsc;
    submissionRule = SubmissionRuleType::SUBMIT_NORMAL;
}

// default destructor
SlowCommand::~SlowCommand() {
    
}

/*
 return the device database with the dafualt device information
 */
chaos::common::data::DatasetDB *SlowCommand::getDeviceDatabase() {
    return deviceDatabasePtr;
}

void SlowCommand::getChangedVariableIndex(IOCAttributeShareCache::SharedVeriableDomain domain, std::vector<VariableIndexType>& changed_index) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getSharedDomain(domain).getChangedIndex(changed_index);
}

ValueSetting *SlowCommand::getVariableValue(IOCAttributeShareCache::SharedVeriableDomain domain, VariableIndexType variable_index) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getSharedDomain(domain).getValueSettingForIndex(variable_index);
}

ValueSetting *SlowCommand::getVariableValue(IOCAttributeShareCache::SharedVeriableDomain domain, const char *variable_name) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
	AttributeSetting& attribute_setting = sharedAttributeSettingPtr->getSharedDomain(domain);
    VariableIndexType index = attribute_setting.getIndexForName(variable_name);
    return attribute_setting.getValueSettingForIndex(index);
}

void SlowCommand::setVariableValueForKey(IOCAttributeShareCache::SharedVeriableDomain domain, const char *variable_name, void * value, uint32_t size) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    VariableIndexType index = sharedAttributeSettingPtr->getSharedDomain(domain).getIndexForName(variable_name);
    sharedAttributeSettingPtr->getSharedDomain(domain).setValueForAttribute(index, value, size);
}

void SlowCommand::getVariableNames(IOCAttributeShareCache::SharedVeriableDomain domain, std::vector<std::string>& names) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    sharedAttributeSettingPtr->getSharedDomain(domain).getAttributeNames(names);
}

/*
 Send device data to output buffer
 */
void SlowCommand::pushDataSet(CDataWrapper *acquired_data) {
    //send data to related buffer
    keyDataStoragePtr->pushDataSet(acquired_data);
}

/*
 Return a new instance of CDataWrapper filled with a mandatory data
 according to key
 */
CDataWrapper *SlowCommand::getNewDataWrapper() {
    return keyDataStoragePtr->getNewDataWrapper();
}

/*
 Start the slow command sequence
 */
void SlowCommand::setHandler(CDataWrapper *data) {}

/*
 implemente thee data acquisition for the command
 */
void SlowCommand::acquireHandler() {}

/*
 Performe correlation and send command to the driver
 */
void SlowCommand::ccHandler() {}

//timeout handler
bool SlowCommand::timeoutHandler() {return true;}

//! called befor the command start the execution
void SlowCommand::command_start() {
	timing_stats.command_start_time_usec = shared_stat->lastCmdStepStart;
}

#define SET_FAULT(c, m, d) \
SL_FAULT_RUNNIG_STATE \
SCLERR_ << "Exception -> err:" << c << " msg: "<<m<<" domain:"<<d; \
faultDescription.code = c; \
faultDescription.description = m; \
faultDescription.domain = d;
//! called after the command step excecution
void SlowCommand::command_post_step() {
	if(commandFeatures.featuresFlag & features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT) {
		timing_stats.command_running_time_usec += shared_stat->lastCmdStepTime;
		//check timeout
		if(timing_stats.command_running_time_usec  > commandFeatures.featureCommandTimeout) {
			//call the timeout handler
			try {
				if(timeoutHandler()) {
					SET_FAULT(-1, "Command timeout", __FUNCTION__)
				}
			} catch(chaos::CException& ex) {
				SET_FAULT(ex.errorCode, ex.errorMessage, ex.errorDomain)
			} catch(std::exception& ex) {
				SET_FAULT(-2, ex.what(), "Acquisition Handler");
			} catch(...) {
				SET_FAULT(-3, "Unmanaged exception", "Acquisition Handler");
			}
		}
	}
}