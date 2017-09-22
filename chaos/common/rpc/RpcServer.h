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

#ifndef AbstractRpcAdapter_H
#define AbstractRpcAdapter_H

#include <boost/shared_ptr.hpp>
#include <string>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/rpc/RpcServerHandler.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/event/EventHandler.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/NamedService.h>

namespace chaos {
    using namespace std;
	
	//forward declaration
	namespace common {
		namespace network {
			class NetworkBroker;

		}
	}
    /*
     Abstract class for standard adapter method for permit, to CommandManager
     the correct initialization for the adapter instance
     */
    class RpcServer:
	public common::utility::StartableService,
	public common::utility::NamedService {
		friend class chaos::common::network::NetworkBroker;
    protected:
        //! port where server has been published
        int port_number;
        
        RpcServerHandler *command_handler;
        
        /*
         init the rpc adapter
         */
        virtual void init(void*) throw(CException) = 0;
        
        /*
         start the rpc adapter
         */
        virtual void start() throw(CException) = 0;
        
        /*
         deinit the rpc adapter
         */
        virtual void deinit() throw(CException) = 0;
        
    public:
        RpcServer(const std::string& alias);
        
        /*!
         Return the published port
         */
        virtual int getPublishedPort();
        
        /*
         set the command dispatcher associated to the instance of rpc adapter
         */
        virtual void setCommandDispatcher(RpcServerHandler *new_command_handler);
        
        
        //! return the numebr of message that are waiting to be sent
        /*!
         driver can overload this method to return(if has any) the size
         of internal queue message
         */
        virtual uint64_t getMessageQueueSize();
    };
}
#endif
