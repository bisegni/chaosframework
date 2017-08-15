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
#ifndef CHAOSFramework_EventForwarder_h
#define CHAOSFramework_EventForwarder_h

#include <chaos/common/exception/exception.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    namespace common {
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
                 @return a true value if the submitions to output queue has been done
                 */
                //virtual CDataWrapper * newRpcPackHasArrived(CDataWrapper& newPack) throw(CException) = 0;
                virtual bool submitEvent(EventDescriptor *event)  throw(CException) = 0;
            };
        }
    }
}
#endif
