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
                        ChaosUniquePtr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(ser_key));
                        for(int idx = 0;
                            idx < serialized_array->size();
                            idx++) {
                            const std::string element(serialized_array->getStringElementAtIndex(idx));
                            Subclass::dataWrapped().push_back(element);
                        }
                    }
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
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
                    typedef typename std::vector<T>::iterator iterator;
                    typedef typename std::vector<T>::const_iterator const_iterator;
                    std::string serialization_key;
                    StdVectorSDWrapper(ChaosSharedPtr< DataWrapperReference< std::vector<T> > > _data =ChaosSharedPtr< DataWrapperReference< std::vector<T> > >(new DataWrapperCopy< std::vector<T> >())):
                    chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >(_data),
                    serialization_key(){}

                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().clear();
                        const std::string ser_key = (serialization_key.size()==0)?"std_vector_":serialization_key;
                        if(serialized_data->hasKey(ser_key) &&
                           serialized_data->isVectorValue(ser_key)) {
                            ChaosUniquePtr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(ser_key));
                            for(int idx = 0;
                                idx < serialized_array->size();
                                idx++) {
                                ChaosUniquePtr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                                embedded_data_serializer.deserialize(element.get());
                                chaos::common::data::TemplatedDataSDWrapper< std::vector<T> >::dataWrapped().push_back(embedded_data_serializer.dataWrapped());
                            }
                        }
                    }
                    
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                        ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
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
