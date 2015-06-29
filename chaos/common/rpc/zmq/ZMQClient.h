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

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zmq.h>

#include <map>
#include <deque>
namespace chaos {
    
    class ZMQClient;
    class SocketEndpointPool;
    
    class SocketInfo {
        friend class ZMQClient;
        friend class SocketEndpointPool;
        std::string endpoint;
        uint64_t last_push_ts;
        
        SocketInfo();
    public:     
        ~SocketInfo();
   
        void *socket;
    };
    
    typedef std::deque< SocketInfo* >                       SocketPoolQueue;
    typedef std::deque< SocketInfo* > ::reverse_iterator    SocketPoolQueueReverseIterator;

    
    class SocketEndpointPool {
        friend class ZMQClient;
        unsigned int created_socket;
        void *zmq_context;
        const std::string endpoint;
        SocketPoolQueue pool;
        boost::mutex mutex_pool;
        
        SocketEndpointPool(const std::string& _endpoint,
                           void *_zmq_context);
    public:
        ~SocketEndpointPool();
        SocketInfo *getSocket();
        unsigned int getSize();
        void releaseSocket(SocketInfo *socket_info);
        void mantainance();
    };
    
    typedef std::map<std::string, boost::shared_ptr<SocketEndpointPool> >           SocketMap;
    typedef std::map<std::string, boost::shared_ptr<SocketEndpointPool> >::iterator SocketMapIterator;
    
    /*
     Class that manage the MessagePack message send.
     */
    DECLARE_CLASS_FACTORY(ZMQClient, RpcClient),
    public CObjectProcessingQueue<NetworkForwardInfo>,
    public chaos::common::async_central::TimerHandler {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQClient)
        ZMQClient(const string& alias);
        virtual ~ZMQClient();
        
        boost::shared_mutex map_socket_mutex;
        SocketMap map_socket;
    protected:
        virtual void processBufferElement(NetworkForwardInfo*, ElementManagingPolicy&) throw(CException);
        void *zmqContext;
        inline SocketInfo *getSocketForNFI(NetworkForwardInfo *nfi);
        inline void releaseSocket(SocketInfo *socket_info_to_dispose);
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
    };
}
#endif
