/*
 *	ExternalBSONExtJsonSerialization.h
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

#ifndef __CHAOSFramework__5D1E9AF_B79A_4FD7_89CC_3CD7D8E7925B_ExternalBSONExtJsonSerialization_h
#define __CHAOSFramework__5D1E9AF_B79A_4FD7_89CC_3CD7D8E7925B_ExternalBSONExtJsonSerialization_h

#include <chaos/cu_toolkit/external_gateway/serialization/AbstractExternalSerialization.h>
namespace chaos{
    namespace cu {
        namespace external_gateway {
            namespace serialization {
                
                //!serialization class that convert CDataWrapper to/from Bson extended serialization
                /*!
                 https://docs.mongodb.com/manual/reference/mongodb-extended-json/
                 */
                class ExternalBSONExtJsonSerialization:
                public AbstractExternalSerialization {
                    
                public:
                    //serialization type
                    static const std::string ex_serialization_type;
                    
                    ExternalBSONExtJsonSerialization();
                    virtual ~ExternalBSONExtJsonSerialization();
                    
                    virtual ChaosUniquePtr<chaos::common::data::CDataBuffer> serialize(const chaos::common::data::CDataWrapper& cdw_in);
                    virtual ChaosUniquePtr<chaos::common::data::CDataWrapper> deserialize(const chaos::common::data::CDataBuffer& cdb_in);
                };
                
            }
        }
    }
}
#endif /* __CHAOSFramework__5D1E9AF_B79A_4FD7_89CC_3CD7D8E7925B_ExternalBSONExtJsonSerialization_h */
