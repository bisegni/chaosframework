/*
 *	SlowCommandExecutor.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <vector>
#include <string>

#include <chaos/common/global.h>

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandExecutor.h>



using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;

namespace chaos_batch = chaos::common::batch_command;


#define LOG_HEAD_SBE "[SlowCommandExecutor-" << deviceSchemaDbPtr->getDeviceID() << "] "

#define SCELAPP_ LAPP_ << LOG_HEAD_SBE
#define SCELDBG_ LDBG_ << LOG_HEAD_SBE
#define SCELERR_ LERR_ << LOG_HEAD_SBE

using namespace chaos::common::batch_command;

SlowCommandExecutor::SlowCommandExecutor(std::string _executorID, DatasetDB *_deviceSchemaDbPtr) : BatchCommandExecutor(_executorID),deviceSchemaDbPtr(_deviceSchemaDbPtr){

}

SlowCommandExecutor::~SlowCommandExecutor() {
}

// Initialize instance
void SlowCommandExecutor::init(void *initData) throw(chaos::CException) {
    std::vector<std::string> attribute_names;
    
    //initialize superclass
	BatchCommandExecutor::init(initData);
	
    SCELAPP_ << "Populating sandbox shared setting for device input attribute";
    deviceSchemaDbPtr->getDatasetAttributesName(DataType::Input, attribute_names);
    initAttributeOnSahredVariableDomain(IOCAttributeSharedCache::SVD_INPUT, attribute_names);

	SCELAPP_ << "Populating sandbox shared setting for device output attribute";
	attribute_names.clear();
    deviceSchemaDbPtr->getDatasetAttributesName(DataType::Output, attribute_names);
    initAttributeOnSahredVariableDomain(IOCAttributeSharedCache::SVD_OUTPUT, attribute_names);


}

void SlowCommandExecutor::initAttributeOnSahredVariableDomain(IOCAttributeSharedCache::SharedVeriableDomain domain, std::vector<string>& attribute_names) {
	//add input attribute to shared setting
    RangeValueInfo attributeInfo;
	
	AttributeSetting& attribute_setting = global_attribute_cache.getSharedDomain(domain);
	
	for(int idx = 0;
        idx < attribute_names.size();
        idx++) {
        
        attributeInfo.reset();
        
        // retrive the attribute description from the device database
        deviceSchemaDbPtr->getAttributeRangeValueInfo(attribute_names[idx], attributeInfo);
        
        // add the attribute to the shared setting object
        attribute_setting.addAttribute(attribute_names[idx], attributeInfo.maxSize, attributeInfo.valueType);
        
        if(!attributeInfo.defaultValue.size()) continue;
        
        //setting value using index (the index into the sharedAttributeSetting are sequencial to the inserted order)
        switch (attributeInfo.valueType) {
            case DataType::TYPE_BOOLEAN : {
                bool val = boost::lexical_cast<bool>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(bool));
                break;}
            case DataType::TYPE_DOUBLE : {
                double val = boost::lexical_cast<double>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(double));
                break;}
            case DataType::TYPE_INT32 : {
                int32_t val = boost::lexical_cast<int32_t>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(int32_t));
                break;}
            case DataType::TYPE_INT64 : {
                int64_t val = boost::lexical_cast<int64_t>(attributeInfo.defaultValue);
                attribute_setting.setDefaultValueForAttribute(idx, &val, sizeof(int64_t));
                break;}
            case DataType::TYPE_STRING : {
                const char * val = attributeInfo.defaultValue.c_str();
                attribute_setting.setDefaultValueForAttribute(idx, val, (uint32_t)attributeInfo.defaultValue.size());
                break;}
            default:
                break;
        }
    }

}


//! Install a command associated with a type
void SlowCommandExecutor::installCommand(string alias, chaos::common::utility::NestedObjectInstancer<SlowCommand, chaos_batch::BatchCommand> *instancer) {
    //call superclss method
    BatchCommandExecutor::installCommand(alias, instancer);
}


//! Check if the waithing command can be installed
chaos_batch::BatchCommand *SlowCommandExecutor::instanceCommandInfo(std::string& commandAlias) {
    SlowCommand *result = (SlowCommand*) BatchCommandExecutor::instanceCommandInfo(commandAlias);
    if(result) {
		//forward the pointers of the needed data
		result->driverAccessorsErogator = driverAccessorsErogator;
        result->keyDataStoragePtr = keyDataStoragePtr;
        result->deviceDatabasePtr = deviceSchemaDbPtr;
    }
    return result;
}
