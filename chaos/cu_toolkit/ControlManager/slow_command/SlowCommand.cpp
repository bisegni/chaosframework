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
using namespace chaos::cu::control_manager::slow_command;

//! default constructor
SlowCommand::SlowCommand() {
    runningState = RunningStateType::RS_Exsc;
    submissionRule = SubmissionRuleType::SUBMIT_NORMAL;
}

//! default destructor
SlowCommand::~SlowCommand() {
    
}

/*!
 return the device database with the dafualt device information
 */
chaos::cu::DeviceSchemaDB  *SlowCommand::getDeviceDatabase() {
    return deviceDatabasePtr;
}

ValueSetting *SlowCommand::getValueSettingForKey(const char *keyName){
    AttributeIndexType index = sharedChannelSettingPtr->getIndexForName(keyName);
    return sharedChannelSettingPtr->getValueSettingForIndex(index);
}

void SlowCommand::setValueSettingForKey(const char *keyName, void * value, uint32_t size) {
    AttributeIndexType index = sharedChannelSettingPtr->getIndexForName(keyName);
    sharedChannelSettingPtr->setValueForAttribute(index, value, size);
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
void SlowCommand::setHandler(chaos::CDataWrapper *data) {}

/*
 implemente thee data acquisition for the command
 */
void SlowCommand::acquireHandler() {}

/*
 Performe correlation and send command to the driver
 */
void SlowCommand::ccHandler() {}