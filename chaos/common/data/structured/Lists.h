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
                CHAOS_OPEN_SDWRAPPER(ChaosStringVector)
                std::string serialization_key;
                
                void deserialize(chaos::common::data::CDataWrapper *serialized_data){
                    if(serialized_data == NULL) return;
                    Subclass::dataWrapped().clear();
                    const std::string ser_key = (serialization_key.size()==0)?"std_vector_":serialization_key;
                    if(serialized_data->hasKey(ser_key) &&
                       serialized_data->isVectorValue(ser_key)) {
                        std::unique_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(ser_key));
                        for(int idx = 0;
                            idx < serialized_array->size();
                            idx++) {
                            const std::string element(serialized_array->getStringElementAtIndex(idx));
                            Subclass::dataWrapped().push_back(element);
                        }
                    }
                }
                
                std::unique_ptr<chaos::common::data::CDataWrapper> serialize() {
                    std::unique_ptr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                    const std::string ser_key = (serialization_key.size()==0)?"std_vector_":serialization_key;
                    for(ChaosStringVectorIterator it = Subclass::dataWrapped().begin(),
                        end = Subclass::dataWrapped().end();
                        it != end;
                        it++) {
                        result->appendStringToArray(*it);
                    }
                    result->finalizeArrayForKey(ser_key);
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER();
                
                
                
                template<typename T, typename W>
                class StdVectorSDWrapper:
                public chaos::common::data::TemplatedDataSDWrapper< std::vector<T> > {
                    W embedded_data_serializer;
                public:
                    std::string serialization_key;
                    
                    StdVectorSDWrapper(std::shared_ptr< DataWrapperReference< std::vector<T> > > _data = std::shared_ptr< DataWrapperReference< std::vector<T> > >(new DataWrapperCopy< std::vector<T> >())):
                    chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >(_data),
                    serialization_key(){}

                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().clear();
                        const std::string ser_key = (serialization_key.size()==0)?"std_vector_":serialization_key;
                        if(serialized_data->hasKey(ser_key) &&
                           serialized_data->isVectorValue(ser_key)) {
                            std::unique_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(ser_key));
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                std::unique_ptr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                                embedded_data_serializer.deserialize(element.get());
                                chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().push_back(embedded_data_serializer.dataWrapped());
                            }
                        }
                    }
                    
                    std::unique_ptr<chaos::common::data::CDataWrapper> serialize() {
                        std::unique_ptr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                        const std::string ser_key = (serialization_key.size()==0)?"std_vector_":serialization_key;
                        for(typename std::vector<T>::iterator it = chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().begin(),
                            end = chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().end();
                            it != end;
                            it++) {
                            embedded_data_serializer() = *it;
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
