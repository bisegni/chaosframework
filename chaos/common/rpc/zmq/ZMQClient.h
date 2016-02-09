//
//  ZMQClient.h
//  CHAOSFramework
//
//  Created by Bisegni Claudio on 11/03/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ZMQClient_h
#define CHAOSFramework_ZMQClient_h

#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/pool/ResourcePool.h>
#include <chaos/common/chaos_types.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zmq.h>

#include <map>
#include <deque>
namespace chaos {
    
    class ZMQClient;
    class SocketEndpointPool;

    //define the pool my for every endpoint
    CHAOS_DEFINE_MAP_FOR_TYPE(std::string, boost::shared_ptr< chaos::common::pool::ResourcePool<void*> >, SocketMap)
    
    /*
     Class that implemnt !CHAOS RPC messaggin gusing ZMQ
     
     driver parameter:
        key:zmq_timeout value is a stirng that represent the integer used as timeout
     */
    DECLARE_CLASS_FACTORY(ZMQClient, RpcClient),
    public chaos::common::pool::ResourcePool<void*>::ResourcePoolHelper,
    public CObjectProcessingQueue<NetworkForwardInfo>,
    public chaos::common::async_central::TimerHandler {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQClient)
        ZMQClient(const string& alias);
        virtual ~ZMQClient();
        uint32_t zmq_timeout;
        boost::shared_mutex map_socket_mutex;
        SocketMap map_socket;
    protected:
        void *zmq_context;
        virtual void processBufferElement(NetworkForwardInfo*, ElementManagingPolicy&) throw(CException);
        inline chaos::common::pool::ResourcePool<void*>::ResourceSlot *getSocketForNFI(NetworkForwardInfo *nfi);
        inline void releaseSocket(chaos::common::pool::ResourcePool<void*>::ResourceSlot *socket_slot_to_release);
        inline void deleteSocket(chaos::common::pool::ResourcePool<void*>::ResourceSlot *socket_slot_to_release);
        
        //resource pool handler
        void* allocateResource(const std::string& pool_identification, uint32_t& alive_for_ms);
        void deallocateResource(const std::string& pool_identification, void* resource_to_deallocate);
        
        //timer handler
        void timeout();
    public:
        
        /*
         init the rpc adapter
         */
        void init(void *init_data) throw(CException);
        
        /*
         start the rpc adapter
         */
        void start() throw(CException);
        
        /*
         start the rpc adapter
         */
        void stop() throw(CException);
        
        /*
         deinit the rpc adapter
         */
        void deinit() throw(CException);
        
        /*
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP
         */
        bool submitMessage(NetworkForwardInfo *forwardInfo, bool onThisThread=false) throw(CException);
        
        //inherited method
        virtual uint64_t getMessageQueueSize();
    };
}
#endif
