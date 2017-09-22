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
