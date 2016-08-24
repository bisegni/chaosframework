/*
 *	EventManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/trigger_system/EventManager.h>

#define INFO    INFO_LOG(EventManager)
#define DBG     DBG_LOG(EventManager)
#define ERR     ERR_LOG(EventManager)

using namespace chaos::common::utility;
using namespace chaos::common::data::cache;
using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::trigger_system;
using namespace chaos::cu::data_manager::trigger_system::dataset_event;

EventManager::EventManager(chaos::common::utility::LockableObject<DatasetElementContainer>& _container_dataset):
container_dataset(_container_dataset){}

EventManager::~EventManager() {}

int EventManager::registerDatasetAttributeAsSubject() {
    //read lock on container
    LockableObjectReadLock rl;
    container_dataset.getReadLock(rl);
    
    //we can use the container
    DECOrderedIndex&   ds_index_ordered = container_dataset().get<DatasetElementTagOrderedId>();

    //we can proceed to scan dataset and attribute
    for (DECOrderedIndexIterator it = ds_index_ordered.begin(),
         end = ds_index_ordered.end();
         it != end;
         it++) {
        INFO << CHAOS_FORMAT("Create subject for dataset %1%", %(*it)->dataset->getDatasetName());
        
        for (DECOrderedIndexConstIterator it_attribute = (*it)->dataset->getOrderedBeginIterator(),
             end_attribute = (*it)->dataset->getOrderedEndIterator();
             it_attribute != end_attribute;
             it_attribute++) {
            INFO << CHAOS_FORMAT("Creating event subject for attribute %1%[%2%]", %(*it_attribute)->dataset_attribute->name%(*it)->dataset->getDatasetName());
            if((*it)->dataset_value_cache.hasName((*it_attribute)->dataset_attribute->name)) {
                ERR << CHAOS_FORMAT("No cache found for attribute %1%[%2%]", %(*it_attribute)->dataset_attribute->name%(*it)->dataset->getDatasetName());
                continue;
            }
            
            //get the subject parameter pointer
            DatasetAttribute *attribute_ptr = (*it_attribute)->dataset_attribute.get();
            if(attribute_ptr == NULL) {
                ERR << CHAOS_FORMAT("DatasetAttribute pointer for attribute %1%[%2%] is null", %(*it_attribute)->dataset_attribute->name%(*it)->dataset->getDatasetName());
                continue;
            }
            
            AttributeValue *attribute_cache_value_ptr = (*it)->dataset_value_cache.getValueSettingByName((*it_attribute)->dataset_attribute->name);
            if(attribute_cache_value_ptr == NULL) {
                ERR << CHAOS_FORMAT("AttributeValue pointer for attribute %1%[%2%]", %(*it_attribute)->dataset_attribute->name%(*it)->dataset->getDatasetName());
                continue;
            }
            //create subject class
            DatasetRegisterEnviroment::SubjectInstanceShrdPtr subjec_for_attribute(boost::make_shared<DatasetSubject>(attribute_ptr, attribute_cache_value_ptr));
            
            //register subject within environment
            dataset_trigger_environment.registerSubject(subjec_for_attribute);
            INFO << CHAOS_FORMAT("Event subject for attribute %1%[%2%] has been registered", %(*it_attribute)->dataset_attribute->name%(*it)->dataset->getDatasetName());
        }
    }
    return 0;
}
