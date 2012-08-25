/*
 *	AsioEventForwarder.h
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

#ifndef __CHAOSFramework__AsioEventForwarder__
#define __CHAOSFramework__AsioEventForwarder__


#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace chaos {
    namespace event{
        using namespace boost;
        
        class AsioEventForwarder {
            std::string hanlderID;
            
        public:
            AsioEventForwarder(const boost::asio::ip::address& multicast_address,
                               unsigned short mPort,
                               boost::asio::io_service& io_service);
            
            void handle_send_to(const boost::system::error_code& error, std::size_t bytes_transferred);
            
            void sendDataAsync(const unsigned char *buffer, uint16_t length);
        private:
            boost::asio::ip::udp::socket _socket;
            boost::asio::ip::udp::endpoint _endpoint;
            enum { max_length = 1024 };
            char data_[max_length];
        };
    }
}

#endif /* defined(__CHAOSFramework__AsioEventForwarder__) */
