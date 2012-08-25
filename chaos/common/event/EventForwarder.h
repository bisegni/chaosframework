/*
 *	EventForwarding.h
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
#ifndef CHAOSFramework_EventServerHandler_h
#define CHAOSFramework_EventServerHandler_h

#include <chaos/common/exception/CException.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    namespace event{
            //!Handle the rpc pack forwarding event
        /*!
         Abstract handler class for standard comunication between rpc client and server and other classes
         */
        class EventForwarder {
        public:
                //! method called when the rpc server receive a new data
            /*!
             This method will be called by the implementation of RPCServer when a new data apck
             will be received
             @param newPack the new CDatawrapper instance pointer received by rpc server
             @return the response to the pack, if there is one
             */
                //virtual CDataWrapper * newRpcPackHasArrived(CDataWrapper& newPack) throw(CException) = 0;
            virtual void submitEvent(EventDescriptor *event)  throw(CException) = 0;
        };
    }
}


#endif
