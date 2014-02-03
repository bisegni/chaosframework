//
//  ZMQDirectIOClinet.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__ZMQDirectIOClinet__
#define __CHAOSFramework__ZMQDirectIOClinet__

#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/thread.hpp>

namespace chaos {
	namespace common {
		namespace direct_io {
            namespace impl {
                REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(ZMQDirectIOClient, DirectIOClient) {
                    int32_t priority_port;
					
					int32_t service_port;

                    boost::shared_mutex mutex_socket_manipolation;
					
					void *zmq_context;
					
                    void *priority_socket;
                    
                    void *service_socket;
                    
                    inline uint32_t writeToSocket(void *socket, void *data_buffer, uint32_t data_size);
                    
                    inline uint32_t readFromSocket(void *socket, void **data_buffer, uint32_t *data_size);
                    
                    //set the spread functionality on zmq socket
                    void switchMode(DirectIOConnectionSpreadType::DirectIOConnectionSpreadType direct_io_spread_mode);
                    
                public:
                    ZMQDirectIOClient(string alias);
                    
                    ~ZMQDirectIOClient();
                    
                    //! Initialize instance
                    void init(void *init_data) throw(chaos::CException);
                    
                    //! Start the implementation
                    void start() throw(chaos::CException);
                    
                    //! Stop the implementation
                    void stop() throw(chaos::CException);
                    
                    //! Deinit the implementation
                    void deinit() throw(chaos::CException);
                    
                    // send the data to the server layer on priority channel
                    uint32_t sendPriorityData(void *data_buffer, uint32_t data_size);
                    
                    // send the data to the server layer on the service channel
                    uint32_t sendServiceData(void *data_buffer, uint32_t data_size);
                    
                    uint32_t receiveFromPriorityChannel(void **data_buffer, uint32_t *data_size);
                    
                    uint32_t receiveFromServiceChannel(void **data_buffer, uint32_t *data_size);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOZMQClient__) */
