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

#include <chaos_metadata_service_client/api_proxy/service/SetSnapshotDatasetsForNode.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::service;

API_PROXY_CD_DEFINITION(SetSnapshotDatasetsForNode,
                        "service",
                        "setSnapshotDatasetsForNode")

ApiProxyResult SetSnapshotDatasetsForNode::execute(const std::string& snapshot_name,
                                                   const std::string& node_uid,
                                                   const std::string& dataset_key,
                                                   const chaos::common::data::CDataWrapper& dataset_value) {
    VectorDatasetValue vec;
    vec.push_back(createDatasetValue(dataset_key,
                                     dataset_value));
    return execute(snapshot_name,
                   node_uid,
                   vec);
}

ApiProxyResult SetSnapshotDatasetsForNode::execute(const std::string& snapshot_name,
                                                   const std::string& node_uid,
                                                   const VectorDatasetValue& datasets_value_vec) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, node_uid);
    message->addStringValue("snapshot_name", snapshot_name);

    for(VectorDatasetValue::const_iterator i=datasets_value_vec.begin();i!=datasets_value_vec.end();i++){
    	message->appendCDataWrapperToArray(*(i->get()));
    }
    message->finalizeArrayForKey("dataset");
    return callApi(message);
}

DatasetValueShrdPtr SetSnapshotDatasetsForNode::createDatasetValue(const std::string& dataset_key,
                                                                   const DatasetValueShrdPtr& dataset_value) {
    DatasetValueShrdPtr dataset_value_shrdptr(new CDataWrapper());
    dataset_value_shrdptr->addStringValue("dataset_key", dataset_key);
    dataset_value_shrdptr->addCSDataValue("dataset_value", *dataset_value.get());
    return dataset_value_shrdptr;
}

DatasetValueShrdPtr SetSnapshotDatasetsForNode::createDatasetValue(const std::string& dataset_key,
                                                                   const chaos::common::data::CDataWrapper& dataset_value) {
    DatasetValueShrdPtr dataset_value_shrdptr(new CDataWrapper());
    dataset_value_shrdptr->addStringValue("dataset_key", dataset_key );
    dataset_value_shrdptr->addCSDataValue("dataset_value", dataset_value);
    return dataset_value_shrdptr;
}
