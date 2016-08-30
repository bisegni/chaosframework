/*
 *	DatasetConsumer.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit//data_manager/trigger_system/dataset_event/DatasetConsumer.h>

using namespace chaos::common::data;
using namespace chaos::common::trigger;
using namespace chaos::cu::data_manager::trigger_system::dataset_event;

#pragma mark DatasetConsumerTest
ConsumerResult DatasetConsumerTest::consumeEvent(ETDatasetAttributeType event_type,
                                                 DatasetSubject& subject) {
    //int32_t incremnet_size = 0;
    //const CDataVariant& inc_value = getPropertyValue("property name");
    //if(inc_value.isValid()) {
        //get increment size
        //incremnet_size = inc_value.asInt32();
    //}
    
    return ConsumerResultOK;
}
