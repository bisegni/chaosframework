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

#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common//global.h>
#include <chaos/common/utility/InizializableService.h>

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
    IODirectIODriver::IODirectIODriver(std::string alias):NamedService(alias), current_endpoint_port(0), current_endpoint_index(0) {
		//clear
		std::memset(&init_parameter, 0, sizeof(IODirectIODriverInitParam));
		
		read_write_index = 0;
		std::memset(&data_cache, 0, sizeof(IODData));
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

		if(!init_parameter.client_instance) throw CException(-1, "No client configured", __PRETTY_FUNCTION__);
		if(!init_parameter.endpoint_instance) throw CException(-1, "No endpoint configured", __PRETTY_FUNCTION__);
		
		//initialize client
		utility::InizializableService::initImplementation(init_parameter.client_instance, _init_parameter, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);
		
		//get the client and server channel
		device_client_channel = (chaos_dio_channel::DirectIODeviceClientChannel *)init_parameter.client_instance->getNewChannelInstance("DirectIODeviceClientChannel");
		device_server_channel = (chaos_dio_channel::DirectIODeviceServerChannel *)init_parameter.endpoint_instance->getNewChannelInstance("DirectIODeviceServerChannel");
		
		//store endpoint idnex for fast access
		current_endpoint_port = init_parameter.endpoint_instance->getPublicServerInterface()->getPriorityPort();
		current_endpoint_index = init_parameter.endpoint_instance->getRouteIndex();
    }
    
    /*
     * Deinitialization of memcached driver
     */
    void IODirectIODriver::deinit() throw(CException) {
		if(data_cache.data_ptr) {
			free(data_cache.data_ptr);
		}
		
		//initialize client
		utility::InizializableService::deinitImplementation(init_parameter.client_instance, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);

		
		IODataDriver::deinit();
   }
    
    /*
     * This method retrive the cached object by CSDawrapperUsed as query key and
     * return a pointer to the class ArrayPointer of CDataWrapper type
     */
    void IODirectIODriver::storeRawData(chaos_data::SerializationBuffer *serialization)  throw(CException) {
		CHAOS_ASSERT(serialization)
		serialization->disposeOnDelete = false;
		device_client_channel->putDataOutputChannel(true, (void*)serialization->getBufferPtr(), (uint32_t)serialization->getBufferLen());
		delete(serialization);
    }
    
    /*
     * This method retrive the cached object by CSDawrapperUsed as query key and
     * return a pointer to the class ArrayPointer of CDataWrapper type
     */
    char* IODirectIODriver::retriveRawData(size_t *dim)  throw(CException) {
		char* result;
		device_client_channel->requestLastOutputData(current_endpoint_port, current_endpoint_index);
		wait_get_answer.wait(1000);
		*dim = (size_t)data_cache.data_len;
		result = (char*)data_cache.data_ptr;
		std::memset(&data_cache, 0, sizeof(IODData));
        return result;
    }
    
	//we have request data and this arrive with the put opcode
	void IODirectIODriver::consumePutEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderPutOpcode header, void *channel_data, uint32_t channel_data_len) {
		data_cache.data_len = channel_data_len;
		data_cache.data_ptr = channel_data;
		wait_get_answer.unlock();
	}
	
	void IODirectIODriver::consumeGetEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcode header, void *channel_data, uint32_t channel_data_len) {
	}
	
    /*
     Update the driver configuration
     */
    chaos_data::CDataWrapper* IODirectIODriver::updateConfiguration(chaos_data::CDataWrapper* newConfigration) {
		//checkif someone has passed us the device indetification
        if(newConfigration->hasKey(DatasetDefinitionkey::DEVICE_ID)){
            dataKey = newConfigration->getStringValue(DatasetDefinitionkey::DEVICE_ID);
			device_client_channel->setDeviceID(dataKey);
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
				if(tokens.size()==2) {
					IODirectIODriver_DLDBG_ << "Data proxy server description " << tokens[0];
					IODirectIODriver_DLDBG_ << "Data proxy server Endpoint " << tokens[1];
					init_parameter.client_instance->addServer(tokens[0]);
					if(device_client_channel) device_client_channel->setEndpoint(boost::lexical_cast<uint16_t>(tokens[1]));
				} else {
					IODirectIODriver_DLDBG_ << "Bad server configuration";
				}
            }
		}
        return NULL;
    }
}