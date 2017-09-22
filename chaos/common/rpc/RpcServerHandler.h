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

#ifndef CHAOSFramework_RpcServerHanlder_h
#define CHAOSFramework_RpcServerHanlder_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>

namespace chaos_data = chaos::common::data;

namespace chaos {
        //!Handle the rpc pack receivement event
    /*!
     Abstract handler class for standard comunication between rpc client and server and other classes
     */
    class RpcServerHandler {
    public:
        //! method called when the rpc server receive a new data
        /*!
         This method will be called by the implementation of RPCServer when a new data apck
         will be received
         @param newPack the new CDatawrapper instance pointer received by rpc server
         @return the response if the action has been submitted
         */
            //virtual CDataWrapper * newRpcPackHasArrived(CDataWrapper& newPack) throw(CException) = 0;
        virtual chaos_data::CDataWrapper* dispatchCommand(chaos_data::CDataWrapper * action_pack)  throw(CException) = 0;
		
		//! execute an action in synchronous mode
		/*!
		 This method will be called by the implementation of RPCServer when a new data apck
		 will be received
		 @param action_pack the CDatawrapper instance pointer received by rpc server taht contain infromatio to execute an action
		 @return the response of the called action
		 */
		virtual chaos_data::CDataWrapper* executeCommandSync(chaos_data::CDataWrapper * action_pack) = 0;
        
        //! execute an action in synchronous mode
        /*!
         This method will be called by the implementation of RPCServer when a new data apck
         will be received
         @param domain the domain where search the action
         @param action is the alias of the rpc action to call
         @param action_pack the CDatawrapper instance pointer received by rpc server taht contain infromatio to execute an action
         @return the response of the called action
         */
//        virtual chaos_data::CDataWrapper* executeCommandSync(const std::string& domain,
//                                                             const std::string& action,
//                                                             chaos_data::CDataWrapper * message_data) = 0;
    };
}
#endif
