/*
 *	StatusFlagCatalogSDWrapper.cpp
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

#include <chaos/common/status_manager/StatusFlagCatalogSDWrapper.h>
#include <chaos/common/status_manager/StatusFlagSDWrapper.h>

using namespace chaos::common::data;
using namespace chaos::common::status_manager;

#pragma mark StatusFlagSDWrapper
StatusFlagCatalogSDWrapper::StatusFlagCatalogSDWrapper():
StatusFlagCatalogSDWrapperSubclass(){}

StatusFlagCatalogSDWrapper::StatusFlagCatalogSDWrapper(const StatusFlagCatalog& copy_source):
StatusFlagCatalogSDWrapperSubclass(copy_source){}

StatusFlagCatalogSDWrapper::StatusFlagCatalogSDWrapper(CDataWrapper *serialized_data):
StatusFlagCatalogSDWrapperSubclass(serialized_data){deserialize(serialized_data);}

void StatusFlagCatalogSDWrapper::deserialize(CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    
    dataWrapped().catalog_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_NAME, "");
    
    if(serialized_data->hasKey(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET) &&
       serialized_data->isVectorValue(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET) ) {
        StatusFlagSDWrapper sfsdw;
        //we have element to deserialize
        std::auto_ptr<CMultiTypeDataArrayWrapper> flags_vec(serialized_data->getVectorValue(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET));
        if(flags_vec->size()){
            for (int idx = 0;
                 idx < flags_vec->size();
                 idx++) {
                std::auto_ptr<CDataWrapper> status_flag_element_obj_ser(flags_vec->getCDataWrapperElementAtIndex(idx));
                if(!status_flag_element_obj_ser->hasKey(NodeStatusFlagDefinitionKey::NODE_SF_COMPOSED_NAME) ||
                   !status_flag_element_obj_ser->hasKey(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_FLAG)) continue;
                
                //deserialize the flag
                const std::string cat_flag_name = CDW_GET_SRT_WITH_DEFAULT(status_flag_element_obj_ser, NodeStatusFlagDefinitionKey::NODE_SF_COMPOSED_NAME, "");
                std::auto_ptr<CDataWrapper> status_flag_ser(status_flag_element_obj_ser->getCSDataValue(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_FLAG));
                sfsdw.deserialize(status_flag_ser.get());
                boost::shared_ptr<StatusFlag> new_flag(new StatusFlag(sfsdw.dataWrapped()));
                
                //we can insert
                dataWrapped().catalog_container.insert(StatusFlagElement::StatusFlagElementPtr(new StatusFlagElement((unsigned int)dataWrapped().catalog_container.size(), cat_flag_name, new_flag)));
                
                dataWrapped().addMemberToSeverityMap(new_flag);
                
                new_flag->addListener(&dataWrapped());
            }
        }
    }
}


std::auto_ptr<CDataWrapper> StatusFlagCatalogSDWrapper::serialize() {
    std::auto_ptr<CDataWrapper> data_serialized(new CDataWrapper());
    data_serialized->addStringValue(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_NAME, dataWrapped().catalog_name);
    
    if(dataWrapped().catalog_container.size()) {
        StatusFlagSDWrapper sfsdw;
        
        //we have flag to encode
        StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(dataWrapped().catalog_container);
        for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
            end = ordered_index.end();
            it != end;
            it++){
            CDataWrapper status_flag_ser;
            status_flag_ser.addStringValue(NodeStatusFlagDefinitionKey::NODE_SF_COMPOSED_NAME, (*it)->flag_name);
            sfsdw.dataWrapped() = *(*it)->status_flag;
            status_flag_ser.addCSDataValue(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_FLAG, *sfsdw.serialize());
            data_serialized->appendCDataWrapperToArray(status_flag_ser);
        }
        data_serialized->finalizeArrayForKey(NodeStatusFlagDefinitionKey::NODE_SF_CATALOG_FLAG_SET);
    }
    return data_serialized;
}
