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

// default constructor
SlowCommand::SlowCommand() {
	//reset all lock flag
    lockFeaturePropertyFlag.reset();
	//setup feautere fields
	std::memset(&commandFeatures, 0,sizeof(features::Features));
	commandFeatures.featuresFlag = 0;
	//commandFeatures.lockedOnUserModification = false;
	
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

void SlowCommand::getChangedAttributeIndex(IOCAttributeShareCache::SharedVeriableDomain domain, std::vector<AttributeIndexType>& changedIndex) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getSharedDomain(domain).getChangedIndex(changedIndex);
}

ValueSetting *SlowCommand::getValueSetting(IOCAttributeShareCache::SharedVeriableDomain domain, AttributeIndexType attributeIndex) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getSharedDomain(domain).getValueSettingForIndex(attributeIndex);
}

ValueSetting *SlowCommand::getValueSetting(IOCAttributeShareCache::SharedVeriableDomain domain, const char *keyName) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
	AttributeSetting& attribute_setting = sharedAttributeSettingPtr->getSharedDomain(domain);
    AttributeIndexType index = attribute_setting.getIndexForName(keyName);
    return attribute_setting.getValueSettingForIndex(index);
}

void SlowCommand::setValueSettingForKey(IOCAttributeShareCache::SharedVeriableDomain domain, const char *keyName, void * value, uint32_t size) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    AttributeIndexType index = sharedAttributeSettingPtr->getSharedDomain(domain).getIndexForName(keyName);
    sharedAttributeSettingPtr->getSharedDomain(domain).setValueForAttribute(index, value, size);
}

void SlowCommand::getAttributeNames(IOCAttributeShareCache::SharedVeriableDomain domain, std::vector<std::string>& names) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    sharedAttributeSettingPtr->getSharedDomain(domain).getAttributeNames(names);
}

/*
 Send device data to output buffer
 */
void SlowCommand::pushDataSet(CDataWrapper *acquiredData) {
    //send data to related buffer
    keyDataStoragePtr->pushDataSet(acquiredData);
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