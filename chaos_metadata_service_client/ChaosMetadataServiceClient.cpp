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

#include <chaos/common/chaos_constants.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>
#include <chaos_metadata_service_client/metadata_service_client_constants.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <chaos/common/exception/CException.h>
#include <chaos/common/io/IODirectIODriver.h>

using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::event;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_monitor;
using namespace chaos::metadata_service_client::event;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::monitor_system;

using ChaosSharedPtr;

#define CMSC_LAPP INFO_LOG(ChaosMetadataServiceClient)
#define CMSC_LDBG DBG_LOG(ChaosMetadataServiceClient)
#define CMSC_LERR ERR_LOG(ChaosMetadataServiceClient)

#define POOL_SIZE_OPTION "live-channel-pool-size"

ChaosMetadataServiceClient::ChaosMetadataServiceClient(){
	 mds_client_initialized=mds_client_deinitialized=false;
	 io_pool_req=0;
	 GlobalConfiguration::getInstance()->addOption<int>(POOL_SIZE_OPTION,
	                                                         "number of independent live channels to fetch data",
															 DPCK_LAST_DATASET_INDEX);
}

ChaosMetadataServiceClient::~ChaosMetadataServiceClient() {}

void ChaosMetadataServiceClient::init(int argc, const char* argv[]) throw (CException) {
    ChaosCommon<ChaosMetadataServiceClient>::init(argc, argv);
}

void ChaosMetadataServiceClient::init()  throw(CException) {

	if(mds_client_initialized){
		CMSC_LDBG<<"Already initialized";
		return ;
	}
	CMSC_LDBG<<"Initializing";

	mds_client_initialized= true;
    mds_client_deinitialized = false;
    //--------------api proxy------------------------
      api_proxy_manager.reset(new ApiProxyManager(),
                              "ApiProxyManager");
      api_proxy_manager.init(NULL, __PRETTY_FUNCTION__);

      //--------------event dispatcher-----------------
      event_dispatch_manager.reset(new EventDispatchManager(&setting),
                                   "EventDispatchManager");
      event_dispatch_manager.init(NULL, __PRETTY_FUNCTION__);

      //--------------monitor manager------------------
      monitor_manager.reset(new MonitorManager(NetworkBroker::getInstance(),
                                               &setting),
                            "MonitorManager");
      monitor_manager.init(NULL, __PRETTY_FUNCTION__);

      //--------------node monitor---------------------
      node_monitor.reset(new node_monitor::NodeMonitor(monitor_manager.get(),
                                                       api_proxy_manager.get()),
                         "NodeMonitor");
      node_monitor.init(NULL, __PRETTY_FUNCTION__);

      //configure metadata server got from command line
      std::vector<chaos::common::network::CNetworkAddress> mds_address_list = GlobalConfiguration::getInstance()->getMetadataServerAddressList();
      for(std::vector<chaos::common::network::CNetworkAddress>::iterator it = mds_address_list.begin();
          it != mds_address_list.end();
          it++) {
          addServerAddress(it->ip_port);
      }
}

void ChaosMetadataServiceClient::init(void *init_data)  throw(CException) {
    try {
        ChaosCommon<ChaosMetadataServiceClient>::init(init_data);
        init();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        throw;
    }
}

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
        throw;
    }
}

void ChaosMetadataServiceClient::stop()   throw(CException) {
    try {
        //stop monitor manager
        if(monitoringIsStarted()) {
            CHAOS_NOT_THROW(monitor_manager.stop(__PRETTY_FUNCTION__);)
        }
        //stop batch system
        CHAOS_NOT_THROW( ChaosCommon<ChaosMetadataServiceClient>::stop();)
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        throw;
    }
}

void ChaosMetadataServiceClient::deinit()   throw(CException) {

	if(mds_client_deinitialized){
		LDBG_<<"Already deinitialized";
		return;
	}
	mds_client_deinitialized= true;
	mds_client_initialized=false;
    //deinit api system
    CHAOS_NOT_THROW(node_monitor.deinit(__PRETTY_FUNCTION__););
    
    CHAOS_NOT_THROW(monitor_manager.deinit(__PRETTY_FUNCTION__););
    
    CHAOS_NOT_THROW(event_dispatch_manager.deinit(__PRETTY_FUNCTION__););
    
    CHAOS_NOT_THROW(api_proxy_manager.deinit(__PRETTY_FUNCTION__););

    CHAOS_NOT_THROW(ChaosCommon<ChaosMetadataServiceClient>::deinit(););
    
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


chaos::common::io::IODataDriverShrdPtr ChaosMetadataServiceClient::getDataProxyChannelNewInstance() throw(CException){
	int poolsize=DPCK_LAST_DATASET_INDEX+1;
	chaos::common::io::IODataDriverShrdPtr shret;
	if(GlobalConfiguration::getInstance()->hasOption(POOL_SIZE_OPTION)){
		poolsize=GlobalConfiguration::getInstance()->getOption<int>(POOL_SIZE_OPTION);
	}

	if(iopool.size()<poolsize){
		chaos::common::io::IODataDriver *result = NULL;
		    const std::string impl_name =  CHAOS_FORMAT("%1%IODriver",%GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL));
		    result = ObjectFactoryRegister<chaos::common::io::IODataDriver>::getInstance()->getNewInstanceByName(impl_name);

		    if(result) {
		        if(impl_name.compare("IODirectIODriver") == 0) {
		            //set the information
		        	chaos::common::io::IODirectIODriverInitParam init_param;
		            std::memset(&init_param, 0, sizeof(chaos::common::io::IODirectIODriverInitParam));
		            init_param.client_instance = NULL;
		            init_param.endpoint_instance = NULL;
		            ((chaos::common::io::IODirectIODriver*)result)->setDirectIOParam(init_param);
		        }
		        result->init(NULL);
		        shret.reset(result);
		        iopool.push_back(shret);
		        io_pool_req++;
			    CMSC_LDBG<<"Allocating new iolive channel 0x"<<hex<<result<<dec<<", n:"<<iopool.size()<<" tot iorequest:"<<io_pool_req;
		        return shret;
		    }
		    if(iopool.size()){
		    	return iopool[0];
		    }
		    return shret;
	}
	shret=iopool[io_pool_req%iopool.size()];
    CMSC_LDBG<<"Retriving iolive channel 0x"<<hex<<shret.get()<<dec<<", id:"<<io_pool_req%iopool.size() <<" tot iorequest:"<<io_pool_req;
	io_pool_req++;
	return shret;


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
    CMultiTypeDataArrayWrapperSPtr endpoint_array = available_enpoint_result->getResult()->getVectorValue(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
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


bool ChaosMetadataServiceClient::addHandlerToNodeMonitor(const std::string& node_uid,
                                                         node_monitor::ControllerType controller_type,
                                                         node_monitor::NodeMonitorHandler *handler_to_add) {
    return node_monitor->addHandlerToNodeMonitor(node_uid,
                                                 controller_type,
                                                 handler_to_add);
}

bool ChaosMetadataServiceClient::removeHandlerToNodeMonitor(const std::string& node_uid,
                                                            node_monitor::ControllerType controller_type,
                                                            node_monitor::NodeMonitorHandler *handler_to_remove) {
    return node_monitor->removeHandlerToNodeMonitor(node_uid,
                                                    controller_type,
                                                    handler_to_remove);
    
}

//! add a new quantum slot for key
bool ChaosMetadataServiceClient::addKeyConsumer(const std::string& key_to_monitor,
                                                int quantum_multiplier,
                                                monitor_system::QuantumSlotConsumer *consumer,
                                                int consumer_priority) {
    CHAOS_ASSERT(monitor_manager.get());
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
    const std::string healt_key = getHealtKeyFromGeneralKey(key_to_monitor);
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
    CHAOS_ASSERT(monitor_manager.get());
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
    const std::string healt_key = getHealtKeyFromGeneralKey(key_to_monitor);
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
                                     DataPackPrefixID::OUTPUT_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::INPUT_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::CUSTOM_DATASET_POSTFIX);
            break;
            
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::SYSTEM_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
            dataset_key = boost::str(boost::format("%1%%2%")%
                                     key_to_monitor%
                                     DataPackPrefixID::CU_ALARM_DATASET_POSTFIX);
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

bool ChaosMetadataServiceClient::removeKeyConsumer(const std::string& key_to_monitor,
                                                   int quantum_multiplier,
                                                   monitor_system::QuantumSlotConsumer *consumer,
                                                   bool wait_completion) {
    CHAOS_ASSERT(monitor_manager.get());
    return monitor_manager->removeKeyConsumer(key_to_monitor,
                                              quantum_multiplier,
                                              consumer,
                                              wait_completion);
}

bool ChaosMetadataServiceClient::removeKeyConsumerForHealt(const std::string& key_to_monitor,
                                                           int quantum_multiplier,
                                                           monitor_system::QuantumSlotConsumer *consumer,
                                                           bool wait_completion) {
    // compose healt key for node
    std::string healt_key = boost::str(boost::format("%1%%2%")%
                                       key_to_monitor%
                                       NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
    return removeKeyConsumer(healt_key,
                             quantum_multiplier,
                             consumer,
                             wait_completion);
}

bool ChaosMetadataServiceClient::removeKeyAttributeHandler(const std::string& key_to_monitor,
                                                           int quantum_multiplier,
                                                           AbstractQuantumKeyAttributeHandler *attribute_handler) {
    if(monitor_manager.get() == NULL) return false;
    monitor_manager->removeKeyAttributeHandler(key_to_monitor,
                                               quantum_multiplier,
                                               attribute_handler);
    return true;
}


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

std::string ChaosMetadataServiceClient::getDatasetKeyFromGeneralKey(const std::string& node_uid,
                                                                    const unsigned int dataset_type) {
    switch(dataset_type) {
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            return boost::str(boost::format("%1%%2%")%
                              node_uid%
                              DataPackPrefixID::OUTPUT_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            return boost::str(boost::format("%1%%2%")%
                              node_uid%
                              DataPackPrefixID::INPUT_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            return boost::str(boost::format("%1%%2%")%
                              node_uid%
                              DataPackPrefixID::CUSTOM_DATASET_POSTFIX);
            break;
            
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            return boost::str(boost::format("%1%%2%")%
                              node_uid%
                              DataPackPrefixID::SYSTEM_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
            return boost::str(boost::format("%1%%2%")%
                              node_uid%
                              DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX);
            break;
        case chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
            return boost::str(boost::format("%1%%2%")%
                              node_uid%
                              DataPackPrefixID::CU_ALARM_DATASET_POSTFIX);
            break;
        default:
            return "";
    }
}

std::string ChaosMetadataServiceClient::getHealtKeyFromGeneralKey(const std::string& node_uid) {
    return boost::str(boost::format("%1%%2%")%
                      node_uid%
                      NodeHealtDefinitionKey::HEALT_KEY_POSTFIX);
}

void ChaosMetadataServiceClient::registerEventHandler(AbstractEventHandler *handler) {
    CHAOS_ASSERT(event_dispatch_manager.get());
    event_dispatch_manager->registerEventHandler(handler);
}

void ChaosMetadataServiceClient::deregisterEventHandler(AbstractEventHandler *handler) {
    event_dispatch_manager->deregisterEventHandler(handler);
}

void ChaosMetadataServiceClient::getNewCUController(const std::string& cu_id,
                                                    node_controller::CUController **cu_ctrl_handler) {
    *cu_ctrl_handler = new node_controller::CUController(cu_id,getDataProxyChannelNewInstance());
}

void ChaosMetadataServiceClient::deleteCUController(node_controller::CUController *cu_ctrl_ptr) {
    if(cu_ctrl_ptr) delete(cu_ctrl_ptr);
}
