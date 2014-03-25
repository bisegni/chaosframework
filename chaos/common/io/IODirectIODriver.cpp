 /*
 *	IODirectIODriver.cpp
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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common//global.h>
#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex DataProxyServerDescriptionIPRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}:[0-9]{4,5}\\|[0-9]{1,3}");
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const boost::regex DataProxyServerDescriptionHostRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}:[0-9]{4,5}\\|[0-9]{1,3}");

#define LMEMDRIVER_ LAPP_ << "[Memcached IO Driver] - "

namespace chaos_data = chaos::common::data;
namespace chaos_direct_io = chaos::common::direct_io;
namespace chaos_dio_channel = chaos::common::direct_io::channel;

#define IODirectIODriver_LOG_HEAD "[IODirectIODriver] - "

#define IODirectIODriver_LAPP_ LAPP_ << IODirectIODriver_LOG_HEAD
#define IODirectIODriver_DLDBG_ LDBG_ << IODirectIODriver_LOG_HEAD
#define IODirectIODriver_LERR_ LERR_ << IODirectIODriver_LOG_HEAD

namespace chaos{
    using namespace std;
    using namespace boost;
    using namespace boost::algorithm;
	
	
    //using namespace memcache;
	
    /*
     * Driver constructor
     */
    IODirectIODriver::IODirectIODriver(std::string alias):NamedService(alias), current_endpoint_p_port(0), current_endpoint_s_port(0), current_endpoint_index(0) {
		//clear
		std::memset(&init_parameter, 0, sizeof(IODirectIODriverInitParam));
		
		device_server_channel = NULL;
		
		read_write_index = 0;
		data_cache.data_ptr = NULL;
		data_cache.data_len = 0;
    }
    
    /*
     * Driver distructor
     */
    IODirectIODriver::~IODirectIODriver() {
    }
    
	void IODirectIODriver::setDirectIOParam(IODirectIODriverInitParam& _init_parameter) {
		//store the configuration
		init_parameter = _init_parameter;
	}
	
    /*
     * Init method, the has map has all received value for configuration
     * every implemented driver need to get all needed configuration param
     */
    void IODirectIODriver::init(void *_init_parameter) throw(CException) {
        IODataDriver::init(_init_parameter);
		
		IODirectIODriver_LAPP_ << "Check init parameter";
		
		if(!init_parameter.network_broker) throw CException(-1, "No network broker configured", __PRETTY_FUNCTION__);
		
		init_parameter.client_instance = init_parameter.network_broker->getDirectIOClientInstance();
		if(!init_parameter.client_instance) throw CException(-1, "No client configured", __PRETTY_FUNCTION__);
		
		init_parameter.endpoint_instance = init_parameter.network_broker->getDirectIOServerEndpoint();
		if(!init_parameter.endpoint_instance) throw CException(-1, "No endpoint configured", __PRETTY_FUNCTION__);
		
		//initialize client
		utility::InizializableService::initImplementation(init_parameter.client_instance, _init_parameter, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);
		
		//get the client and server channel
		IODirectIODriver_LAPP_ << "Allcoate the default device server channel";
        device_server_channel = (chaos_dio_channel::DirectIODeviceServerChannel *)init_parameter.endpoint_instance->getNewChannelInstance("DirectIODeviceServerChannel");
		device_server_channel->setHandler(this);
		
		//store endpoint idnex for fast access
		current_endpoint_p_port = init_parameter.endpoint_instance->getPublicServerInterface()->getPriorityPort();
		current_endpoint_s_port = init_parameter.endpoint_instance->getPublicServerInterface()->getServicePort();
		current_endpoint_index = init_parameter.endpoint_instance->getRouteIndex();
		IODirectIODriver_LAPP_ << "Our receiving priority port is " << current_endpoint_p_port << " and enpoint is " <<current_endpoint_index;
		
    }
    
    /*
     * Deinitialization of memcached driver
     */
    void IODirectIODriver::deinit() throw(CException) {
		if(data_cache.data_ptr) {
			IODirectIODriver_LAPP_ << "delete last data received";
			free(data_cache.data_ptr);
		}
		
		//scan all slot and deinitialize all clients
		IODirectIODriver_LAPP_ << "Scan all slot and deinitialize all clients";
		for(int idx = 0; idx < channels_slot.getNumberOfSlot(); idx++) {
			IODirectIODriverClientChannels	*next_client = channels_slot.accessSlotByIndex(idx);
			next_client->connection->releaseChannelInstance(next_client->device_client_channel);
			init_parameter.client_instance->releaseConnection(next_client->connection);
		}
		channels_slot.clearSlots();
		
		//initialize client
		utility::InizializableService::deinitImplementation(init_parameter.client_instance, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);
		delete(init_parameter.client_instance);
		
		//deinitialize server channel
		if(device_server_channel) {
			init_parameter.endpoint_instance->releaseChannelInstance(device_server_channel);
		}
		
		if(init_parameter.endpoint_instance) {
			init_parameter.network_broker->releaseDirectIOServerEndpoint(init_parameter.endpoint_instance);
		}
		IODataDriver::deinit();
	}
    
	IODirectIODriverClientChannels *IODirectIODriver::getNextClientChannel() {
		uint32_t start_index;
		uint32_t cur_index;
		IODirectIODriverClientChannels	*found = NULL;
		IODirectIODriverClientChannels	*next_client = channels_slot.accessSlot(start_index);
		while(true) {
			if(next_client->connection->getState() == chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected) {
				found=next_client;
				break;
			}
			next_client = channels_slot.accessSlot(cur_index);
			if(start_index == cur_index) break;
		}
		return found;
	}
	
    /*
     * This method retrive the cached object by CSDawrapperUsed as query key and
     * return a pointer to the class ArrayPointer of CDataWrapper type
     */
    void IODirectIODriver::storeRawData(chaos_data::SerializationBuffer *serialization)  throw(CException) {
		CHAOS_ASSERT(serialization)
		IODirectIODriverClientChannels	*next_client = getNextClientChannel();
		serialization->disposeOnDelete = !next_client;
		if(next_client) {
			//free the packet
			next_client->device_client_channel->storeAndCacheDataOutputChannel((void*)serialization->getBufferPtr(), (uint32_t)serialization->getBufferLen());
			return;
		} else {
			DEBUG_CODE(IODirectIODriver_DLDBG_ << "No available socket->loose packet");
		}
		delete(serialization);
    }
    
    /*
     * This method retrive the cached object by CSDawrapperUsed as query key and
     * return a pointer to the class ArrayPointer of CDataWrapper type
     */
    char* IODirectIODriver::retriveRawData(size_t *dim)  throw(CException) {
		char* result = NULL;
		IODirectIODriverClientChannels	*next_client = getNextClientChannel();
		if(!next_client) return NULL;
		next_client->device_client_channel->requestLastOutputData();
		wait_get_answer.wait(1000);
		if(data_cache.data_ptr &&
		   data_cache.data_len) {
			if(dim) *dim = (size_t)data_cache.data_len;
			result = (char*)data_cache.data_ptr;
			std::memset(&data_cache, 0, sizeof(IODData));
		}
        return result;
    }
    
	//we have request data and this arrive with the put opcode
	void IODirectIODriver::consumePutEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header, void *channel_data, uint32_t channel_data_len) {
		delete(header);
		data_cache.data_len = channel_data_len;
		data_cache.data_ptr = channel_data;
		wait_get_answer.unlock();
	}
	
	void IODirectIODriver::consumeGetEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header, void *channel_data, uint32_t channel_data_len) {
		delete(header);
		if(channel_data)free(channel_data);
	}
	
    /*
     Update the driver configuration
     */
    chaos_data::CDataWrapper* IODirectIODriver::updateConfiguration(chaos_data::CDataWrapper* newConfigration) {
		//checkif someone has passed us the device indetification
        if(newConfigration->hasKey(DatasetDefinitionkey::DEVICE_ID)){
            dataKey = newConfigration->getStringValue(DatasetDefinitionkey::DEVICE_ID);
            IODirectIODriver_LAPP_ << "The key for memory cache is: " << dataKey;
        }
        if(newConfigration->hasKey(DataProxyConfigurationKey::CS_DM_LD_SERVER_ADDRESS)){
            IODirectIODriver_LAPP_ << "Get the DataManager LiveData address value";
            auto_ptr<chaos_data::CMultiTypeDataArrayWrapper> liveMemAddrConfig(newConfigration->getVectorValue(DataProxyConfigurationKey::CS_DM_LD_SERVER_ADDRESS));
			size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
            for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){
				string serverDesc = liveMemAddrConfig->getStringElementAtIndex(idx);
				if(!boost::regex_match(serverDesc, DataProxyServerDescriptionIPRegExp) &&
				   !boost::regex_match(serverDesc, DataProxyServerDescriptionHostRegExp)) {
					IODirectIODriver_DLDBG_ << "Data proxy server description " << serverDesc << " non well formed";
					continue;
				}
				IODirectIODriver_DLDBG_ << "Try to install data proxy description "<< serverDesc;
				std::vector<string> tokens;
				boost::algorithm::split(tokens, serverDesc, boost::algorithm::is_any_of("|"), boost::algorithm::token_compress_on);
				if(registered_server.find(tokens[0]) ==  registered_server.end()) {
					if(tokens.size()==2) {
						IODirectIODriver_DLDBG_ << "Data proxy server description " << tokens[0];
						IODirectIODriver_DLDBG_ << "Data proxy server Endpoint " << tokens[1];
						addNewServerConnection(serverDesc);
						registered_server.insert(serverDesc);
					} else {
						IODirectIODriver_DLDBG_ << "Bad server configuration";
					}
				} else {
					IODirectIODriver_DLDBG_ << "Server already configured";
				}
            }
		}
        return NULL;
    }
	
	void IODirectIODriver::addNewServerConnection(std::string server_description) {
		IODirectIODriver_DLDBG_ << "Add connection for " << server_description;
		chaos_direct_io::DirectIOClientConnection *tmp_connection = init_parameter.client_instance->getNewConnection(server_description);
		if(tmp_connection) {
			IODirectIODriverClientChannels * clients_channel = new IODirectIODriverClientChannels();
			clients_channel->connection = tmp_connection;
			clients_channel->device_client_channel = (chaos_dio_channel::DirectIODeviceClientChannel *)tmp_connection->getNewChannelInstance("DirectIODeviceClientChannel");
			if(!clients_channel->device_client_channel) {
				IODirectIODriver_DLDBG_ << "Error creating client device channel for " << server_description;
				delete(clients_channel);
			}
            //set device id
			clients_channel->device_client_channel->setDeviceID(dataKey);
            
            //set the answer information
			clients_channel->device_client_channel->setAnswerServerInfo(current_endpoint_p_port, current_endpoint_s_port, current_endpoint_index);
			channels_slot.addSlot(clients_channel);
		} else {
			IODirectIODriver_DLDBG_ << "Error creating client connection for " << server_description;
		}
	}
}