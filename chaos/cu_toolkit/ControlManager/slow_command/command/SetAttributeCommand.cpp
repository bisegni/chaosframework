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

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::control_manager::slow_command::command;

uint8_t SetAttributeCommand::implementedHandler() {
	return HandlerType::HT_Set;
}

#define CHECK_FOR_RANGE_VALUE(t, v, attr_name)\
t max = attribute_info.maxRange.size()?boost::lexical_cast<t>(attribute_info.maxRange):std::numeric_limits<t>::max();\
t min = attribute_info.maxRange.size()?boost::lexical_cast<t>(attribute_info.minRange):std::numeric_limits<t>::min();\
if(v < min || v > max) throw CException(-1,  boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name % attribute_info.minRange % attribute_info.maxRange).c_str(), __PRETTY_FUNCTION__);\

#define CHECK_FOR_STRING_RANGE_VALUE(v, attr_name)\
if(attribute_info.minRange.size() && v < attribute_info.minRange ) throw CException(-1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name % attribute_info.minRange % attribute_info.maxRange).c_str(), __PRETTY_FUNCTION__);\
if(attribute_info.maxRange.size() && v > attribute_info.maxRange ) throw CException(-1, boost::str(boost::format("Invalid value (%1%) [max:%2% Min:%3%] for attribute %4%") % v % attr_name %attribute_info.minRange % attribute_info.maxRange).c_str(), __PRETTY_FUNCTION__);\


// Start the command execution
void SetAttributeCommand::setHandler(CDataWrapper *data) {
	if(!data) return;
	
	if(!data->hasKey(BatchCommandsKey::ATTRIBUTE_SET_NAME) ||
	   !data->hasKey(BatchCommandsKey::ATTRIBUTE_SET_VALUE)) {
		throw CException(1, "No default key found!", __FUNCTION__);
	}
	RangeValueInfo attribute_info;
	
	//get the attribute name
	std::string name = data->getStringValue(BatchCommandsKey::ATTRIBUTE_SET_NAME);
	
	//get attribute info
	getDeviceDatabase()->getAttributeRangeValueInfo(name, attribute_info);
	
	//get the cache slot for attribute
	ValueSetting * attribute_cache_value = getSharedCacheInterface()->getVariableValue(SharedCacheInterface::SVD_INPUT, name);
	
	//manage types
	switch (attribute_cache_value->type) {
		case DataType::TYPE_BOOLEAN: {
			bool bv = data->getBoolValue(name.c_str());
			attribute_cache_value->setValue(&bv, sizeof(bool));
			break;
		}
		case DataType::TYPE_INT32: {
			int32_t i32v = data->getInt32Value(name.c_str());
			CHECK_FOR_RANGE_VALUE(int32_t, i32v, name.c_str())
			attribute_cache_value->setValue(&i32v, sizeof(int32_t));
			break;
		}
		case DataType::TYPE_INT64: {
			int64_t i64v = data->getInt64Value(name.c_str());
			CHECK_FOR_RANGE_VALUE(int64_t, i64v, name.c_str())
			attribute_cache_value->setValue(&i64v, sizeof(int64_t));
			break;
		}
		case DataType::TYPE_DOUBLE: {
			double dv = data->getDoubleValue(name.c_str());
			CHECK_FOR_RANGE_VALUE(double, dv, name.c_str())
			attribute_cache_value->setValue(&dv, sizeof(double));
			break;
		}
		case DataType::TYPE_STRING: {
			std::string str = data->getStringValue(name.c_str());
			CHECK_FOR_STRING_RANGE_VALUE(str, name.c_str())
			attribute_cache_value->setValue(str.c_str(), (uint32_t)str.size());
			break;
		}
		case DataType::TYPE_BYTEARRAY: {
			int bin_size = 0;
			const char *binv = data->getBinaryValue(name.c_str(), bin_size);
			attribute_cache_value->setValue(binv, bin_size);
			break;
		}
	}
}