//
//  ZMQDirectIOServerDataPack.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 02/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__ZMQDirectIOServerDataPack__
#define __CHAOSFramework__ZMQDirectIOServerDataPack__

#include <chaos/common/direct_io/DirectIOServerDataPack.h>

namespace chaos {
	namespace common {
		namespace direct_io {
            
            class ZMQDirectIOServer;
            
            //! ZMQ implementation for the server data pack
            class ZMQDirectIOServerDataPack : public DirectIOServerDataPack {
                friend class ZMQDirectIOServer;
                //! dealer/rep socket
                void *zmq_socket;
                
                ZMQDirectIOServerDataPack();
                ~ZMQDirectIOServerDataPack();
                
                void listen();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__ZMQDirectIOServerDataPack__) */
