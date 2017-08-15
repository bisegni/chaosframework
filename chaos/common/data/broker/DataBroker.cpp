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
#include <chaos/common/data/broker/DataBroker.h>

namespace chaos_data = chaos::common::data;
namespace chaos_databroker = chaos::common::data::broker;



    //! Initialize instance
void chaos_databroker::DataBroker::init(void*) throw(chaos::CException) {
    
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
void chaos_databroker::DataBroker::start() throw(chaos::CException) {
    
}

    //! Start the implementation
void chaos_databroker::DataBroker::stop() throw(chaos::CException) {

}

    //! Deinit the implementation
void chaos_databroker::DataBroker::deinit() throw(chaos::CException) {
    
}

chaos_data::CDataWrapper *chaos_databroker::DataBroker::startHandshake(chaos_data::CDataWrapper *handshakeData, bool& detach) {
    return NULL;
}