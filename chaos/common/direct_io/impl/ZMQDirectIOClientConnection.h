/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__ZMQDirectIOClientConnection__
#define __CHAOSFramework__ZMQDirectIOClientConnection__

#include <string>

#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/direct_io/impl/ZMQBaseClass.h>
#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zmq.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            namespace channel{
                class DirectIOVirtualClientChannel;
            }
            
            namespace impl {
                //forward declaration
                class ZMQDirectIOClient;
                
                typedef struct ConnectionMonitorInfo {
                    bool run;
                    std::string unique_identification;
                    ChaosUniquePtr<boost::thread> monitor_thread;
                    void *monitor_socket;
                    std::string monitor_url;
                } ConnectionMonitorInfo;
                
                /*!
                 Class that represetn th eimplementation of one connection of the direct io
                 connection implemented with zmq
                 */
                class ZMQDirectIOClientConnection :
                protected ZMQBaseClass,
                public chaos::common::direct_io::DirectIOClientConnection,
                public chaos::common::utility::InizializableService {
                    friend class ZMQDirectIOClient;
                    void *zmq_context;
                    MapZMQConfiguration default_configuration;
                    boost::shared_mutex mutext_send_message;
                    std::string priority_endpoint;
                    std::string service_endpoint;
                    std::string priority_identity;
                    std::string service_identity;
                    void *socket_priority;
                    void *socket_service;
                    uint16_t message_counter;
                    
                    ConnectionMonitorInfo monitor_info;
                    
                    boost::shared_mutex mutex_socket_manipolation;
                    
                    ZMQDirectIOClientConnection(void *zmq_context,
                                                const std::string& server_description,
                                                uint16_t endpoint);
                    ~ZMQDirectIOClientConnection();
                    
                    inline bool ensureSocket();
                    
                    inline int writeToSocket(void *socket,
                                             std::string& identity,
                                             chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                             chaos::common::direct_io::DirectIODataPackSPtr& asynchronous_answer);
                    inline int writeToSocket(void *socket,
                                             std::string& identity,
                                             chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                    int readMonitorMesg(void *monitor,
                                        int *value,
                                        char *address,
                                        int address_max_size);
                    void monitorWorker();
                    
                    void socketMonitor();
                    
                    //! get new connection
                    int getNewSocketPair();
                    
                    //! release an instantiated connection
                    int releaseSocketPair();
                protected:
                    
                    void init(void *init_data) ;
                    
                    void deinit() ;
                    
                    // send the data to the server layer on priority channel
                    int sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                    
                    int sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                         chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
                    
                    // send the data to the server layer on the service channel
                    int sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                    
                    int sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                        chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__ZMQDirectIOClientConnection__) */
