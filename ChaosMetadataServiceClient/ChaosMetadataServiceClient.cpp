/*
 *	ChaosMetadataServiceClient.cpp
 *	!CHAOS
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
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>
#include <ChaosMetadataServiceClient/metadata_service_client_constants.h>


#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <chaos/common/exception/CException.h>

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex KVParamRegex("[a-zA-Z0-9/_-]+:[a-zA-Z0-9/_-]+");

using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::metadata_service_client;
using namespace chaos::DataServiceNodeDefinitionKey;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::monitor_system;

using boost::shared_ptr;

#define CMSC_LAPP INFO_LOG(ChaosMetadataServiceClient)
#define CMSC_LDBG DBG_LOG(ChaosMetadataServiceClient)
#define CMSC_LERR ERR_LOG(ChaosMetadataServiceClient)

//!default constructor
ChaosMetadataServiceClient::ChaosMetadataServiceClient() {
    setClientInitParameter();
}

//! default destructor
ChaosMetadataServiceClient::~ChaosMetadataServiceClient() {
    
}

//! set the custom client init parameter
void ChaosMetadataServiceClient::setClientInitParameter() {
    getGlobalConfigurationInstance()->addOption< std::vector<std::string> >(OPT_MDS_ADDRESS,
                                                                            OPT_MDS_ADDRESS_DESCRIPTION,
                                                                            &setting.mds_backend_servers);
}

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosMetadataServiceClient::init(int argc, char* argv[]) throw (CException) {
    ChaosCommon<ChaosMetadataServiceClient>::init(argc, argv);
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
        
        api_proxy_manager.reset(new ApiProxyManager(network_broker_service.get(), &setting), "ApiProxyManager");
        api_proxy_manager.init(NULL, __PRETTY_FUNCTION__);
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        throw ex;
    }
}

/*
 *
 */
void ChaosMetadataServiceClient::start()  throw(CException) {
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
        //stop monitor manager
        if(monitor_manager.get()) monitor_manager.stop(__PRETTY_FUNCTION__);
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
        CHAOS_NOT_THROW(api_proxy_manager.deinit(__PRETTY_FUNCTION__);)
        
        CHAOS_NOT_THROW(network_broker_service.deinit(__PRETTY_FUNCTION__);)
        
        if(monitor_manager.get())CHAOS_NOT_THROW(monitor_manager.deinit(__PRETTY_FUNCTION__);)
            CMSC_LAPP << "-------------------------------------------------------------------------";
        CMSC_LAPP << "Metadata service client has been stopped";
        CMSC_LAPP << "-------------------------------------------------------------------------";
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        throw ex;
    }
}

void ChaosMetadataServiceClient::clearServerList() {
    CHAOS_ASSERT(api_proxy_manager.get())
    api_proxy_manager->clearServer();
}

void ChaosMetadataServiceClient::addServerAddress(const std::string& server_address_and_port) {
    CHAOS_ASSERT(api_proxy_manager.get())
    api_proxy_manager->addServerAddress(server_address_and_port);
}

void ChaosMetadataServiceClient::enableMonitoring() throw(CException) {
    std::vector<std::string> endpoints_list;
    
    monitor_manager.reset(new MonitorManager(network_broker_service.get(), &setting), "MonitorManager");
    monitor_manager.init(NULL, __PRETTY_FUNCTION__);
    
    CMSC_LDBG << "Ask to metadata server for cds enpoint for monitoring";
    
    //get server available for dataservice from metadata server with two second of timeout
    ApiProxyResult available_enpoint_result = getApiProxy<data_service::GetBestEndpoints>(2000)->execute();
    CHAOS_LASSERT_EXCEPTION(available_enpoint_result->wait(),
                            CMSC_LERR,
                            -1,
                            "No answer for get data service endpoint")
    
    //we have the result
    CHAOS_LASSERT_EXCEPTION((available_enpoint_result->getError()==0),
                            CMSC_LERR,
                            available_enpoint_result->getError(),
                            boost::str(boost::format("%1%\n%2%")%
                                       available_enpoint_result->getErrorMessage()%
                                       available_enpoint_result->getErrorDomain()))
    //we have result with no error
    CHAOS_LASSERT_EXCEPTION(available_enpoint_result->getResult()->hasKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST),
                            CMSC_LERR,
                            -2,
                            "No available server has been found")
    
    //get the endpoint array
    CMSC_LDBG<< "Scan the result for serverlist";
    std::auto_ptr<CMultiTypeDataArrayWrapper> endpoint_array(available_enpoint_result->getResult()->getVectorValue(DS_DIRECT_IO_FULL_ADDRESS_LIST));
    for(int idx = 0;
        idx < endpoint_array->size();
        idx++) {
        std::string server = endpoint_array->getStringElementAtIndex(idx);
        CMSC_LDBG<< "Add " << server << " to server list";
        endpoints_list.push_back(server);
    }
    monitor_manager->resetEndpointList(endpoints_list);
    //start the monitor manager
    monitor_manager.start(__PRETTY_FUNCTION__);
}

void ChaosMetadataServiceClient::disableMonitoring() throw(CException) {
    if(monitor_manager.get()) {
        CHAOS_NOT_THROW(monitor_manager.stop(__PRETTY_FUNCTION__);)
        CHAOS_NOT_THROW(monitor_manager.deinit(__PRETTY_FUNCTION__);)
    }
}

//! add a new quantum slot for key
bool ChaosMetadataServiceClient::addKeyConsumer(const std::string& key_to_monitor,
                                                int quantum_multiplier,
                                                monitor_system::QuantumSlotConsumer *consumer,
                                                int consumer_priority) {
    if(monitor_manager.get() == NULL) return false;
    monitor_manager->addKeyConsumer(key_to_monitor,
                                    quantum_multiplier,
                                    consumer,
                                    consumer_priority);
    return true;
}

//! add a new consumer ofr the healt data associated to a key
bool ChaosMetadataServiceClient::addKeyConsumerForHealt(const std::string& key_to_monitor,
                                                        int quantum_multiplier,
                                                        monitor_system::QuantumSlotConsumer *consumer,
                                                        int consumer_priority) {
    // compose healt key for node
    std::string healt_key = boost::str(boost::format("%1%_%2%")%
                                       key_to_monitor%
                                       NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
    // call api for register the conusmer
    return addKeyConsumer(healt_key,
                          quantum_multiplier,
                          consumer,
                          consumer_priority);
}

bool ChaosMetadataServiceClient::addKeyAttributeHandler(const std::string& key_to_monitor,
                                                        int quantum_multiplier,
                                                        AbstractQuantumKeyAttributeHandler *attribute_handler,
                                                        unsigned int consumer_priority) {
    if(monitor_manager.get() == NULL) return false;
    monitor_manager->addKeyAttributeHandler(key_to_monitor,
                                            quantum_multiplier,
                                            attribute_handler,
                                            consumer_priority);
    return true;
}

//! add a new handler for a single attribute for a healt data for a key
bool ChaosMetadataServiceClient::addKeyAttributeHandlerForHealt(const std::string& key_to_monitor,
                                                                int quantum_multiplier,
                                                                monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler,
                                                                unsigned int consumer_priority) {
    // compose healt key for node
    std::string healt_key = boost::str(boost::format("%1%_%2%")%
                                       key_to_monitor%
                                       NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
    // call api for register the conusmer
    return addKeyAttributeHandler(healt_key,
                                  quantum_multiplier,
                                  attribute_handler,
                                  consumer_priority);
}

//! remove a consumer by key and quantum
bool ChaosMetadataServiceClient::removeKeyConsumer(const std::string& key_to_monitor,
                                                   int quantum_multiplier,
                                                   monitor_system::QuantumSlotConsumer *consumer) {
    if(monitor_manager.get() == NULL) return false;
    monitor_manager->addKeyConsumer(key_to_monitor,
                                    quantum_multiplier,
                                    consumer);
    return true;
}

//! remove a consumer for the healt data associated to a key
bool ChaosMetadataServiceClient::removeKeyConsumerForHealt(const std::string& key_to_monitor,
                                                           int quantum_multiplier,
                                                           monitor_system::QuantumSlotConsumer *consumer) {
    // compose healt key for node
    std::string healt_key = boost::str(boost::format("%1%_%2%")%
                                       key_to_monitor%
                                       NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
    return removeKeyConsumer(healt_key,
                             quantum_multiplier,
                             consumer);
}

//! remove an handler associated to ans attirbute of a key
bool ChaosMetadataServiceClient::removeKeyAttributeHandler(const std::string& key_to_monitor,
                                                           int quantum_multiplier,
                                                           AbstractQuantumKeyAttributeHandler *attribute_handler) {
    if(monitor_manager.get() == NULL) return false;
    monitor_manager->removeKeyAttributeHandler(key_to_monitor,
                                               quantum_multiplier,
                                               attribute_handler);
    return true;
}

//! remove an handler associated to ans attirbute of a key
bool ChaosMetadataServiceClient::removeKeyAttributeHandlerForHealt(const std::string& key_to_monitor,
                                                                   int quantum_multiplier,
                                                                   monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler) {
    // compose healt key for node
    std::string healt_key = boost::str(boost::format("%1%_%2%")%
                                       key_to_monitor%
                                       NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
    return removeKeyAttributeHandler(healt_key,
                                     quantum_multiplier,
                                     attribute_handler);
}