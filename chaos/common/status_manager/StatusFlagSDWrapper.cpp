/*
 *	StatusFlagSDWrapper.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 18/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/status_manager/StatusFlagSDWrapper.h>

#include <boost/algorithm/string.hpp>

using namespace chaos::common::data;
using namespace chaos::common::status_manager;

#pragma mark StateLevelSDWrapper
StateLevelSDWrapper::StateLevelSDWrapper():
StateLevelSDWrapperSubclass(){}

StateLevelSDWrapper::StateLevelSDWrapper(const StateLevel& copy_source):
StateLevelSDWrapperSubclass(copy_source){}

StateLevelSDWrapper::StateLevelSDWrapper(CDataWrapper *serialized_data):
StateLevelSDWrapperSubclass(serialized_data){deserialize(serialized_data);}

void StateLevelSDWrapper::deserialize(CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    dataWrapped().value = static_cast<int8_t>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_VALUE, 0));
    dataWrapped().tag = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_TAG, "");
    dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_DESCRIPTION, "");
    dataWrapped().severity = static_cast<StatusFlagServerity>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_SEVERITY, 0));
    
}

std::auto_ptr<CDataWrapper> StateLevelSDWrapper::serialize(const uint64_t sequence) {
    std::auto_ptr<CDataWrapper> data_serialized(new CDataWrapper());
    data_serialized->addInt32Value(chaos::NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_VALUE, static_cast<int32_t>(dataWrapped().value));
    data_serialized->addStringValue(chaos::NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_TAG, dataWrapped().tag);
    data_serialized->addStringValue(chaos::NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_DESCRIPTION, dataWrapped().description);
    data_serialized->addInt32Value(chaos::NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_SEVERITY, static_cast<StatusFlagServerity>(dataWrapped().severity));
    return data_serialized;
}

#pragma mark StatusFlagSDWrapper
StatusFlagSDWrapper::StatusFlagSDWrapper():
StatusFlagSDWrapperSubclass(){}

StatusFlagSDWrapper::StatusFlagSDWrapper(const StatusFlag& copy_source):
StatusFlagSDWrapperSubclass(copy_source){}

StatusFlagSDWrapper::StatusFlagSDWrapper(CDataWrapper *serialized_data):
StatusFlagSDWrapperSubclass(serialized_data){deserialize(serialized_data);}

void StatusFlagSDWrapper::deserialize(CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStatusFlagDefinitionKey::NODE_SF_NAME, "");
    //check if we have a catalog name
    ChaosStringVector splitted_name;
    boost::split( splitted_name,
                 dataWrapped().name ,
                 boost::is_any_of("/"),
                 boost::token_compress_on);
    if(splitted_name.size() > 1) {
        dataWrapped().name = splitted_name[splitted_name.size()-1];
    }
    dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStatusFlagDefinitionKey::NODE_SF_DESCRIPTION, "");
    
    //decode the list fo state level
    StateLevelSDWrapper slsdw;
    dataWrapped().set_levels.clear();
    
    if(serialized_data->hasKey(NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_SET) &&
       serialized_data->isVectorValue(NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_SET)) {
        std::auto_ptr<CMultiTypeDataArrayWrapper> state_level_vec(serialized_data->getVectorValue(NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_SET));
        for(int idx = 0;
            idx < state_level_vec->size();
            idx++) {
            std::auto_ptr<CDataWrapper> state_level(state_level_vec->getCDataWrapperElementAtIndex(idx));
            slsdw.deserialize(state_level.get());
            dataWrapped().addLevel(slsdw.dataWrapped());
        }
    }

}

std::auto_ptr<CDataWrapper> StatusFlagSDWrapper::serialize(const uint64_t sequence) {
    std::auto_ptr<CDataWrapper> data_serialized(new CDataWrapper());
    data_serialized->addStringValue(NodeStatusFlagDefinitionKey::NODE_SF_NAME, dataWrapped().name);
    data_serialized->addStringValue(NodeStatusFlagDefinitionKey::NODE_SF_DESCRIPTION, dataWrapped().description);
    if(dataWrapped().set_levels.size()) {
        StateLevelSDWrapper slsdw;
        //we have state level to encode
        const StatusLevelContainerOrderedIndex& src_ordered_index = dataWrapped().set_levels.get<ordered_index_tag>();
        for(StatusLevelContainerOrderedIndexConstIterator it = src_ordered_index.begin(),
            end = src_ordered_index.end();
            it != end;
            it++){
            slsdw.dataWrapped() = *it;
            //add state level serialization to the set
            data_serialized->appendCDataWrapperToArray(*slsdw.serialize());
        }
        data_serialized->finalizeArrayForKey(NodeStatusFlagDefinitionKey::NODE_SF_LEVEL_SET);
    }
    return data_serialized;
}
