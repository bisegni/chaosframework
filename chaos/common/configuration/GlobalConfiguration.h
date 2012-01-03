//
//  GlobalConfiguration.h
//  ControlSystemLib
//
//  Created by Bisegni Claudio on 27/12/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ControlSystemLib_GlobalConfiguration_h
#define ControlSystemLib_GlobalConfiguration_h

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
    
    static const regex MetadataServerHostNameRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+){0,1}:[0-9]{4,5}");
    static const regex MetadataServerIPRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}");
    static const regex LocalServerIPRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    
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
        
        
        GlobalConfiguration():desc("Allowed options"){};
        ~GlobalConfiguration(){};

        
    public:

        /*
         parse the tandard startup parameters
         */
        virtual void preParseStartupParameters() throw (CException);
        /*
         parse the tandard startup parameters
         */
        virtual void parseStartupParameters(int,const char* argv[]) throw (CException);
        
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
            return vm.count(optName);
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
            bool isHostnameAndPort = regex_match(mdsAddress, MetadataServerHostNameRegExp);
            bool isIpAndPort  = regex_match(mdsAddress, MetadataServerIPRegExp);
            if(!isHostnameAndPort && !isIpAndPort)
                throw new CException(1, "Bad server address", "GlobalConfiguration::addMetadataServerAddress");
                    
                //address can be added
            configuration.addStringValue(CS_LIB_METADATASET_ADDRESS, mdsAddress);
        }
        
        /**
         *Add the metadataserver address
         */
        void addLocalServerAddress(const char * mdsAddress) throw (CException) {
            bool isHostnameAndPort = regex_match(mdsAddress, LocalServerIPRegExp);
            bool isIpAndPort  = regex_match(mdsAddress, LocalServerIPRegExp);
            if(!isHostnameAndPort && !isIpAndPort)
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
            return configuration.getStringValue(CS_LIB_METADATASET_ADDRESS);
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
            return configuration.hasKey(CS_LIB_METADATASET_ADDRESS);
        }
    };  
}



#endif
