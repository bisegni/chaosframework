/*
 *	AsioEventHandler.h
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
#include "AsioEventHandler.h"

using namespace boost;
using namespace chaos;
using namespace chaos::event;

AsioEventHandler::AsioEventHandler(const boost::asio::ip::address& listen_address,
                                   const boost::asio::ip::address& multicast_address,
                                   boost::asio::io_service& io_service,
                                   unsigned short mPort) : socket_(io_service) {
        // Create the socket so that multiple may be bound to the same address.
    boost::asio::ip::udp::endpoint listen_endpoint(listen_address, mPort);
    socket_.open(listen_endpoint.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(listen_endpoint);
    
        // Join the multicast group.
    socket_.set_option(
                       boost::asio::ip::multicast::join_group(multicast_address));
    
    socket_.async_receive_from(
                               boost::asio::buffer(data_, max_length), sender_endpoint_,
                               boost::bind(&AsioEventHandler::handle_receive_from, this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
}

void AsioEventHandler::handle_receive_from(const boost::system::error_code& error,
                                           size_t bytes_recvd){
    if (!error) {
        std::cout.write(data_, bytes_recvd);
        std::cout << "received byte:"<<bytes_recvd<<" "<< std::endl;
        
        socket_.async_receive_from(boost::asio::buffer(data_, max_length), sender_endpoint_,
                                   boost::bind(&AsioEventHandler::handle_receive_from, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));
    }
}