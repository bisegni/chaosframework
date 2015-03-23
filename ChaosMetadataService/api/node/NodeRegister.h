/*
 *	NodeRegister.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__UnitServerRegisterApi__
#define __CHAOSFramework__UnitServerRegisterApi__

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                
                //! Implement the api for the node registration
                /*!
                 The received pack is scanned to find the unique node id and the his type.
                 If the received node is recognized as belonging to a default type, it is 
                 forwarded to the method that performs the specific registration for every type. 
                 Otherwhise the datapack will be saved in node structure without further 
                 considerations.
                 */
                class NodeRegister:
                public AbstractApi {
                    //! perform registration for specific unit server
                    chaos::common::data::CDataWrapper *unitServerRegistration(chaos::common::data::CDataWrapper *api_data,
                                                                              bool& detach_data) throw(chaos::CException);
                    
                public:
                    NodeRegister();
                    ~NodeRegister();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerRegisterApi__) */
