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
#ifndef ChaosFramework_GlobalConfiguration_h
#define ChaosFramework_GlobalConfiguration_h

#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/network/CNodeNetworkAddress.h>

#include <map>
#include <set>
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
    
#define CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(t,x,y,z)\
t x = z;\
if(hasOption(y)){\
x = getOption<t>(y);\
}

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
    
    CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::network::CNetworkAddress, VectorMetadatserver);
    CHAOS_DEFINE_MAP_FOR_TYPE(std::string, std::string, MapStrKeyStrValue);
    
    /*
     Central class for all CHOAS framework configuraitons
     */
    class GlobalConfiguration:
    public chaos::common::utility::Singleton<GlobalConfiguration> {
        //for program option
        po::variables_map vm;
        po::options_description desc;
        //for contain program option value and other usefull think
        ChaosUniquePtr<chaos_data::CDataWrapper> configuration;
        friend class chaos::common::utility::Singleton<GlobalConfiguration>;

        
        GlobalConfiguration();
        ~GlobalConfiguration();
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
        
        //! contains the key value pair for the rpc implementation
        MapStrKeyStrValue map_kv_param_rpc_impl;
        
        //! contains the key value pair for the rpc implementation
        MapStrKeyStrValue map_kv_param_directio_srv_impl;
        
        MapStrKeyStrValue map_kv_param_directio_clnt_impl;
        
        //!map for key value used by script virtual machine
        MapStrKeyStrValue map_kv_param_script_vm;
        
        //fill the rpc
//        void fillKVParameter(std::map<std::string, std::string>& kvmap,
//                             const std::string& kv_string,
//                             const std::string& regex);
    public:
        void loadStartupParameter(int, const char* argv[]) throw (CException);
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
        void parseStartupParameters(int, const char* argv[]) throw (CException);
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
        void addOptionZeroTokens(const char* name,
                                 const char* description,
                                 bool *default_variable) throw (CException);
        
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
         Add a custom option
         */
        template<typename T>
        void addOptionZeroToken(const char* name,
                                const char* description)  throw (CException) {
            try{
                const po::value_semantic* s = po::value<T>()->zero_tokens();
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
        
        bool isZeroTokensOptions(const std::string& option_name) {
            return vm.count(option_name)>0 && vm.find(option_name)->second.empty();
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
         *return the cdatawrapper that contains the global configuration
         */
        chaos_data::CDataWrapper *getConfiguration();
        
        /**
         * @param conf the cdatawrapper that contains the global configuration
         */
        void setConfiguration(chaos_data::CDataWrapper *conf);
        
        /**
         *Add the metadataserver address
         */
        void addMetadataServerAddress(const string& mdsAddress) throw (CException);
        
        //!close the metadata server list array
        void finalizeMetadataServerAddress();
        
        /**
         *Add the metadataserver address
         */
        void addLocalServerAddress(const std::string& mdsAddress) throw (CException);
        /**
         *Add the metadataserver address
         */
        void addLocalServerBasePort(int32_t localDefaultPort) throw (CException);
        
        //!return the hostname of the host that run chaos node
        std::string getHostname();
        
        /*
         return the address of metadataserver
         */
        std::string getMetadataServerAddress();
        
        /*
         return the address list of multiple configured metadataserver
         */
        VectorMetadatserver getMetadataServerAddressList();
        
        /*
         return the address of metadataserver
         */
        string getLocalServerAddress();
        
        /*
         return the address of metadataserver
         */
        int32_t getLocalServerBasePort();
        
        string getLocalServerAddressAnBasePort();
        
        /*
         return the address of metadataserver
         */
        bool isMEtadataServerConfigured();
        
        //! return the rpc implementation kevy value map
        MapStrKeyStrValue& getRpcImplKVParam();
        
        //! return the directio server implementation kevy value map
        MapStrKeyStrValue& getDirectIOServerImplKVParam();
        
        //! return the directio client implementation key value map
        MapStrKeyStrValue& getDirectIOClientImplKVParam();
        
        //! return the script virtualmachine key value parameter
        MapStrKeyStrValue& getScriptVMKVParam();
        
        static void fillKVParameter(std::map<std::string, std::string>& kvmap,
                                    const std::vector<std::string>& kv_vector,
                                    const std::string& regex);
    };
}
#endif
