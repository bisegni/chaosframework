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
        
        SocketInfo():
        socket(NULL),
        endpoint(""),
        last_push_ts(0){};
    public:     
        ~SocketInfo(){
            if(socket)zmq_close(socket);
        }
   
        void *socket;
    };
    
    class SocketEndpointPool {
        friend class ZMQClient;
        const std::string endpoint;
        void *zmq_context;
        std::deque< SocketInfo* > pool;
        boost::mutex mutex_pool;
        
        SocketEndpointPool(const std::string& _endpoint):
        endpoint(_endpoint) {
        }
        
        public:
        ~SocketEndpointPool() {
            for(std::deque< SocketInfo* >::iterator it = pool.begin();
                it!=pool.end();
                it++) {
                delete(*it);
            }
        }
    
        SocketInfo *getSocket() {
            int	err = 0;
            int linger = 0;
            int water_mark = 1;
            int timeout = 5000;
            boost::unique_lock<boost::mutex> l(mutex_pool);
            SocketInfo *socket_info_ptr = NULL;
            if(pool.empty()) {
                std::auto_ptr<SocketInfo> _temp_socket_info_ptr;
                _temp_socket_info_ptr.reset(new SocketInfo());
                _temp_socket_info_ptr->endpoint = endpoint;
                _temp_socket_info_ptr->socket = zmq_socket (zmq_context, ZMQ_REQ);
                if(!_temp_socket_info_ptr->socket) {
                    err = -1;
                } else if ((err = zmq_setsockopt(_temp_socket_info_ptr->socket, ZMQ_LINGER, &linger, sizeof(int)))) {
                } else if ((err = zmq_setsockopt(_temp_socket_info_ptr->socket, ZMQ_RCVHWM, &water_mark, sizeof(int)))) {
                } else if ((err = zmq_setsockopt(_temp_socket_info_ptr->socket, ZMQ_SNDHWM, &water_mark, sizeof(int)))) {
                } else if ((err = zmq_setsockopt(_temp_socket_info_ptr->socket, ZMQ_SNDTIMEO, &timeout, sizeof(int)))) {
                } else if ((err = zmq_setsockopt(_temp_socket_info_ptr->socket, ZMQ_RCVTIMEO, &timeout, sizeof(int)))) {
                } else {
                    string url = "tcp://";
                    url.append(endpoint);
                    if((err = zmq_connect(_temp_socket_info_ptr->socket, url.c_str()))) {
                    }
                }
                
                if(err) {
                    if(_temp_socket_info_ptr->socket) {
                        zmq_close(_temp_socket_info_ptr->socket);
                    }
                } else {
                    //all is gone weel so we can release the temp smatr pointer to result pointer
                    socket_info_ptr = _temp_socket_info_ptr.release();
                    socket_info_ptr->last_push_ts = chaos::common::utility::TimingUtil::getTimeStamp();
                }
            } else {
                //return alread allcoated one
                socket_info_ptr = pool.front();
                socket_info_ptr->last_push_ts = chaos::common::utility::TimingUtil::getTimeStamp();
                //remove associated pointr
                pool.pop_front();
            }
            return socket_info_ptr;
        }
        
        void releaseSocket(SocketInfo *socket_info) {
            boost::unique_lock<boost::mutex> l(mutex_pool);
            pool.push_front(socket_info);
        }
        
        //! remove timeouted element
        void mantainance() {
            boost::unique_lock<boost::mutex> l(mutex_pool);
            //check if last element is in timeout
            SocketInfo *checked_socket = pool.back();
            if((chaos::common::utility::TimingUtil::getTimeStamp() - checked_socket->last_push_ts) >
               60000) {
                //we can remove and delete it
                delete(checked_socket);
            }
        }
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
