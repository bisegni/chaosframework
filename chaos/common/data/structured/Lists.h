/*
 *	GeneralLists.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__8E5C901_512F_422F_97B8_B2A185BB12EE_GeneralLists_h
#define __CHAOSFramework__8E5C901_512F_422F_97B8_B2A185BB12EE_GeneralLists_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                
                //! Implementation of a SDWrapepr for a string list
                CHAOS_DEFINE_TEMPLATED_SDWRAPPER_CLASS(ChaosStringVector) {
                public:
                    const std::string serialization_postfix;
                    
                    ChaosStringVectorSDWrapper();
                    
                    ChaosStringVectorSDWrapper(const ChaosStringVector& copy_source);
                    
                    ChaosStringVectorSDWrapper(chaos::common::data::CDataWrapper *serialized_data);
                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                    
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0);                };
                
                template<typename T, typename W>
                class StdVectorSDWrapper:
                public chaos::common::data::TemplatedDataSDWrapper< std::vector<T> > {
                    W embedded_data_serializer;
                public:
                    const std::string serialization_postfix;
                    
                    StdVectorSDWrapper():
                    chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >(){}
                    
                    StdVectorSDWrapper(const T& copy_source):
                    chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >(copy_source){}
                    
                    StdVectorSDWrapper(chaos::common::data::CDataWrapper *serialized_data):
                    chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >(serialized_data){deserialize(serialized_data);}
                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().clear();
                        const std::string ser_key = "std_vector_"+serialization_postfix;
                        if(serialized_data->hasKey(ser_key) &&
                           serialized_data->isVectorValue(ser_key)) {
                            std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(ser_key));
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                std::auto_ptr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                                embedded_data_serializer.deserialize(element.get());
                                chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().push_back(embedded_data_serializer.dataWrapped());
                            }
                        }
                    }
                    
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                        std::auto_ptr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                        const std::string ser_key = "std_vector_"+serialization_postfix;
                        for(typename std::vector<T>::iterator it = chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().begin(),
                            end = chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().end();
                            it != end;
                            it++) {
                            embedded_data_serializer = *it;
                            result->appendCDataWrapperToArray(*embedded_data_serializer.serialize());
                        }
                        result->finalizeArrayForKey(ser_key);
                        return result;
                    }
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__8E5C901_512F_422F_97B8_B2A185BB12EE_GeneralLists_h */
