/*
 *	StateFlagSDWrapper.h
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

#ifndef __CHAOSFramework_A4A802EC_5299_484B_AFF5_2FF3271807C2_StateFlagSDWrapper_h
#define __CHAOSFramework_A4A802EC_5299_484B_AFF5_2FF3271807C2_StateFlagSDWrapper_h

#include <chaos/common/state_flag/StateFlag.h>

#include <chaos/common/data/TemplatedDataSDWrapper.h>

#include <boost/algorithm/string.hpp>

namespace chaos {
    namespace common {
        namespace state_flag {
            //!class for serialization of state level that belong to a status flag
            CHAOS_OPEN_SDWRAPPER(StateLevel)
            void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                if(serialized_data == NULL) return;
                dataWrapped().value = static_cast<int8_t>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, chaos::NodeStateFlagDefinitionKey::NODE_SF_LEVEL_VALUE, 0));
                dataWrapped().tag = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_LEVEL_TAG, "");
                dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_LEVEL_DESCRIPTION, "");
                dataWrapped().severity = static_cast<StateFlagServerity>(CDW_GET_INT32_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SEVERITY, 0));
                
            }
            
            std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                data_serialized->addInt32Value(chaos::NodeStateFlagDefinitionKey::NODE_SF_LEVEL_VALUE, static_cast<int32_t>(dataWrapped().value));
                data_serialized->addStringValue(chaos::NodeStateFlagDefinitionKey::NODE_SF_LEVEL_TAG, dataWrapped().tag);
                data_serialized->addStringValue(chaos::NodeStateFlagDefinitionKey::NODE_SF_LEVEL_DESCRIPTION, dataWrapped().description);
                data_serialized->addInt32Value(chaos::NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SEVERITY, static_cast<StateFlagServerity>(dataWrapped().severity));
                return data_serialized;
            }
            CHAOS_CLOSE_SDWRAPPER()
            
            
            //!class for serialization of status flag
            /*!
             status flag serializaiton will endoce and decode only static information
             and not runtime one, so only flag and level information will be
             encoded no listener no current value.
             */
            CHAOS_OPEN_SDWRAPPER(StateFlag)
            void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                if(serialized_data == NULL) return;
                dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_NAME, "");
                //check if we have a catalog name
                ChaosStringVector splitted_name;
                boost::split( splitted_name,
                             dataWrapped().name ,
                             boost::is_any_of("/"),
                             boost::token_compress_on);
                if(splitted_name.size() > 1) {
                    dataWrapped().name = splitted_name[splitted_name.size()-1];
                }
                dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_DESCRIPTION, "");
                
                //decode the list fo state level
                StateLevelSDWrapper slsdw;
                dataWrapped().set_levels.clear();
                
                if(serialized_data->hasKey(NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SET) &&
                   serialized_data->isVectorValue(NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SET)) {
                    std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> state_level_vec(serialized_data->getVectorValue(NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SET));
                    for(int idx = 0;
                        idx < state_level_vec->size();
                        idx++) {
                        std::auto_ptr<chaos::common::data::CDataWrapper> state_level(state_level_vec->getCDataWrapperElementAtIndex(idx));
                        slsdw.deserialize(state_level.get());
                        dataWrapped().addLevel(slsdw.dataWrapped());
                    }
                }
                
            }
            
            std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                data_serialized->addStringValue(NodeStateFlagDefinitionKey::NODE_SF_NAME, dataWrapped().name);
                data_serialized->addStringValue(NodeStateFlagDefinitionKey::NODE_SF_DESCRIPTION, dataWrapped().description);
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
                    data_serialized->finalizeArrayForKey(NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SET);
                }
                return data_serialized;
            }
            CHAOS_CLOSE_SDWRAPPER()
            
        }
    }
}

#endif /* __CHAOSFramework_A4A802EC_5299_484B_AFF5_2FF3271807C2_StateFlagSDWrapper_h */
