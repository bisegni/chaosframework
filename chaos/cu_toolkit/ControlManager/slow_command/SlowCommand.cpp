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
namespace cccs = chaos::cu::control_manager::slow_command;

// default constructor
cccs::SlowCommand::SlowCommand() {
    featuresFlag = 0;
    runningState = RunningStateType::RS_Exsc;
    submissionRule = SubmissionRuleType::SUBMIT_NORMAL;
}

// default destructor
cccs::SlowCommand::~SlowCommand() {
    
}

/*
 return the device database with the dafualt device information
 */
chaos::cu::DeviceSchemaDB  *cccs::SlowCommand::getDeviceDatabase() {
    return deviceDatabasePtr;
}

// set the features with the ui32 value
void cccs::SlowCommand::setFeatures(cccs::FeatureFlagTypes::FeatureFlag features, uint32_t featuresValue) {
    if(featuresValue == 0) return;

    featuresFlag |= features;
    switch (features) {
        case cccs::FeatureFlagTypes::FF_SET_SCHEDULER_DELAY:
            featureSchedulerStepsDelay = featuresValue;
            break;
            
        case cccs::FeatureFlagTypes::FF_SET_SUBMISSION_RETRY:
            featureSubmissionRetryDelay = featuresValue;
            break;
            
        default:
            break;
    }
}

void cccs::SlowCommand::getChangedAttributeIndex(std::vector<AttributeIndexType>& changedIndex) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getChangedIndex(changedIndex);
}

cccs::ValueSetting *cccs::SlowCommand::getValueSetting(cccs::AttributeIndexType attributeIndex) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    return sharedAttributeSettingPtr->getValueSettingForIndex(attributeIndex);
}

cccs::ValueSetting *cccs::SlowCommand::getValueSetting(const char *keyName) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    cccs::AttributeIndexType index = sharedAttributeSettingPtr->getIndexForName(keyName);
    return sharedAttributeSettingPtr->getValueSettingForIndex(index);
}

void cccs::SlowCommand::setValueSettingForKey(const char *keyName, void * value, uint32_t size) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    cccs::AttributeIndexType index = sharedAttributeSettingPtr->getIndexForName(keyName);
    sharedAttributeSettingPtr->setValueForAttribute(index, value, size);
}

void cccs::SlowCommand::getAttributeNames(std::vector<std::string>& names) {
    CHAOS_ASSERT(sharedAttributeSettingPtr)
    sharedAttributeSettingPtr->getAttributeNames(names);
}

/*
 Send device data to output buffer
 */
void cccs::SlowCommand::pushDataSet(CDataWrapper *acquiredData) {
    //send data to related buffer
    keyDataStoragePtr->pushDataSet(acquiredData);
}

/*
 Return a new instance of CDataWrapper filled with a mandatory data
 according to key
 */
CDataWrapper *cccs::SlowCommand::getNewDataWrapper() {
    return keyDataStoragePtr->getNewDataWrapper();
}

/*
 Start the slow command sequence
 */
void cccs::SlowCommand::setHandler(chaos::CDataWrapper *data) {}

/*
 implemente thee data acquisition for the command
 */
void cccs::SlowCommand::acquireHandler() {}

/*
 Performe correlation and send command to the driver
 */
void cccs::SlowCommand::ccHandler() {}