//
//  DirectIOVirtualClientChannel.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 30/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

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
