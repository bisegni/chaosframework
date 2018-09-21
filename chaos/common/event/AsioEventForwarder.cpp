 /*
 *	AsioEventForwarder.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 26/08/12.
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
#include <string.h>
#include <chaos/common/event/AsioEventForwarder.h>
#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
using namespace std;
using namespace boost;
using namespace chaos;
using namespace chaos::common::event;

AsioEventForwarder::AsioEventForwarder(const boost::asio::ip::address& multicast_address,
                                       unsigned short mPort,
                                       boost::asio::io_service& io_service
                                       ) :_socket(io_service), _endpoint(multicast_address, mPort) {
    hanlderID = common::utility::UUIDUtil::generateUUIDLite();
    sent = true;
}

AsioEventForwarder::~AsioEventForwarder() {}

/*
 init the event adapter
 */
void AsioEventForwarder::init() {
    _socket.connect(_endpoint);
    CObjectProcessingPriorityQueue<EventDescriptor>::init(1);
}

/*
 deinit the event adapter
 */
void AsioEventForwarder::deinit() {
    boost::unique_lock<boost::mutex> lock( wait_answer_mutex );
    _socket.close();
    CObjectProcessingPriorityQueue<EventDescriptor>::clear();
    CObjectProcessingPriorityQueue<EventDescriptor>::deinit();
}

bool AsioEventForwarder::submitEventAsync(EventDescriptorSPtr event) {
    return CObjectProcessingPriorityQueue<EventDescriptor>::push(MOVE(event), event->getEventPriority());
}

void AsioEventForwarder::processBufferElement(EventDescriptorSPtr priorityElement) {
    _socket.send(boost::asio::buffer(priorityElement->getEventData(), priorityElement->getEventDataLength()));
}
