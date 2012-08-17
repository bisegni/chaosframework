/*	
 *	AbstractRpcAdapter.h
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

#ifndef AbstractRpcAdapter_H
#define AbstractRpcAdapter_H

#include <boost/shared_ptr.hpp>
#include <string>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>
#include <chaos/common/exception/CException.h>

namespace chaos {
using namespace std;

    class MessageBroker;
    
    /*
     Abstract class for standard adapter method for permit, to CommandManager
     the correct initialization for the adapter instance
     */
    class RpcServer {
        friend class MessageBroker;
        string *typeName;
    protected:
        //! port where server has been published
        int portNumber;
        
        RpcServerHanlder *commandHandler;
        
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
        
    public:
        RpcServer(string *alias);
        
        /*!
         Return the published port
         */
        int getPublishedPort();
        
        /*
         set the command dispatcher associated to the instance of rpc adapter
         */
        void setCommandDispatcher(RpcServerHanlder *newCommandHandler);
        
        /*
         Return the adapter alias
         */
        const char * getName() const;
    };
}
#endif
