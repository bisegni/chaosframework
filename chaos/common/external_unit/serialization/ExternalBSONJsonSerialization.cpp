/*
 * Copyright 2012, 21/11/2017 INFN
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
#include "ExternalBSONJsonSerialization.h"

using namespace chaos::common::data;
using namespace chaos::common::external_unit::serialization;

//serialization type
const std::string ExternalBSONJsonSerialization::ex_serialization_type = "application/json";

ExternalBSONJsonSerialization::ExternalBSONJsonSerialization(){}

ExternalBSONJsonSerialization::~ExternalBSONJsonSerialization(){}

ChaosUniquePtr<CDataBuffer> ExternalBSONJsonSerialization::serialize(const CDataWrapper& cdw_in) {
    const std::string json_result = cdw_in.getCompliantJSONString();
    return ChaosUniquePtr<CDataBuffer>(new CDataBuffer(json_result.c_str(), (uint32_t)json_result.size()));
}

ChaosUniquePtr<CDataWrapper> ExternalBSONJsonSerialization::deserialize(const CDataBuffer& cdb_in) {
    return CDataWrapper::instanceFromJson(std::string(cdb_in.getBuffer(), cdb_in.getBufferSize()));
}
