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
using namespace chaos::event;

AsioEventForwarder::AsioEventForwarder(const boost::asio::ip::address& multicast_address,
                                       unsigned short mPort,
                                       boost::asio::io_service& io_service
                                       ) :_endpoint(multicast_address, mPort), _socket(io_service, _endpoint.protocol()){
    hanlderID = UUIDUtil::generateUUIDLite();
    memset(data_, 0, 1024);
}

void AsioEventForwarder::handle_send_to(const boost::system::error_code& error,
                                        std::size_t bytes_transferred) {
    if (!error) {
        memset(data_, 0, 1024);
    }
}

void AsioEventForwarder::sendDataAsync(const unsigned char *buffer, uint16_t length) {
    std::memcpy(data_, buffer, length>1024?1024:length);
    _socket.async_send_to(boost::asio::buffer(data_), _endpoint,
                          boost::bind(&AsioEventForwarder::handle_send_to, this,
                                      boost::asio::placeholders::error));

}