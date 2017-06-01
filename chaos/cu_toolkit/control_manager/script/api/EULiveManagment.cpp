/*
 *	EULiveManagment.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/05/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/control_manager/script/api/EULiveManagment.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::common::network;

using namespace chaos::cu::data_manager;
using namespace chaos::cu::control_manager::script;
using namespace chaos::cu::control_manager::script::api;

#define EUSW_LAPP    INFO_LOG_1_P(EULiveManagment, eu_instance->getCUID())
#define EUSW_DBG     DBG_LOG_1_P(EULiveManagment, eu_instance->getCUID())
#define EUSW_LERR    ERR_LOG_1_P(EULiveManagment, eu_instance->getCUID())

#define EULM_FETCH              "fetch"
#define EULM_GET_ATTR_VALUE     "getValueForKey"
#define EULM_CLEAR_LOCAL_CACHE  "clear"

EULiveManagment::EULiveManagment(ScriptableExecutionUnit *_eu_instance):
TemplatedAbstractScriptableClass(this,
                                 "live"),
eu_instance(_eu_instance) {
    addApi(EULM_FETCH, &EULiveManagment::fetch);
    addApi(EULM_GET_ATTR_VALUE, &EULiveManagment::getValueForKey);
    addApi(EULM_CLEAR_LOCAL_CACHE, &EULiveManagment::clear);
}

EULiveManagment::~EULiveManagment() {}

int EULiveManagment::fetch(const ScriptInParam& input_parameter,
                           ScriptOutParam& output_parameter) {
    if(input_parameter.size() < 2) {
        return -1;
    }
    
    int err = 0;
    VectorCDWShrdPtr found_dataset;
    ChaosStringVector node_uid_to_fetch;
    try {
        const KeyDataStorageDomain domain_type = static_cast<KeyDataStorageDomain>(input_parameter[0].asInt32());
        for(ScriptInParamConstIterator it =  (input_parameter.begin()+1),
            end = input_parameter.end();
            it != end;
            it++) {
            node_uid_to_fetch.push_back(*it);
        }
        
        if((err = eu_instance->performLiveFetch(node_uid_to_fetch,
                                                domain_type,
                                                found_dataset)) == 0) {
            for(int elemend_index = 0;
                elemend_index < found_dataset.size();
                elemend_index++) {
                //create the key
                const NodeDomainPair map_key(input_parameter[elemend_index+1],
                                             domain_type);
                map_node_ds[map_key] = found_dataset[elemend_index];
            }
            
        }
    } catch(...) {
        return -2;
    }
    return err;
}

int EULiveManagment::getValueForKey(const ScriptInParam& input_parameter,
                                    ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 3) {
        return -1;
    }
    const KeyDataStorageDomain domain = static_cast<KeyDataStorageDomain>(input_parameter[0].asInt32());
    const std::string node_uid = input_parameter[1];
    const std::string attribute_name = input_parameter[2];
    
    const NodeDomainPair map_key(node_uid,
                                 domain);
    MapNodeDomainDSIterator it_ds = map_node_ds.find(map_key);
    if(it_ds == map_node_ds.end()) {
        EUSW_LERR << CHAOS_FORMAT("The dataset for node %1% and domain %2% has not been fetched", %node_uid%domain);
        return -2;
    }
    
    if(it_ds->second->hasKey(attribute_name) == false) {
        EUSW_LERR << CHAOS_FORMAT("The attribute name %1% is not present in dataset of %2%", %attribute_name%node_uid);
    }
    
    output_parameter.push_back(it_ds->second->getVariantValue(attribute_name));
    return 0;
}

int EULiveManagment::clear(const chaos::common::script::ScriptInParam& input_parameter,
                           chaos::common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() < 2) {
        return -1;
    }
    try {
        const KeyDataStorageDomain domain_type = static_cast<KeyDataStorageDomain>(input_parameter[0].asInt32());
        for(ScriptInParamConstIterator it =  (input_parameter.begin()+1),
            end = input_parameter.end();
            it != end;
            it++) {
            map_node_ds.erase(NodeDomainPair(*it,
                                             domain_type));
        }
    } catch(...) {
        return -2;
    }
    return 0;
}
