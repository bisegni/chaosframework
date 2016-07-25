/*
 *	StatusFlagSDWrapper.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 18/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_A4A802EC_5299_484B_AFF5_2FF3271807C2_StatusFlagSDWrapper_h
#define __CHAOSFramework_A4A802EC_5299_484B_AFF5_2FF3271807C2_StatusFlagSDWrapper_h

#include <chaos/common/status_manager/StatusFlag.h>

#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace common {
        namespace status_manager {
            //!class for serialization of state level that belong to a status flag
            CHAOS_DEFINE_TEMPLATED_DATA_SDWRAPPER_CLASS(StateLevel) {
            public:
                StateLevelSDWrapper();
                
                StateLevelSDWrapper(const StateLevel& copy_source);
                
                StateLevelSDWrapper(chaos::common::data::CDataWrapper *serialized_data);
                
                void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0);
            };
            
            //!class for serialization of status flag
            /*!
             status flag serializaiton will endoce and decode only static information
             and not runtime one, so only flag and level information will be
             encoded no listener no current value.
             */
            CHAOS_DEFINE_TEMPLATED_DATA_SDWRAPPER_CLASS(StatusFlag) {
            public:
                StatusFlagSDWrapper();
                
                StatusFlagSDWrapper(const StatusFlag& copy_source);
                
                StatusFlagSDWrapper(chaos::common::data::CDataWrapper *serialized_data);
                
                void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0);
            };
        }
    }
}

#endif /* __CHAOSFramework_A4A802EC_5299_484B_AFF5_2FF3271807C2_StatusFlagSDWrapper_h */
