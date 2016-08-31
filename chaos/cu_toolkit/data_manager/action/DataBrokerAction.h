/*
 *	DataBrokerAction.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 31/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__68B3148_7F28_4B08_B4F3_C7DE793FD75C_DataBrokerAction_h
#define __CHAOSFramework__68B3148_7F28_4B08_B4F3_C7DE793FD75C_DataBrokerAction_h

#include <chaos/common/utility/NamedService.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace action {
                
                class DataBrokerAction:
                public chaos::common::utility::NamedService {
                    
                public:
                    DataBrokerAction(const std::string& action_name);
                    ~DataBrokerAction();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__68B3148_7F28_4B08_B4F3_C7DE793FD75C_DataBrokerAction_h */
