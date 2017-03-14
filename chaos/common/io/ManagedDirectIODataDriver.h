/*
 *	ManagedDirectIODataDriver.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 08/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A4DDDA4_0D15_4B60_9D85_E983B249A7FE_ManagedDirectIODataDriver_h
#define __CHAOSFramework__A4DDDA4_0D15_4B60_9D85_E983B249A7FE_ManagedDirectIODataDriver_h

#include <chaos/common/io/IODirectIODriver.h>

namespace chaos{
    namespace common {
        
        namespace message {
            class MDSMessageChannel;
        }
        
        namespace io {
            
            //!thia class extend the DirectIO IODataDriver
            /*!
             this class birng funcitonality for get best endpoint
             when the driver get initilized event and can be recofigured remotelly
             by metadata service
             */
            class ManagedDirectIODataDriver:
            public IODirectIODriver {
                message::MDSMessageChannel *mds_channel;
            public:
                ManagedDirectIODataDriver();
                ~ManagedDirectIODataDriver();
                void init(void *init_parameter) throw(CException);
                void deinit() throw(CException);
                void storeLogEntries(const std::string& key,
                                     ChaosStringVector log_entries) throw(CException);
            };
        }
    }
}

#endif /* __CHAOSFramework__A4DDDA4_0D15_4B60_9D85_E983B249A7FE_ManagedDirectIODataDriver_h */
