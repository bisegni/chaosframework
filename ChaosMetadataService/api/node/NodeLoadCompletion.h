/*
 *	NodeLoadCompletion.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFrameowrk__NodeLoadCompletion_h
#define __CHAOSFrameowrk__NodeLoadCompletion_h

#include "../AbstractApi.h"
namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                //! Implement the api for the node load completion phase
                /*!
                 
                 */
                class NodeLoadCompletion:
                public AbstractApi {
                    //! perform the completion for control unit type node
                    chaos::common::data::CDataWrapper *controlUnitCompletion(chaos::common::data::CDataWrapper *api_data,
                                                                             bool& detach_data) throw(chaos::CException);
                public:
                    NodeLoadCompletion();
                    ~NodeLoadCompletion();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException);
                };
                
            }
        }
    }
}
#endif /* NodeLoadCompletion_hpp */
