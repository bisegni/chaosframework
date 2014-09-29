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

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <zmq.h>

#include <map>

namespace chaos {
    
	class SocketInfo {
	public:
		void *socket;
		boost::shared_mutex socket_mutex;
		
		SocketInfo():socket(NULL){};
	};
	
    /*
     Class that manage the MessagePack message send.
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(ZMQClient, RpcClient), public CObjectProcessingQueue<NetworkForwardInfo> {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(ZMQClient)
        ZMQClient(string alias);
        virtual ~ZMQClient();
		
		boost::shared_mutex map_socket_mutex;
		std::map<string, boost::shared_ptr<SocketInfo> > map_socket;
		
    protected:
        virtual void processBufferElement(NetworkForwardInfo*, ElementManagingPolicy&) throw(CException);
        void *zmqContext;
		boost::shared_ptr<SocketInfo> getSocketForNFI(NetworkForwardInfo *nfi);
		void disposeSocket(boost::shared_ptr<SocketInfo> socket_info_to_dispose);
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
