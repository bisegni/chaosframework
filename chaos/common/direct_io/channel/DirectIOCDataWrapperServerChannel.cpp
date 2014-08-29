/*
 *	DirectIOCDataWrapperServerChannel.cpp
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

#include <chaos/common/direct_io/channel/DirectIOCDataWrapperServerChannel.h>


namespace chaos_data = chaos::common::data;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;


DirectIOCDataWrapperServerChannel::DirectIOCDataWrapperServerChannel(std::string alias):DirectIOVirtualServerChannel(alias, DIOCDC_Channel_Index), cdatawrapper_handler(NULL) {
	//set this class as delegate for the endpoint
	DirectIOVirtualServerChannel::setDelegate(this);
}

int DirectIOCDataWrapperServerChannel::consumeDataPack(DirectIODataPack *dataPack, DirectIOSynchronousAnswerPtr synchronous_answer) {
	CHAOS_ASSERT(cdatawrapper_handler)
	//get CData Wrapper
	chaos_data::CDataWrapper *cdata = new chaos_data::CDataWrapper(static_cast<const char *>(dataPack->channel_data));
	
	//forward to hanlde
	cdatawrapper_handler->consumeCDataWrapper(dataPack->header.dispatcher_header.fields.channel_opcode, cdata);
																   
	//delete pack
	delete dataPack;
	
	return 0;
}

void DirectIOCDataWrapperServerChannel::setHandler(DirectIOCDataWrapperServerChannelHandler *handler) {
	//associate the exepternal delegate
	cdatawrapper_handler = handler;
}