/*	
 *	InetUtility.h
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

#ifndef CHAOSFramework_InetUtility_h
#define CHAOSFramework_InetUtility_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

namespace chaos {
    
    class InetUtility {
        
    public:
        
            //! Network address scan
        /*! 
         This methdo will take care for network address scan process
         */
        static void scanForLocalNetworkAddress(string& ipPort) {
            struct ifaddrs * ifAddrStruct=NULL;
            struct ifaddrs * ifa=NULL;
#if __APPLE__
            const char *interfaceName  = "en";
#elif __linux__
            const char *interfaceName = "eth";
#endif
            void * tmpAddrPtr=NULL;
            
            LAPP_ << "Scan for local network interface and ip";
            getifaddrs(&ifAddrStruct);
            
            for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
                if(strstr(ifa->ifa_name, interfaceName)== NULL) continue;
                
                if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
                                                          // is a valid IP4 Address
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    char addressBuffer[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                    LAPP_ << "Interface " << ifa->ifa_name << " address " << addressBuffer << "<- this will be choosen";
                    ipPort.assign(addressBuffer);
                    break;
                } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
                                                                 // is a valid IP6 Address
                    tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
                    char addressBuffer[INET6_ADDRSTRLEN];
                    inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
                    LAPP_ << "Interface " << ifa->ifa_name << " address " << addressBuffer;
                } 
            }
            if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
        }
        
            //! Network port scan
        /*! 
         This methdo will take care for search nex available port starting from a base value
         */
        
        static int scanForLocalFreePort(int basePort) {
            int sockfd, portno;
            struct sockaddr_in serv_addr;
            portno = basePort;
            int err = 0;
            for (; err>=0;) {
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0) return -1;
                bzero((char *) &serv_addr, sizeof(serv_addr));
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                serv_addr.sin_port = htons(portno);
                err = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
                close(sockfd);
                if(err>=0)portno++;
            }
            
            return portno;
        }
    };
}
#endif
