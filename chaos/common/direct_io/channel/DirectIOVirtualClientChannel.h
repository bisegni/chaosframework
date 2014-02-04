/*
 *	DirectIOVirtualChannel.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__DirectIOVirtualChannel__
#define __CHAOSFramework__DirectIOVirtualChannel__

namespace chaos {
	namespace common {
		namespace direct_io {
                class DirectIOClient;
                class DirectIOForwarder;
            
            namespace channel {
                
                class DirectIOVirtualClientChannel {
                    friend class chaos::common::direct_io::DirectIOClient;
                    
                    unsigned int channel_index;
                    
                    chaos::common::direct_io::DirectIOForwarder *client_instance;
                    
                    DirectIOVirtualClientChannel();
                    ~DirectIOVirtualClientChannel();
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualChannel__) */
