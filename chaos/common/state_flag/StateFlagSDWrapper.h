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
            
            ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
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
                dataWrapped().flag_description.name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_NAME, "");
                //check if we have a catalog name
                ChaosStringVector splitted_name;
                boost::split( splitted_name,
                             dataWrapped().flag_description.name ,
                             boost::is_any_of("/"),
                             boost::token_compress_on);
                if(splitted_name.size() > 1) {
                    dataWrapped().flag_description.name = splitted_name[splitted_name.size()-1];
                }
                dataWrapped().flag_description.description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_DESCRIPTION, "");
                
                //decode the list fo state level
                StateLevelSDWrapper slsdw;
                dataWrapped().set_levels.clear();
                
                if(serialized_data->hasKey(NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SET) &&
                   serialized_data->isVectorValue(NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SET)) {
                    chaos::common::data::CMultiTypeDataArrayWrapperSPtr state_level_vec = serialized_data->getVectorValue(NodeStateFlagDefinitionKey::NODE_SF_LEVEL_SET);
                    for(int idx = 0;
                        idx < state_level_vec->size();
                        idx++) {
                        ChaosUniquePtr<chaos::common::data::CDataWrapper> state_level(state_level_vec->getCDataWrapperElementAtIndex(idx));
                        slsdw.deserialize(state_level.get());
                        dataWrapped().addLevel(slsdw.dataWrapped());
                    }
                }
            }
            
            ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                data_serialized->addStringValue(NodeStateFlagDefinitionKey::NODE_SF_NAME, dataWrapped().flag_description.name);
                data_serialized->addStringValue(NodeStateFlagDefinitionKey::NODE_SF_DESCRIPTION, dataWrapped().flag_description.description);
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
