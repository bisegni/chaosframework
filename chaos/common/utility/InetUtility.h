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

#ifndef CHAOSFramework_InetUtility_h
#define CHAOSFramework_InetUtility_h

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <chaos/common/global.h>

#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>
#define SERVERHOSTNAME "[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}"
#define SERVERIPANDPORT "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}"
#define SERVERIP "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b"
namespace chaos {
    namespace common {
        namespace utility {
            //! Regular expression for check server hostname and port
        //    static const char * const ServerHostName = "[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}";
            //! Regular expression for check server ip and port
        //    static const char * const ServerIPAndPort = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}";
            //! Regular expression for check server ip
        //    static const char * const ServerIP = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b";
            
            
            class InetUtility {
                
            public:
                
                struct InterfaceInfo {
                    std::string interface_name;
                    std::string address;
                    
                    InterfaceInfo(const std::string& _interface_name,
                                  const std::string& _address):
                    interface_name(_interface_name),
                    address(_address){}
                    
                    InterfaceInfo(const InterfaceInfo& _info):
                    interface_name(_info.interface_name),
                    address(_info.address){}
                };
                
                static void checkInterfaceName(std::vector<InterfaceInfo>& interface_infos,
                                               const std::string& interface_found,
                                               const std::string& interface_address);
                
                //! Network address scan
                /*!
                 This methdo will take care for network address scan process
                 */
                static std::string scanForLocalNetworkAddress(std::string _eth_interface_name = std::string());
                
                //! Network port scan
                /*!
                 This methdo will take care for search nex available port starting from a base value
                 */
                
                static int scanForLocalFreePort(int basePort);
                static bool checkWellFormedHostPort(std::string host_port);
                static bool checkWellFormedHostNamePort(std::string host_port);
                static bool checkWellFormedHostIpPort(std::string host_port);
                static void queryDns(std::string hostname, std::vector<std::string>& resolved_endpoints);
                
                static std::string getHostname();
#define		STRIP_TO_UI64(x)  boost::asio::ip::address_v4::from_string(x)
#define		UI64_TO_STRIP(i)  boost::asio::ip::address_v4(i).to_string()
            };
        }
    }
}
#endif
