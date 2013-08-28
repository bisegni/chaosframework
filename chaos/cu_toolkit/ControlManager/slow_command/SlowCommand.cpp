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

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;

// default constructor
SlowCommand::SlowCommand() {
    
	//setup feautere fields
	commandFeatures.featuresFlag = 0;
	commandFeatures.lockedOnUserModification = false;
	
    runningState = RunningStateType::RS_Exsc;
    submissionRule = SubmissionRuleType::SUBMIT_NORMAL;
}

// default destructor
SlowCommand::~SlowCommand() {
    
}

/*
 return the device database with the dafualt device information
 */
chaos::cu::DeviceSchemaDB  *SlowCommand::getDeviceDatabase() {
    return deviceDatabasePtr;
}

// set the features with the ui32 value
void SlowCommand::setFeatures(features::FeaturesFlagTypes::FeatureFlag features, uint32_t featuresValue) {
	//check if the features are locked for the user modifications
    if(commandFeatures.lockedOnUserModification) return;

    commandFeatures.featuresFlag |= features;
    switch (features) {
        case features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY:
            commandFeatures.featureSchedulerStepsDelay = featuresValue;
            break;
            
        case features::FeaturesFlagTypes::FF_SET_SUBMISSION_RETRY:
            commandFeatures.featureSubmissionRetryDelay = featuresValue;
            break;
            
        default:
            break;
    }
}

void SlowCommand::getChangedAttributeIndex(std::vector<AttributeIndexType>& changedIndex) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getChangedIndex(changedIndex);
}

ValueSetting *SlowCommand::getValueSetting(AttributeIndexType attributeIndex) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getValueSettingForIndex(attributeIndex);
}

ValueSetting *SlowCommand::getValueSetting(const char *keyName) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    AttributeIndexType index = sharedAttributeSettingPtr->getIndexForName(keyName);
    return sharedAttributeSettingPtr->getValueSettingForIndex(index);
}

void SlowCommand::setValueSettingForKey(const char *keyName, void * value, uint32_t size) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    AttributeIndexType index = sharedAttributeSettingPtr->getIndexForName(keyName);
    sharedAttributeSettingPtr->setValueForAttribute(index, value, size);
}

void SlowCommand::getAttributeNames(std::vector<std::string>& names) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    sharedAttributeSettingPtr->getAttributeNames(names);
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