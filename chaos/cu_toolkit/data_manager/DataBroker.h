/*
 *	DataBroker.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h
#define __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>
#include <chaos/cu_toolkit/data_manager/manipulation/DataBrokerEditor.h>

namespace chaos {
    namespace cu {
        namespace data_manager {

            //! main class for the data broker functionality
            class DataBroker {
                //!dataset editing interface
                manipulation::DataBrokerEditor editor;
                
            public:
                DataBroker();
                ~DataBroker();
                
                //!return the dataset editor
                manipulation::DataBrokerEditor& getEditor();
            };
            
        }
    }
}

#endif /* __CHAOSFramework__555554A_6C80_44FA_B263_6A03AA133D4D_DataBroker_h */
