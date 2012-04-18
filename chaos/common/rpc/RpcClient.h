/*	
 *	RpcClient.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#ifndef ChaosFramework_RPCClient_h
#define ChaosFramework_RPCClient_h

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <chaos/common/global.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

namespace chaos {
    /*!
     Define the information for send a message to some server
     */
    typedef struct {
        //! listO of the server where to send data
        vector<string> remoteServers;
        //message to forward to remote server
        CDataWrapper *message;
    } MessageForwardingInfo;
    
    /*!
     Abstract class for standard adapter method for permit, to CommandManager
     the correct initialization for the adapter instance
     */
    class RpcClient {
        string *typeName;
    protected:

    public:
        /*!
         Constructor di default per i
         */
        RpcClient(string *alias){
            typeName = alias;
        };
        
        /*!
         init the rpc adapter
         */
        virtual void init(CDataWrapper*) throw(CException) = 0;
        
        /*!
         start the rpc adapter
         */
        virtual void start() throw(CException) = 0;
        
        /*!
         deinit the rpc adapter
         */
        virtual void deinit() throw(CException) = 0;
        
        /*!
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP, the second bool parameter specify, if this is 
         false, the message is sent in another queue thread 
         */
        virtual bool submitMessage(CDataWrapper*, bool)  throw(CException) = 0;
        
        /*!
         Submite a message specifieng the address
         */
        bool submitMessage(string& destinationIpAndPort, CDataWrapper *message, bool onThisThread=false) throw(CException) {
                //check in debug for pointer
            CHAOS_ASSERT(message)
                // add the address to the message
            message->addStringValue(RpcActionDefinitionKey::CS_CMDM_REMOTE_HOST_IP, destinationIpAndPort);
                //submite the message
            return submitMessage(message,onThisThread);
        }
        
        /*!
         Return the adapter alias
         */
        const char * getName() const {
            return typeName->c_str();
        }
    };
}
#endif
