//
//  ZMQClient.h
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 11/03/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ZMQClient_h
#define CHAOSFramework_ZMQClient_h

//#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/pool/ResourcePool.h>
#include <chaos/common/chaos_types.h>

#if CHAOS_PROMETHEUS
#include <chaos/common/metric/metric.h>
#endif

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zmq.h>

#include <map>
#include <deque>
namespace chaos {
    
    class ZMQClient;
    class SocketEndpointPool;
    
    struct ZMQSocketPoolDef{
        void * socket;
        std::string identity;
    };
    
    typedef chaos::common::pool::ResourcePool<ZMQSocketPoolDef> ZMQSocketPool;
    
    //define the pool my for every endpoint
    CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr< ZMQSocketPool >, SocketMap)
    
    /*
     Class that implemnt !CHAOS RPC messaggin gusing ZMQ
     
     driver parameter:
     key:zmq_timeout value is a stirng that represent the integer used as timeout
     */
    DECLARE_CLASS_FACTORY(ZMQClient, RpcClient),
    public ZMQSocketPool::ResourcePoolHelper,
    public CObjectProcessingQueue<NetworkForwardInfo>,
    public chaos::common::async_central::TimerHandler {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQClient)
        ZMQClient(const string& alias);
        virtual ~ZMQClient();
        uint32_t zmq_timeout;
        boost::shared_mutex map_socket_mutex;
        SocketMap map_socket;
        ChaosAtomic<uint64_t> seq_id;
#if CHAOS_PROMETHEUS
        //custom driver metrics
        chaos::common::metric::GaugeUniquePtr counter_queuend_uptr;
        chaos::common::metric::GaugeUniquePtr counter_zmqerror_uptr;
#else
        ChaosUniquePtr<uint32_t> counter_zmqerror_uptr;
#endif
    protected:
        void *zmq_context;
        virtual void processBufferElement(NFISharedPtr element);
        inline ZMQSocketPool::ResourceSlot *getSocketForNFI(NetworkForwardInfo *nfi);
        inline void releaseSocket(ZMQSocketPool::ResourceSlot *socket_slot_to_release);
        inline void deleteSocket(ZMQSocketPool::ResourceSlot *socket_slot_to_release);
        
        //resource pool handler
        ZMQSocketPoolDef* allocateResource(const std::string& pool_identification,
                                           uint32_t& alive_for_ms);
        void deallocateResource(const std::string& pool_identification,
                                ZMQSocketPoolDef* resource_to_deallocate);
        
        //timer handler
        void timeout();
        
        int sendMessage(void *socket,
                        void *message_data,
                        size_t message_size,
                        bool more_to_send);
        
    public:
        
        /*
         init the rpc adapter
         */
        void init(void *init_data);
        
        /*
         start the rpc adapter
         */
        void start();
        
        /*
         start the rpc adapter
         */
        void stop();
        
        /*
         deinit the rpc adapter
         */
        void deinit();
        
        /*
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP
         */
        bool submitMessage(NFISharedPtr forwardInfo, bool onThisThread=false);
        
        //inherited method
        virtual uint64_t getMessageQueueSize();
    };
}
#endif
