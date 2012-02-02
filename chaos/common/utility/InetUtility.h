    //
    //  InetUtility.h
    //  CHAOSFramework
    //
    //  Created by Bisegni Claudio on 01/02/12.
    //  Copyright (c) 2012 INFN. All rights reserved.
    //

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
            struct hostent *server;
            
            
            portno = basePort;
            
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) return -1;
            
            server = gethostbyname("127.0.0.1");
            if (server == NULL) {
                return -1;
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, 
                  (char *)&serv_addr.sin_addr.s_addr,
                  server->h_length);
            int err = 0;
            for (; err>=0;) {
                serv_addr.sin_port = htons(portno);
                err = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
                
                if(err>=0)portno++;
            }
            close(sockfd);
            return portno;
        }
    };
    
}

#endif
