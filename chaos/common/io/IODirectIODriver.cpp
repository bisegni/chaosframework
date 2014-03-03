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

#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common//global.h>

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
        IODataDriver::init();

		if(!init_parameter.client_instance) throw CException(-1, "No client configured", __PRETTY_FUNCTION__);
		if(!init_parameter.endpoint_instance) throw CException(-1, "No endpoint configured", __PRETTY_FUNCTION__);
		
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
        IODataDriver::deinit();
		if(data_cache.data_ptr) {
			free(data_cache.data_ptr);
		}
    }
    
    /*
     * This method retrive the cached object by CSDawrapperUsed as query key and
     * return a pointer to the class ArrayPointer of CDataWrapper type
     */
    void IODirectIODriver::storeRawData(size_t dataDim, const char * buffer)  throw(CException) {
		device_client_channel->putDataOutputChannel(true, (void*)buffer, (uint32_t)dataDim);
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

        if(newConfigration->hasKey(DataProxyConfigurationKey::CS_DM_LD_SERVER_ADDRESS)){
            LMEMDRIVER_ << "Get the DataManager LiveData address value";
            auto_ptr<chaos_data::CMultiTypeDataArrayWrapper> liveMemAddrConfig(newConfigration->getVectorValue(DataProxyConfigurationKey::CS_DM_LD_SERVER_ADDRESS));
			size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
            for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){
                string serverDesc = liveMemAddrConfig->getStringElementAtIndex(idx);
				
				init_parameter.client_instance->addServer(serverDesc);
				
            }
		}
        return NULL;
    }
}