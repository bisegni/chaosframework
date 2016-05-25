/*
 *	TempaltedDataHelper.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EB44C3EB_D602_48B6_A57E_FBF4555A4C6A_TempaltedDataHelper_h
#define __CHAOSFramework_EB44C3EB_D602_48B6_A57E_FBF4555A4C6A_TempaltedDataHelper_h

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace service_common {
        namespace data {
            
            //! helepr class for the managment of the serialization and deserialization of data class
            template<typename T>
            class TemplatedDataContainer {
                T data_container;
            public:
                //!constructor with the default container
                TemplatedDataContainer(){}
                TemplatedDataContainer(const T& copy_src):data_container(copy_src){}
                TemplatedDataContainer(chaos::common::data::CDataWrapper *serialized_data){}
                //deserialize encoded data in container
                virtual void deserialize(chaos::common::data::CDataWrapper *serialized_data) = 0;
                
                //!serialize the container
                virtual std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence) = 0;
                
                //!return the container
                T& container(){
                    return data_container;
                }
                
                T& operator=(T const &rhs) {
                    return (data_container = rhs);
                }
                
                TemplatedDataContainer<T>& operator=(TemplatedDataContainer<T> const &rhs) {
                    data_container = rhs.data_container;
                    return *this;
                }
            };
        }
    }
}

#endif /* __CHAOSFramework_EB44C3EB_D602_48B6_A57E_FBF4555A4C6A_TempaltedDataHelper_h */
