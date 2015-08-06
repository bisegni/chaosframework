/*
 *	AbstractApi.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "AbstractApi.h"
#include <boost/lexical_cast.hpp>
#include <chaos/common/bson/util/base64.h>
using namespace chaos::wan_proxy::api;

AbstractApi::AbstractApi(const std::string& name,
						 persistence::AbstractPersistenceDriver *_persistence_driver):
PersistenceAccessor(_persistence_driver),
NamedService(name){}

AbstractApi::~AbstractApi(){}

int AbstractApi::setValueFromString(chaos::common::data::CDataWrapper& dataset,
									const std::string& type,
									const std::string& attribute_name,
									const std::string& value) {
	int err = 0;
	if(type.compare("int32") == 0) {
		dataset.addInt32Value(attribute_name.c_str(),
							  boost::lexical_cast<int32_t>(value));
	}else if(type.compare("int64") == 0) {
		dataset.addInt64Value(attribute_name.c_str(),
							  boost::lexical_cast<int64_t>(value));
	}else if(type.compare("double") == 0) {
		dataset.addDoubleValue(attribute_name.c_str(),
							   boost::lexical_cast<double>(value));
	}else if(type.compare("string") == 0) {
		dataset.addStringValue(attribute_name.c_str(),
							   value);
	}else if(type.compare("binary") == 0) {
		std::string decoded_binary = bson::base64::decode(value);
		dataset.addBinaryValue(attribute_name.c_str(),
							   decoded_binary.c_str(),
							   (uint32_t)decoded_binary.size());
	}else if(type.compare("boolean") == 0) {
		dataset.addBoolValue(attribute_name.c_str(),
							 boost::lexical_cast<bool>(value));
	}else{
		return -1;
	}
	return err;
}