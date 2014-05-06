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

#include <chaos/common/global.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InetUtility.h>

#include <string>
#include <istream>
#include <boost/shared_ptr.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;
namespace chaos_data = chaos::common::data;

namespace chaos {
    
    using namespace std;
    using namespace boost;

	
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
    
    /*
     Central class for all CHOAS framework configuraitons
     */
    class GlobalConfiguration : public Singleton<GlobalConfiguration>{
            //for program option
        po::variables_map vm;
        po::options_description desc;
            //for contain program option value and other usefull think
        chaos_data::CDataWrapper configuration;
        friend class Singleton<GlobalConfiguration>;
        
        
        GlobalConfiguration():desc("!CHAOS Framework Allowed options"){};
        ~GlobalConfiguration(){};
        //! Parse the options
        /*!
         Generalized parser option function
         */
        void parseParameter(const po::basic_parsed_options<char>& optionsParser) throw (CException);
        
        //! recognize the
        /*!
         Generalized parser option function
         */
        int32_t filterLogLevel(string& levelStr) throw (CException);
    public:
		void loadStartupParameter(int, char* argv[]) throw (CException);
		void loadStreamParameter(std::istream &config_file) throw (CException);
		void scanOption() throw (CException);
		void checkDefaultOption() throw (CException);
            //! startup parameter pre setup
        /*
         Set up all stardard input attribute map
         */
        void preParseStartupParameters() throw (CException);
            //! C and C++ attribute parser
        /*!
         Specialized option for startup c and cpp program main options parameter
         */
        void parseStartupParameters(int, char* argv[]) throw (CException);
            //!stringbuffer parser
        /*
         specialized option for string stream buffer with boost semantics
         */
        void parseStringStream(std::istream &) throw (CException);
        
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
                       const char* description) throw (CException);
        /*
         Add a custom option
         */
		template<typename T>
        void addOption(const char* name, 
                       const char* description)  throw (CException) {
			try{//po::value<T>(&timeout)->default_value(2000)
				const po::value_semantic* s = po::value<T>();
				desc.add_options()(name, s, description);
			}catch (po::error &e) {
				throw CException(0, e.what(), "GlobalConfiguration::addOption");
			}
		}
		/*
         Add a custom option
         */
		template<typename T>
        void addOption(const char* name,
						const char* description,
						T default_value,
					   bool multivalue = false)  throw (CException) {
			try{//po::value<T>(&timeout)->default_value(2000)
				const po::value_semantic* s = multivalue?po::value<T>()->default_value(default_value)->multitoken():po::value<T>()->default_value(default_value);
				desc.add_options()(name, s, description);
			}catch (po::error &e) {
				throw CException(0, e.what(), "GlobalConfiguration::addOption");
			}
		}
		/*
         Add a custom option
         */
		template<typename T>
        void addOption(const char* name,
						const char* description,
						T default_value,
						T *default_variable,
					   bool multivalue = false)  throw (CException) {
			try{
				const po::value_semantic* s = multivalue?po::value<T>(default_variable)->default_value(default_value)->multitoken():po::value<T>(default_variable)->default_value(default_value);
				desc.add_options()(name, s, description);
			}catch (po::error &e) {
				throw CException(0, e.what(), "GlobalConfiguration::addOption");
			}
		}
		/*
         Add a custom option
         */
		template<typename T>
        void addOption(const char* name,
					   const char* description,
					   T *default_variable,
					   bool multivalue = false)  throw (CException) {
			try{
				const po::value_semantic* s = multivalue?po::value<T>(default_variable)->multitoken():po::value<T>(default_variable);
				desc.add_options()(name, s, description);
			}catch (po::error &e) {
				throw CException(0, e.what(), "GlobalConfiguration::addOption");
			}
		}
        /*
         return the presence of the option name
         */
        bool hasOption(const char* optName){
            return vm.count(optName)>0;
        }
        
        
        template<typename T>
        T getOption(const char* optName) throw (CException){
			if(vm.count(optName)==0) {
                string opt=string("option:") + "\""+optName + "\"" + string(" not given");
                throw CException(2,opt.c_str(),"GlobalConfiguration::getOption");
            }
			return vm[optName].as<T>();
		}
        
        /**
         *return the cdatawrapper that contains the global configuraiton
         */
        chaos_data::CDataWrapper *getConfiguration(){
            return &configuration;
        }
        
        /**
         *Add the metadataserver address
         */
        void addMetadataServerAddress(string& mdsAddress) throw (CException) {
            bool isHostnameAndPort = regex_match(mdsAddress, ServerHostNameRegExp);
            bool isIpAndPort  = regex_match(mdsAddress, ServerIPAndPortRegExp);
            if(!isHostnameAndPort && !isIpAndPort)
                throw CException(1, "Bad server address", "GlobalConfiguration::addMetadataServerAddress");
            
                //address can be added
            configuration.addStringValue(DataProxyConfigurationKey::CS_LIB_METADATASET_ADDRESS, mdsAddress);
        }
        
        /**
         *Add the metadataserver address
         */
        void addLocalServerAddress(const char * mdsAddress) throw (CException) {
            bool isIp = regex_match(mdsAddress, ServerIPRegExp);
            if(!isIp)
                throw CException(1, "Bad server address", "GlobalConfiguration::addMetadataServerAddress");
            
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
            return configuration.getStringValue(DataProxyConfigurationKey::CS_LIB_METADATASET_ADDRESS);
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
            return configuration.hasKey(DataProxyConfigurationKey::CS_LIB_METADATASET_ADDRESS);
        }
    };  
}
#endif
