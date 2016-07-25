/*
 *	Dataset.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__9AF21F8_5C6D_4BA4_B954_150112079D8D_Dataset_h
#define __CHAOSFramework__9AF21F8_5C6D_4BA4_B954_150112079D8D_Dataset_h

#include <chaos/common/chaos_constants.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/structured/DatasetAttribute.h>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(DatasetAttribute, DatasetAttributeVector);
                
                //! The description of a complete dataset with his attribute and property
                struct Dataset {
                    //is the name of the dataset
                    std::string                     name;
                    //is the postfix associated with the key that represent the dataset into the shared memory
                    std::string                     post_fix;
                    //is the type of the dataset
                    chaos::DataType::DatasetType    type;
                    //is the ocmplete list of the attribute of the dataset
                    DatasetAttributeVector          attribute_list;
                    
                    Dataset();
                    Dataset(const Dataset& copy_src);
                    Dataset& operator=(Dataset const &rhs);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__9AF21F8_5C6D_4BA4_B954_150112079D8D_Dataset_h */
