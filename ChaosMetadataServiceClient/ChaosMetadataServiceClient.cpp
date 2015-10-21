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
        
        api_proxy_manager.reset(new ApiProxyManager(NetworkBroker::getInstance(), &setting), "ApiProxyManager");
        api_proxy_manager.init(NULL, __PRETTY_FUNCTION__);
        
        monitor_manager.reset(new MonitorManager(NetworkBroker::getInstance(), &setting), "MonitorManager");
        monitor_manager.init(NULL, __PRETTY_FUNCTION__);
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
        ChaosCommon<ChaosMetadataServiceClient>::start();
        
        CMSC_LAPP << "-------------------------------------------------------------------------";
        CMSC_LAPP << "!CHAOS Metadata service client started";
        CMSC_LAPP << "RPC Server address: "	<< NetworkBroker::getInstance()->getRPCUrl();
        CMSC_LAPP << "DirectIO Server address: " << NetworkBroker::getInstance()->getDirectIOUrl();
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
        //stop monitor manager
        CHAOS_NOT_THROW(monitor_manager.stop(__PRETTY_FUNCTION__);)
        //stop batch system
        CHAOS_NOT_THROW( ChaosCommon<ChaosMetadataServiceClient>::stop();)
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
    CHAOS_NOT_THROW(monitor_manager.deinit(__PRETTY_FUNCTION__);)
    
    CHAOS_NOT_THROW(api_proxy_manager.deinit(__PRETTY_FUNCTION__);)
    
    if(monitoringIsStarted()){CHAOS_NOT_THROW(monitor_manager.deinit(__PRETTY_FUNCTION__);)}
    
    CHAOS_NOT_THROW(ChaosCommon<ChaosMetadataServiceClient>::deinit();)
    
    CMSC_LAPP << "-------------------------------------------------------------------------";
    CMSC_LAPP << "Metadata service client has been stopped";
    CMSC_LAPP << "-------------------------------------------------------------------------";
}

void ChaosMetadataServiceClient::clearServerList() {
    CHAOS_ASSERT(api_proxy_manager.get())
    api_proxy_manager->clearServer();
}

void ChaosMetadataServiceClient::addServerAddress(const std::string& server_address_and_port) {
    CHAOS_ASSERT(api_proxy_manager.get())
    api_proxy_manager->addServerAddress(server_address_and_port);
}

void ChaosMetadataServiceClient::enableMonitor() throw(CException) {
    //configure data driver
    reconfigureMonitor();
    
    //start the monitor manager
    monitor_manager.start(__PRETTY_FUNCTION__);
}

void ChaosMetadataServiceClient::disableMonitor() throw(CException) {
    CHAOS_NOT_THROW(monitor_manager.stop(__PRETTY_FUNCTION__);)
}

void ChaosMetadataServiceClient::reconfigureMonitor() throw(CException) {
    std::vector<std::string> endpoints_list;
    
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
    CHAOS_LASSERT_EXCEPTION((endpoint_array->size()!=0),
                            CMSC_LERR,
                            -3,
                            "No available server has been returned")
    
    for(int idx = 0;
        idx < endpoint_array->size();
        idx++) {
        std::string server = endpoint_array->getStringElementAtIndex(idx);
        CMSC_LDBG<< "Add " << server << " to server list";
        endpoints_list.push_back(server);
    }
    
    monitor_manager->resetEndpointList(endpoints_list);
}

bool ChaosMetadataServiceClient::monitoringIsStarted() {
    return monitor_manager.get() &&
    (monitor_manager->getServiceState() == CUStateKey::START);
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
    std::string healt_key = getHealtKeyFromGeneralKey(key_to_monitor);
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
    std::string healt_key = getHealtKeyFromGeneralKey(key_to_monitor);
    // call api for register the conusmer
    return addKeyAttributeHandler(healt_key,
                                  quantum_multiplier,
                                  attribute_handler,
                                  consumer_priority);
}

bool ChaosMetadataServiceClient::addKeyAttributeHandlerForDataset(const std::string& key_to_monitor,
                                                                  const unsigned int dataset_type,
                                                                  int quantum_multiplier,
                                                                  monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler,
                                                                  unsigned int consumer_priority) {
    std::string dataset_key;
    switch(dataset_type) {
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::OUTPUT_DATASE_PREFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::INPUT_DATASE_PREFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::CUSTOM_DATASE_PREFIX);
            break;
            
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::SYSTEM_DATASE_PREFIX);
            break;
        default:
            return false;
    }
    // call api for register the conusmer
    return addKeyAttributeHandler(dataset_key,
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
    std::string healt_key = boost::str(boost::format("%1%%2%")%
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
    std::string healt_key = boost::str(boost::format("%1%%2%")%
                                       key_to_monitor%
                                       NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
    return removeKeyAttributeHandler(healt_key,
                                     quantum_multiplier,
                                     attribute_handler);
}

bool ChaosMetadataServiceClient::removeKeyAttributeHandlerForDataset(const std::string& key_to_monitor,
                                                                     const unsigned int dataset_type,
                                                                     int quantum_multiplier,
                                                                     monitor_system::AbstractQuantumKeyAttributeHandler *attribute_handler) {
    std::string dataset_key = getDatasetKeyFromGeneralKey(key_to_monitor, dataset_type);
    if(dataset_key.compare("") == 0) return false;
    return removeKeyAttributeHandler(dataset_key,
                                     quantum_multiplier,
                                     attribute_handler);
}

std::string ChaosMetadataServiceClient::getDatasetKeyFromGeneralKey(const std::string& key,
                                                                    const unsigned int dataset_type) {
    switch(dataset_type) {
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            return boost::str(boost::format("%1%%2%")%
                              key%
                              DataPackPrefixID::OUTPUT_DATASE_PREFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            return boost::str(boost::format("%1%%2%")%
                              key%
                              DataPackPrefixID::INPUT_DATASE_PREFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            return boost::str(boost::format("%1%%2%")%
                              key%
                              DataPackPrefixID::CUSTOM_DATASE_PREFIX);
            break;
            
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            return boost::str(boost::format("%1%%2%")%
                              key%
                              DataPackPrefixID::SYSTEM_DATASE_PREFIX);
            break;
        default:
            return "";
    }
}

std::string ChaosMetadataServiceClient::getHealtKeyFromGeneralKey(const std::string& key) {
    return boost::str(boost::format("%1%%2%")%
                      key%
                      NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
}
