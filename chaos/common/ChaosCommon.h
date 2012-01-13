    //
    //  Common.h
    //  ChaosFramework
    //
    //  Created by Bisegni Claudio on 19/12/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_ChaosCommon_h
#define ChaosFramework_ChaosCommon_h

#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#include <chaos/common/global.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/logging/format.hpp>
#include <boost/logging/writer/ts_write.hpp>


//! Default chaos namespace used to group all common api
namespace chaos {    
    
    using namespace boost::logging;

    //! Chaos common engine class
    /*! 
        This is the base class for the other toolkit, it thake care to initialize all common
        resource used for the base chaos function
     */
    template<class T>
    class ChaosCommon: public Singleton<T>{
        //! Network address scan
        /*! 
         This methdo will take care for network address scan process
         */
        void scanForLocalNetworkAddress() {
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
        }
        
    protected:
        //! Constructor Method
        /*! 
          Thi method call the \ref GlobalConfiguration::preParseStartupParameters method, starting the 
          allocation of the startup framework parameter
         */
        ChaosCommon(){
            GlobalConfiguration::getInstance()->preParseStartupParameters();
        }

        
        //! Destructor method
        /*! 
         A more elaborate description of the destructor.
         */
        virtual ~ChaosCommon() {};

        //! Initialization methdo
        /*! 
         This virtual method can be extended by toolkit subclass for specialized initializaion
         in themain toolkit subclass of ChaosCommon
         */
        virtual void init(int argc, const char* argv[])  throw(CException) {
            try {
                if(argv != NULL){
                    GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
                }
                
                    // Add formatters and destinations
                    // That is, how the message is to be formatted...
                g_l()->writer().add_formatter( formatter::time("$hh:$mm.$ss ") );
                g_l()->writer().add_formatter( formatter::append_newline() );
                
                    //        ... and where should it be written to
                if(GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(UserOption::OPT_LOG_ON_CONSOLE)) {
                    g_l()->writer().add_destination( destination::cout() );
                }
                
                if(GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(UserOption::OPT_LOG_ON_FILE) &&  
                   GlobalConfiguration::getInstance()->getConfiguration()->hasKey(UserOption::OPT_LOG_FILE)) {
                    g_l()->writer().add_destination( destination::file(GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(UserOption::OPT_LOG_FILE).c_str()) );
                }
                
                g_l()->turn_cache_off();
                

                    //the version constant are defined into version.h
                    //file generate to every compilation
                PRINT_LIB_HEADER
                
                    //find our ip
                scanForLocalNetworkAddress();
                
            } catch (CException &e) {
                DECODE_CHAOS_EXCEPTION(e);
            }
            
        }

        
    public:
         //! Return the global configuration for the current singleton instance
        /*!
         \return the GlobalConfiguration pointer to global instance
         */
        GlobalConfiguration *getGlobalConfigurationInstance() {
            return GlobalConfiguration::getInstance();
        }

    }; 
}


#endif
