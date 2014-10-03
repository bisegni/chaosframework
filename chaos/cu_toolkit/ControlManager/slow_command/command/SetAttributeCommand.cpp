/*
 *	SetAttributeCommand.cpp
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

#include <string>
#include <chaos/cu_toolkit/ControlManager/slow_command/command/SetAttributeCommand.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::control_manager::slow_command::command;

uint8_t SetAttributeCommand::implementedHandler() {
    return HandlerType::HT_Set;
}

// Start the command execution
void SetAttributeCommand::setHandler(CDataWrapper *data) {
    if(!data) return;
    
    if(!data->hasKey(BatchCommandsKey::ATTRIBUTE_SET_NAME) ||
       !data->hasKey(BatchCommandsKey::ATTRIBUTE_SET_VALUE)) {
        throw CException(1, "No default key found!", __FUNCTION__);
    }
    
    //we can do the work
    
    std::string name = data->getStringValue(BatchCommandsKey::ATTRIBUTE_SET_NAME);
	//get the name of the input shared variable
    ValueSetting *vs = getVariableValue(AttributeValueSharedCache::SVD_INPUT, name.c_str());
    
    switch (vs->type) {
        case chaos::DataType::TYPE_BOOLEAN:{
            bool v = data->getBoolValue(BatchCommandsKey::ATTRIBUTE_SET_VALUE);
            vs->setNextValue(&v, 1);
            break;}
        case chaos::DataType::TYPE_BYTEARRAY:{
            int bin_size = 0;
            const char * v = data->getBinaryValue(BatchCommandsKey::ATTRIBUTE_SET_VALUE, bin_size);
            vs->setNextValue(static_cast<const void*>(v), bin_size);
            break;}
        case chaos::DataType::TYPE_DOUBLE:{
            double v = data->getDoubleValue(BatchCommandsKey::ATTRIBUTE_SET_VALUE);
            vs->setNextValue(&v, sizeof(double));
            break;}
        case chaos::DataType::TYPE_INT32:{
            int32_t v = data->getInt32Value(BatchCommandsKey::ATTRIBUTE_SET_VALUE);
            vs->setNextValue(&v, sizeof(int32_t));
            break;}
        case chaos::DataType::TYPE_INT64:{
            int64_t v = data->getInt64Value(BatchCommandsKey::ATTRIBUTE_SET_VALUE);
            vs->setNextValue(&v, sizeof(int64_t));
            break;}
        case chaos::DataType::TYPE_STRING:{
            string v = data->getStringValue(BatchCommandsKey::ATTRIBUTE_SET_VALUE);
            vs->setNextValue(v.c_str(), sizeof(int64_t));
            break;}
        case chaos::DataType::TYPE_STRUCT:
            break;

        default:
            break;
    }
}