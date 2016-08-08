/*
 *	DatasetSubject.h
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

#ifndef __CHAOSFramework_CDACC42E_F9DD_4ADE_A55A_04DFD76CC498_DatasetSubject_h
#define __CHAOSFramework_CDACC42E_F9DD_4ADE_A55A_04DFD76CC498_DatasetSubject_h

#include <chaos/common/trigger/trigger.h>
#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/common/data/structured/DatasetAttribute.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    
                    class DatasetSubject:
                    public chaos::common::trigger::AbstractSubject {
                        chaos::common::data::structured::DatasetAttribute *attribute_description;
                        chaos::common::data::cache::AttributeValue *attribute_cache;
                    public:
                        DatasetSubject(chaos::common::data::structured::DatasetAttribute *_attribute_description,
                                       chaos::common::data::cache::AttributeValue *_attribute_cache);
                        ~DatasetSubject();
                        
                        chaos::common::data::structured::DatasetAttribute * getDatasetDescirption();
                        chaos::common::data::cache::AttributeValue * getDatasetCache();
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_CDACC42E_F9DD_4ADE_A55A_04DFD76CC498_DatasetSubject_h */
