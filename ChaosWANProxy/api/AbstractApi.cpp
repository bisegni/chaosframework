/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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