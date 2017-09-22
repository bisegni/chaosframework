/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__AsioEventHandler__
#define __CHAOSFramework__AsioEventHandler__
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


namespace chaos {
    namespace common {
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
                ~AsioEventHandler();
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
}

#endif /* defined(__CHAOSFramework__AsioEventHandler__) */
