/*	
 *	GlobalConfiguration.h
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
#ifndef ChaosFramework_GlobalConfiguration_h
#define ChaosFramework_GlobalConfiguration_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/cconstants.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/Singleton.h>

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace chaos {
    
    using namespace std;
    using namespace boost;
    namespace po = boost::program_options;
    
#define CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(t,x,y,z)\
t x;\
if(hasOption(y)){\
x = getOption<t>(y);\
}else{\
x = z;\
}\

#define CHECK_OPTION_WITH_DEFAULT(t,x,y,z)\
if(hasOption(y)){\
x = getOption<t>(y);\
}else{\
x = z;\
}\

#define CHECK_OPTION(t,x,y)\
if(hasOption(y)){\
x = getOption<t>(y);\
}
    
#define CHECK_AND_DEFINE_OPTION(t,x,y)\
t x;\
if(hasOption(y)){\
x = getOption<t>(y);\
}
    
#define CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(x,y)\
bool x;\
x = hasOption(y);
    
        //! Regular expression for check server hostname and port
    static const regex ServerHostNameRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}");
        //! Regular expression for check server ip and port
    static const regex ServerIPAndPortRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}");
        //! Regular expression for check server ip
    static const regex ServerIPRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    
    /*
     Central class for all CHOAS framework configuraitons
     */
    class GlobalConfiguration : public Singleton<GlobalConfiguration>{
            //for program option
        po::variables_map vm;
        po::options_description desc;
            //for contain program option value and other usefull think
        CDataWrapper configuration;
        friend class Singleton<GlobalConfiguration>;
        
        
        GlobalConfiguration():desc("!CHAOS Framework Allowed options:"){};
        ~GlobalConfiguration(){};
        
        
    public:
        
        /*
         parse the tandard startup parameters
         */
        virtual void preParseStartupParameters() throw (CException);
        /*
         parse the tandard startup parameters
         */
        virtual void parseStartupParameters(int, char* argv[]) throw (CException);
        
        /*
         Add a custom option
         */
        void addOption(const char* name,
                       const po::value_semantic* s,
                       const char* description) throw (CException);
        /*
         Add a custom option
         */
        void addOption(const char* name, 
                       const char* description)  throw (CException);
        
        /*
         return the presence of the option name
         */
        bool hasOption(const char* optName){
            return vm.count(optName)>0;
        }
        
        
        template<typename T>
        T getOption(const char* optName){
            return vm[optName].as<T>();
        }
        
        /**
         *return the cdatawrapper that contains the global configuraiton
         */
        CDataWrapper *getConfiguration(){
            return &configuration;
        }
        
        /**
         *Add the metadataserver address
         */
        void addMetadataServerAddress(string& mdsAddress) throw (CException) {
            bool isHostnameAndPort = regex_match(mdsAddress, ServerHostNameRegExp);
            bool isIpAndPort  = regex_match(mdsAddress, ServerIPAndPortRegExp);
            if(!isHostnameAndPort && !isIpAndPort)
                throw new CException(1, "Bad server address", "GlobalConfiguration::addMetadataServerAddress");
            
                //address can be added
            configuration.addStringValue(LiveHistoryMDSConfiguration::CS_LIB_METADATASET_ADDRESS, mdsAddress);
        }
        
        /**
         *Add the metadataserver address
         */
        void addLocalServerAddress(const char * mdsAddress) throw (CException) {
            bool isIp = regex_match(mdsAddress, ServerIPRegExp);
            if(!isIp)
                throw new CException(1, "Bad server address", "GlobalConfiguration::addMetadataServerAddress");
            
                //address can be added
            configuration.addStringValue("local_ip", mdsAddress);
        }
        /**
         *Add the metadataserver address
         */
        void addLocalServerBasePort(int32_t localDefaultPort) throw (CException) {
            configuration.addInt32Value("base_port", localDefaultPort);
        }
        /*
         return the address of metadataserver
         */
        string getMetadataServerAddress() {
            return configuration.getStringValue(LiveHistoryMDSConfiguration::CS_LIB_METADATASET_ADDRESS);
        }
        
        /*
         return the address of metadataserver
         */
        string getLocalServerAddress() {
            return configuration.getStringValue("local_ip");
        }
        
        /*
         return the address of metadataserver
         */
        int32_t getLocalServerBasePort() {
            return configuration.getInt32Value("base_port");
        }
        
        string getLocalServerAddressAnBasePort(){
            char buf[128];
            string addr = configuration.getStringValue("local_ip");
            sprintf ( buf, "%s:%d", addr.c_str(), (int)configuration.getInt32Value("base_port"));
            addr.assign(buf);
            return addr; 
        }
        
        /*
         return the address of metadataserver
         */
        bool isMEtadataServerConfigured() {
            return configuration.hasKey(LiveHistoryMDSConfiguration::CS_LIB_METADATASET_ADDRESS);
        }
    };  
}
#endif
