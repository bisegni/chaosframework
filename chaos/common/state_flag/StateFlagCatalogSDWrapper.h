/*
 *	StateFlagCatalogSDWrapper.h
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

#ifndef __CHAOSFramework_A6322B6B_D5B8_47E4_B17D_EED6342E1526_StateFlagCatalogSDWrapper_h
#define __CHAOSFramework_A6322B6B_D5B8_47E4_B17D_EED6342E1526_StateFlagCatalogSDWrapper_h

#include <chaos/common/data/TemplatedDataSDWrapper.h>
#include <chaos/common/state_flag/StateFlagSDWrapper.h>
#include <chaos/common/state_flag/StateFlagCatalog.h>

namespace chaos {
    namespace common {
        namespace state_flag {
            
            //!serialization wrapepr for status flag catalog
            CHAOS_OPEN_SDWRAPPER(StateFlagCatalog)
            
            void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                if(serialized_data == NULL) return;
                
                dataWrapped().catalog_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStateFlagDefinitionKey::NODE_SF_CATALOG_NAME, "");
                
                if(serialized_data->hasKey(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET) &&
                   serialized_data->isVectorValue(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET) ) {
                    StateFlagSDWrapper sfsdw;
                    //we have element to deserialize
                    std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> flags_vec(serialized_data->getVectorValue(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET));
                    if(flags_vec->size()){
                        for (int idx = 0;
                             idx < flags_vec->size();
                             idx++) {
                            std::auto_ptr<chaos::common::data::CDataWrapper> status_flag_element_obj_ser(flags_vec->getCDataWrapperElementAtIndex(idx));
                            if(!status_flag_element_obj_ser->hasKey(NodeStateFlagDefinitionKey::NODE_SF_COMPOSED_NAME) ||
                               !status_flag_element_obj_ser->hasKey(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_FLAG)) continue;
                            
                            //deserialize the flag
                            const std::string cat_flag_name = CDW_GET_SRT_WITH_DEFAULT(status_flag_element_obj_ser, NodeStateFlagDefinitionKey::NODE_SF_COMPOSED_NAME, "");
                            std::auto_ptr<chaos::common::data::CDataWrapper> status_flag_ser(status_flag_element_obj_ser->getCSDataValue(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_FLAG));
                            sfsdw.deserialize(status_flag_ser.get());
                            boost::shared_ptr<StateFlag> new_flag(new StateFlag(sfsdw.dataWrapped()));
                            
                            //we can insert
                            dataWrapped().catalog_container().insert(StateFlagElement::StateFlagElementPtr(new StateFlagElement((unsigned int)dataWrapped().catalog_container().size(), cat_flag_name, new_flag)));
                            
                            dataWrapped().addMemberToSeverityMap(new_flag);
                            
                            new_flag->addListener(&dataWrapped());
                        }
                    }
                }
            }
            
            std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                data_serialized->addStringValue(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_NAME, dataWrapped().catalog_name);
                
                if(dataWrapped().catalog_container().size()) {
                    StateFlagSDWrapper sfsdw;
                    
                    //we have flag to encode
                    StateFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(dataWrapped().catalog_container());
                    for(StateFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
                        end = ordered_index.end();
                        it != end;
                        it++){
                        chaos::common::data::CDataWrapper status_flag_ser;
                        status_flag_ser.addStringValue(NodeStateFlagDefinitionKey::NODE_SF_COMPOSED_NAME, (*it)->flag_name);
                        sfsdw.dataWrapped() = *(*it)->status_flag;
                        status_flag_ser.addCSDataValue(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_FLAG, *sfsdw.serialize());
                        data_serialized->appendCDataWrapperToArray(status_flag_ser);
                    }
                    data_serialized->finalizeArrayForKey(NodeStateFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET);
                }
                return data_serialized;
            }
            CHAOS_CLOSE_SDWRAPPER()
        }
    }
}

#endif /* __CHAOSFramework_A6322B6B_D5B8_47E4_B17D_EED6342E1526_StateFlagCatalogSDWrapper_h */
