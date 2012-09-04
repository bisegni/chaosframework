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

#ifndef __CHAOSFramework__AsioEventHandler__
#define __CHAOSFramework__AsioEventHandler__
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace chaos {
    namespace event{
        
        class AsioImplEventServer;
        
        using namespace boost;
        class AsioEventHandler {
            friend class AsioImplEventServer;
            
            std::string hanlderID;
            AsioImplEventServer *asioServer;
        public:
            AsioEventHandler(const boost::asio::ip::address& listen_address,
                             const boost::asio::ip::address& multicast_address,
                             boost::asio::io_service &io_service,
                             unsigned short mPort);
            
            void handle_receive_from(const boost::system::error_code& error,
                                     size_t bytes_recvd);
            
        private:
            boost::asio::ip::udp::socket socket_;
            boost::asio::ip::udp::endpoint sender_endpoint_;
            enum { max_length = 1024 };
            char data_[max_length];
        };
    }
}

#endif /* defined(__CHAOSFramework__AsioEventHandler__) */
