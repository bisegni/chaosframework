/*
 *	IODirectIODriver.h
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

#ifndef __CHAOSFramework__IODirectIODriver__
#define __CHAOSFramework__IODirectIODriver__

#include <set>
#include <string>

#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/ObjectSlot.h>

#include <chaos/common/network/URLServiceFeeder.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace chaos{
	
	class NetworkBorker;
	
    using namespace std;
    using namespace boost;
	
	namespace chaos_utility = chaos::common::utility;
    namespace chaos_direct_io = chaos::common::direct_io;
    namespace chaos_dio_channel = chaos::common::direct_io::channel;
    
    /*!
	 Struct for initialization of the io driver
     */
    typedef struct IODirectIODriverInitParam {
		chaos::NetworkBroker					*network_broker;
        chaos_direct_io::DirectIOClient			*client_instance;
        chaos_direct_io::DirectIOServerEndpoint *endpoint_instance;
    } IODirectIODriverInitParam, *IODirectIODriverInitParamPtr;
    
	
	typedef struct IODData {
		void *data_ptr;
		uint32_t data_len;
	} IODData;
	
	typedef struct IODirectIODriverClientChannels {
		chaos_direct_io::DirectIOClientConnection		*connection;
		chaos_dio_channel::DirectIODeviceClientChannel	*device_client_channel;
	} IODirectIODriverClientChannels;
	
    /*!
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(IODirectIODriver, IODataDriver),
	public NamedService,
	public common::network::URLServiceFeederHandler,
	private chaos_dio_channel::DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler,
	protected chaos_direct_io::DirectIOClientConnectionEventHandler {
		REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(IODirectIODriver)
		string dataKey;
        IODirectIODriverInitParam init_parameter;
		std::set<std::string> registered_server;
		
		uint16_t	current_endpoint_p_port;
		uint16_t	current_endpoint_s_port;
		uint16_t	current_endpoint_index;
		
		chaos_dio_channel::DirectIODeviceServerChannel	*device_server_channel;
		chaos_utility::ObjectSlot<IODirectIODriverClientChannels*> channels_slot;
		
		WaitSemaphore wait_get_answer;
		boost::shared_mutex mutext_feeder;
		
		int consumePutEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderPutOpcode *header,
							 void *channel_data,
							 uint32_t channel_data_len,
							 common::direct_io::DirectIOSynchronousAnswerPtr synchronous_answer);
		int consumeGetEvent(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderGetOpcode *header,
							 void *channel_data,
							 uint32_t channel_data_len,
							 common::direct_io::DirectIOSynchronousAnswerPtr synchronous_answer);
		
		IODData data_cache;
		boost::atomic<uint8_t> read_write_index;
		chaos::common::network::URLServiceFeeder connectionFeeder;

	protected:
		void disposeService(void *service_ptr);
		void* serviceForURL(const common::network::URL& url, uint32_t service_index);
		void handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
						 chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
    public:
        
        IODirectIODriver(std::string alias);
        virtual ~IODirectIODriver();
        
		void setDirectIOParam(IODirectIODriverInitParam& _init_parameter);
		
        /*
         * Init method
         */
        void init(void *init_parameter) throw(CException);
        
        /*
         * Deinit method
         */
        void deinit() throw(CException);
        
        /*
         * storeRawData
         */
        virtual void storeRawData(chaos_data::SerializationBuffer *serialization)  throw(CException);
        
        /*
         * retriveRawData
         */
        virtual char * retriveRawData(size_t *dim=NULL)  throw(CException);
        /*
         * updateConfiguration
         */
        chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
    };
}


#endif /* defined(__CHAOSFramework__IODirectIODriver__) */
