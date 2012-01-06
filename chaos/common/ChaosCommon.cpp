//
//  log.cpp
//  ControlSystemLib
//
//  Created by Bisegni Claudio on 19/12/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#include "global.h"
#include "ChaosCommon.h"


#include <boost/logging/format.hpp>
#include <boost/logging/writer/ts_write.hpp>


using namespace chaos;
using namespace boost::logging;


BOOST_DEFINE_LOG(g_l, log_type)
BOOST_DEFINE_LOG_FILTER(g_l_filter, boost::logging::level::holder)

    //BOOST_DEFINE_LOG(g_l, log_type)
    //BOOST_DEFINE_LOG_FILTER(g_l_filter, level::holder)
/*template<class T>
ChaosCommon<T>::ChaosCommon() {
    GlobalConfiguration::getInstance()->preParseStartupParameters();
}
template<class T>
ChaosCommon<T>::~ChaosCommon(){
    
}
template<class T>
void ChaosCommon<T>::init(int argc, const char* argv[])  throw(CException) {
        // Add formatters and destinations
        // That is, how the message is to be formatted...
    g_l()->writer().add_formatter( formatter::time("$hh:$mm.$ss ") );
    g_l()->writer().add_formatter( formatter::append_newline() );
    
        //        ... and where should it be written to
    g_l()->writer().add_destination( destination::cout() );
    g_l()->writer().add_destination( destination::file("out.txt") );
    g_l()->turn_cache_off();
    
    try {
        if(argv != NULL){
            GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
        }
        
            //the version constant are defined into version.h
            //file generate to every compilation
        PRINT_LIB_HEADER
        
            //find our ip
        scanForLocalNetworkAddress();

    } catch (CException &e) {
        DECODE_CHAOS_EXCEPTION(e);
    }
    
}

template<class T>
GlobalConfiguration *ChaosCommon<T>::getGlobalConfigurationInstance() {
    return GlobalConfiguration::getInstance();
}

template<class T>
void ChaosCommon<T>::scanForLocalNetworkAddress(){
        struct ifaddrs * ifAddrStruct=NULL;
        struct ifaddrs * ifa=NULL;
#if __APPLE__
        const char *interfaceName  = "en0";
#elif __linux__
        const char *interfaceName = "eth0";
#endif
        void * tmpAddrPtr=NULL;
        
        LAPP_ << "Scan for local network interface and ip";
        getifaddrs(&ifAddrStruct);
        
        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
            if(strcmp(interfaceName, ifa->ifa_name)!= 0) continue;
            
            
            
            if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
                                                      // is a valid IP4 Address
                tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                LAPP_ << "Interface " << ifa->ifa_name << " address " << addressBuffer << "<- this will be choosen";
                GlobalConfiguration::getInstance()->addLocalServerAddress(addressBuffer);
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
        LAPP_ << "The local address chose is:  " << GlobalConfiguration::getInstance()->getLocalServerAddress();
}*/