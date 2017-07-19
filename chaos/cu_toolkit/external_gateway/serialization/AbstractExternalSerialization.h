/*
 *	AbstractSerialization.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__36DA8C2_0830_470B_9B3F_094BDE63046E_AbstractSerialization_h
#define __CHAOSFramework__36DA8C2_0830_470B_9B3F_094BDE63046E_AbstractSerialization_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataBuffer.h>
namespace chaos{
    namespace cu {
        namespace external_gateway {
            namespace serialization {
            
                class AbstractExternalSerialization {
                    
                public:
                    AbstractExternalSerialization();
                    virtual ~AbstractExternalSerialization();
                    
                    virtual ChaosUniquePtr<chaos::common::data::CDataBuffer> serialize(const chaos::common::data::CDataWrapper& cdw_in) = 0;
                    virtual ChaosUniquePtr<chaos::common::data::CDataWrapper> deserialize(const chaos::common::data::CDataBuffer& cdb_in) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__36DA8C2_0830_470B_9B3F_094BDE63046E_AbstractSerialization_h */
