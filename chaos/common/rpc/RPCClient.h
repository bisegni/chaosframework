//
//  RPCClient.h
//  ChaosFramework
//
//  Created by Bisegni Claudio on 27/12/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_RPCClient_h
#define ChaosFramework_RPCClient_h

#include <boost/shared_ptr.hpp>
#include <string>

#include <chaos/common/global.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

namespace chaos {
    /*
     Abstract class for standard adapter method for permit, to CommandManager
     the correct initialization for the adapter instance
     */
    class RpcClient {
        string *typeName;
        string metadataServerAddress;
        bool canUseMetadataServer;
    protected:

    public:
        /*
         Constructor di default per i
         */
        RpcClient(string *alias){
            typeName = alias;
            canUseMetadataServer = GlobalConfiguration::getInstance()->isMEtadataServerConfigured();
            if(canUseMetadataServer){
                metadataServerAddress = GlobalConfiguration::getInstance()->getMetadataServerAddress();
            }
        };
        
        /*
         init the rpc adapter
         */
        virtual void init(CDataWrapper*) throw(CException) = 0;
        
        /*
         start the rpc adapter
         */
        virtual void start() throw(CException) = 0;
        
        /*
         deinit the rpc adapter
         */
        virtual void deinit() throw(CException) = 0;
        
        /*
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP, the second bool parameter specify, if this is 
         false, the message is sent in another queue thread 
         */
        virtual bool submitMessage(CDataWrapper*, bool) = 0;
        
        /*
         Submite a message specifieng the address
         */
        bool submitMessage(string& destinationIpAndPort, CDataWrapper *message, bool onThisThread=false) {
                //check in debug for pointer
            CHAOS_ASSERT(message)
                // add the address to the message
            message->addStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP, destinationIpAndPort);
                //submite the message
            return submitMessage(message,onThisThread);
        }
        
        /*
         Submite a message specifieng the address
         */
        bool submitMessageToMetadataServer(CDataWrapper *message, bool onThisThread=false) {
            if(!canUseMetadataServer) return false;
                //check in debug for pointer
            CHAOS_ASSERT(message)
                // add the address to the message
            message->addStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP, metadataServerAddress);
                //submite the message
            return submitMessage(message,onThisThread);
        }
        
        /*
         Return the adapter alias
         */
        const char * getName() const {
            return typeName->c_str();
        }
    };
}


#endif
