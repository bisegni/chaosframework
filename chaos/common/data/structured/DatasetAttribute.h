/*
 *	DatasetAttribute.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 31/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_DD3CE2CE_DFAD_42AA_8C8E_8AC82B03C5FD_DatasetAttribute_h
#define __CHAOSFramework_DD3CE2CE_DFAD_42AA_8C8E_8AC82B03C5FD_DatasetAttribute_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                CHAOS_DEFINE_VECTOR_FOR_TYPE(unsigned int, DatasetSubtypeList)
                
                //! The description of a n attribute of a CHAOS dataset
                struct DatasetAttribute {
                    std::string                                     name;
                    std::string                                     description;
                    std::string                                     min_value;
                    std::string                                     max_value;
                    std::string                                     default_value;
                    chaos::DataType::DataSetAttributeIOAttribute    direction;
                    chaos::DataType::DataType                       type;
                    DatasetSubtypeList                              binary_subtype_list;
                    uint32_t                                        binary_cardinality;
                    
                    DatasetAttribute();
                    DatasetAttribute(const DatasetAttribute& copy_src);
                    DatasetAttribute& operator=(DatasetAttribute const &rhs);
                };
                
                
                //!helper for create or read the script description
                CHAOS_DEFINE_TEMPLATED_DATA_SDWRAPPER_CLASS(DatasetAttribute) {
                public:
                    DatasetAttributeSDWrapper();
                    
                    DatasetAttributeSDWrapper(const DatasetAttribute& copy_source);
                    
                    DatasetAttributeSDWrapper(chaos::common::data::CDataWrapper *serialized_data);
                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                    
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0);
                };
                
                //!a list of a script base information usefullt for search operation
                CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(DatasetAttribute,
                                                      DatasetAttributeSDWrapper,
                                                      DatasetAttributeListWrapper);
                
                
                //Definition of dataset attribute list
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::data::structured::DatasetAttribute,
                                             DatasetAttributeList);
            }
        }
    }
}

#endif /* __CHAOSFramework_DD3CE2CE_DFAD_42AA_8C8E_8AC82B03C5FD_DatasetAttribute_h */
