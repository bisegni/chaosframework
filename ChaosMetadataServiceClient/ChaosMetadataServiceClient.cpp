/*
 *	ChaosMetadataServiceClient.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#include "ChaosMetadataServiceClient.h"

#include <csignal>
#include <chaos/common/exception/CException.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>


#include <chaos/common/utility/ObjectFactoryRegister.h>

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex KVParamRegex("[a-zA-Z0-9/_-]+:[a-zA-Z0-9/_-]+");

using namespace std;
using namespace chaos;
using namespace chaos::metadata_service_client;
using namespace chaos::common::utility;
using boost::shared_ptr;

#define CMSC_LAPP INFO_LOG(ChaosMetadataServiceClient)
#define CMSC_LDBG DBG_LOG(ChaosMetadataServiceClient)
#define CMSC_LERR ERR_LOG(ChaosMetadataServiceClient)

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosMetadataServiceClient::init(int argc, char* argv[]) throw (CException) {
    ChaosCommon<ChaosMetadataServiceClient>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosMetadataServiceClient::init(istringstream &initStringStream) throw (CException) {
    ChaosCommon<ChaosMetadataServiceClient>::init(initStringStream);
}

/*
 *
 */
void ChaosMetadataServiceClient::init(void *init_data)  throw(CException) {
    try {
        ChaosCommon<ChaosMetadataServiceClient>::init(init_data);
        // network broker
        network_broker_service.reset(new NetworkBroker(), "NetworkBroker");
        network_broker_service.init(NULL, __PRETTY_FUNCTION__);
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        throw ex;
    }
}

/*
 *
 */
void ChaosMetadataServiceClient::start()  throw(CException) {
    //lock o monitor for waith the end
    try {
        //start network brocker
        network_broker_service.start(__PRETTY_FUNCTION__);
        CMSC_LAPP << "-------------------------------------------------------------------------";
        CMSC_LAPP << "!CHAOS Metadata service client started";
        CMSC_LAPP << "RPC Server address: "	<< network_broker_service->getRPCUrl();
        CMSC_LAPP << "DirectIO Server address: " << network_broker_service->getDirectIOUrl();
        CMSC_LAPP << "-------------------------------------------------------------------------";
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        throw ex;
    }
}

/*
 Stop the toolkit execution
 */
void ChaosMetadataServiceClient::stop()   throw(CException) {
    try {
        //stop batch system
        network_broker_service.stop(__PRETTY_FUNCTION__);
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        throw ex;
    }
}

/*
 Deiniti all the manager
 */
void ChaosMetadataServiceClient::deinit()   throw(CException) {
    
    //deinit api system
    try{
        network_broker_service.deinit(__PRETTY_FUNCTION__);
        CMSC_LAPP << "-------------------------------------------------------------------------";
        CMSC_LAPP << "Metadata service client has been stopped";
        CMSC_LAPP << "-------------------------------------------------------------------------";
    } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
            throw ex;
        }
    }