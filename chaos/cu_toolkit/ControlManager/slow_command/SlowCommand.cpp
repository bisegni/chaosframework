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

#define LOG_HEAD_SL "[SlowCommand-" << device_id << "-" << unique_id << "] "
#define SCLAPP_ LAPP_ << LOG_HEAD_SL
#define SCLDBG_ LDBG_ << LOG_HEAD_SL
#define SCLERR_ LERR_ << LOG_HEAD_SL

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
    runningProperty = RunningPropertyType::RP_Normal;
    submissionRule = SubmissionRuleType::SUBMIT_NORMAL;
}

// default destructor
SlowCommand::~SlowCommand() {
    
}

std::string& SlowCommand::getDeviceID() {
    return device_id;
}

//return the unique id
uint64_t SlowCommand::getUID() {
	return unique_id;
}

/*
 return the device database with the dafualt device information
 */
chaos::common::data::DatasetDB *SlowCommand::getDeviceDatabase() {
    return deviceDatabasePtr;
}

void SlowCommand::getChangedVariableIndex(IOCAttributeSharedCache::SharedVeriableDomain domain, std::vector<VariableIndexType>& changed_index) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getSharedDomain(domain).getChangedIndex(changed_index);
}

ValueSetting *SlowCommand::getVariableValue(IOCAttributeSharedCache::SharedVeriableDomain domain, VariableIndexType variable_index) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getSharedDomain(domain).getValueSettingForIndex(variable_index);
}

ValueSetting *SlowCommand::getVariableValue(IOCAttributeSharedCache::SharedVeriableDomain domain, const char *variable_name) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
	AttributeSetting& attribute_setting = sharedAttributeSettingPtr->getSharedDomain(domain);
    VariableIndexType index = attribute_setting.getIndexForName(variable_name);
    return attribute_setting.getValueSettingForIndex(index);
}

void SlowCommand::setVariableValueForKey(IOCAttributeSharedCache::SharedVeriableDomain domain, const char *variable_name, void * value, uint32_t size) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    VariableIndexType index = sharedAttributeSettingPtr->getSharedDomain(domain).getIndexForName(variable_name);
    sharedAttributeSettingPtr->getSharedDomain(domain).setValueForAttribute(index, value, size);
}

void SlowCommand::getVariableNames(IOCAttributeSharedCache::SharedVeriableDomain domain, std::vector<std::string>& names) {
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
void SlowCommand::commandPre() {
	timing_stats.command_set_time_usec = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now().time_since_epoch()).count();
}

#define SET_FAULT(c, m, d) \
SL_FAULT_RUNNIG_STATE \
SCLERR_ << "Exception -> err:" << c << " msg: "<<m<<" domain:"<<d; \
faultDescription.code = c; \
faultDescription.description = m; \
faultDescription.domain = d;
//! called after the command step excecution
void SlowCommand::commandPost() {
	if(commandFeatures.featuresFlag & features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT) {
            //timing_stats.command_running_time_usec += shared_stat->lastCmdStepTime;
		//check timeout
		if((shared_stat->lastCmdStepStart - timing_stats.command_set_time_usec)  >= commandFeatures.featureCommandTimeout) {
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

// return the set handler time
uint64_t SlowCommand::getSetTime() {
    return timing_stats.command_set_time_usec;
}
//! return the start step time of the sandbox
uint64_t SlowCommand::getStartStepTime() {
    return shared_stat->lastCmdStepStart;
}

//! return the last step time of the sandbox
uint64_t SlowCommand::getLastStepTime() {
    return shared_stat->lastCmdStepStart;
}