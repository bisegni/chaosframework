/*
 *	ExternalBSONExtJsonSerialization.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/07/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/external_unit/serialization/ExternalBSONExtJsonSerialization.h>

using namespace chaos::common::data;
using namespace chaos::common::external_unit::serialization;

//serialization type
const std::string ExternalBSONExtJsonSerialization::ex_serialization_type = "application/bson-json";

ExternalBSONExtJsonSerialization::ExternalBSONExtJsonSerialization(){}

ExternalBSONExtJsonSerialization::~ExternalBSONExtJsonSerialization(){}

ChaosUniquePtr<CDataBuffer> ExternalBSONExtJsonSerialization::serialize(const CDataWrapper& cdw_in) {
    const std::string json_result = cdw_in.getJSONString();
    return ChaosUniquePtr<CDataBuffer>(new CDataBuffer(json_result.c_str(), (uint32_t)json_result.size(), true));
}

ChaosUniquePtr<CDataWrapper> ExternalBSONExtJsonSerialization::deserialize(const CDataBuffer& cdb_in) {
    return CDataWrapper::instanceFromJson(std::string(cdb_in.getBuffer(), cdb_in.getBufferSize()));
}
