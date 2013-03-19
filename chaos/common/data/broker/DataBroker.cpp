/*
 *	DataBroker.cpp
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
#include "DataBroker.h"

using namespace chaos;
using namespace chaos::databroker;



    //! Initialize instance
void DataBroker::init(void*) throw(chaos::CException) {
    
        //define the action for start the hadnshake
    AbstActionDescShrPtr actionDescription = DeclareAction::addActionDescritionInstance(this,
                                                                                        &DataBroker::startHandshake,
                                                                                        "DataBroker",
                                                                                        "startHandshake",
                                                                                        "Start the handshake process for register a remote listener to this data broker instance");
    
    actionDescription->addParam(REMOTE_NODE_ID, DataType::TYPE_STRING, "The requester chaos node id");
    actionDescription->addParam(REMOTE_NODE_ADDR, DataType::TYPE_STRING, "The requester chaos node address");
    actionDescription->addParam(REQUESTED_MODE, DataType::TYPE_INT32, "The request mode to use");
}

    //! Start the implementation
void DataBroker::start() throw(chaos::CException) {
    
}

    //! Start the implementation
void DataBroker::stop() throw(chaos::CException) {

}

    //! Deinit the implementation
void DataBroker::deinit() throw(chaos::CException) {
    
}

CDataWrapper *DataBroker::startHandshake(CDataWrapper *handshakeData, bool& detach) {
    return NULL;
}