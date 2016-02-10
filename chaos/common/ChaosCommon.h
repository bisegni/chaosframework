/*
 *	ChaosCommon.h
 *	!CHAOS
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
#ifndef ChaosFramework_ChaosCommon_h
#define ChaosFramework_ChaosCommon_h

#include <errno.h>
#include <sstream>
#include <fstream>
#include <csignal>
#include <signal.h>
#include <sys/utsname.h>
#include <chaos/common/global.h>
#include <chaos/common/log/LogManager.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/async_central/AsyncCentralManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/metadata_logging/MetadataLoggingManager.h>

#ifdef __CHAOS_DEBUG_MEMORY__
#include <unistd.h>
#include <iostream>
#include <string>
#include <chaos/common/debug/tracey.hpp>
//#include <chaos/common/debug/ProcInfo.h>
#include <boost/format.hpp>
#endif

//! Default chaos namespace used to group all common api
namespace chaos {
    
    static void print_memory_leak_status(int signal_number) {
#ifdef __CHAOS_DEBUG_MEMORY__
        std::string status = tracey::summary();
        std::cout << boost::str(boost::format("___________debug_memory_leak_status____________\n%1%\n___________debug_memory_leak_status____________")%status) << std::endl;
        status.resize(0);
#endif
    }
    
    //! Chaos common engine class
    /*!
     This is the base class for the other toolkit, it thake care to initialize all common
     resource used for the base chaos function
     */
    template<class T>
    class ChaosCommon:
    public common::utility::Singleton<T>,
    public common::utility::StartableService {
        log::LogManager logManager;
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
    public:
        /*!
         parse a config file before initializzation
         */
        void preparseConfigFile(std::istream &config_file_stream) {
            GlobalConfiguration::getInstance()->loadStreamParameter(config_file_stream);
            GlobalConfiguration::getInstance()->scanOption();
        }
        
        /*!
         parse a config file before initializzation
         */
        void preparseCommandOption(int argc, char* argv[]) {
            GlobalConfiguration::getInstance()->loadStartupParameter( argc, argv);
            GlobalConfiguration::getInstance()->scanOption();
            
            //check if we have a config file
            if(GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_CONF_FILE)) {
                //reload configuraiton from file
                std::string file_option = GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_CONF_FILE);
                std::ifstream option_file_stream;
                option_file_stream.open(file_option.c_str(), std::ifstream::in);
                if(!option_file_stream) {
                    throw chaos::CException(-1, "Error opening configuration file", "Startup sequence");
                }
                //reparse the config file
                preparseConfigFile(option_file_stream);
            }
            //parse the dafult framework option
            GlobalConfiguration::getInstance()->checkDefaultOption();
        }
        
        //! C and C++ attribute parser
        /*!
         Specialized option for startup c and cpp program main options parameter
         */
        void init(int argc, char* argv[]) throw (CException) {
            preparseCommandOption(argc, argv);
            if(argv != NULL) {
                GlobalConfiguration::getInstance()->parseStartupParameters(argc, argv);
            }
            init(NULL);
        }
        //!stringbuffer parser
        /*
         specialized option for string stream buffer with boost semantics
         */
        void init(std::istream &initStream) throw (CException) {
            GlobalConfiguration::getInstance()->parseStringStream(initStream);
            init(NULL);
        }
        
        //! Initialization methdo
        /*!
         This virtual method can be extended by toolkit subclass for specialized initializaion
         in themain toolkit subclass of ChaosCommon
         */
        void init(void *init_data) throw (CException) {
            int err = 0;
            struct utsname u_name;
            
            if (std::signal((int) 29, print_memory_leak_status) == SIG_ERR){
                std::cout << "SIGINFO Signal handler registraiton error";
                exit(-1);
                }
                
                
                // SIGINFO is not defined in ARM architectures
                if (std::signal((int) SIGUSR1, print_memory_leak_status) == SIG_ERR){
                    std::cout << "SIGINFO Signal handler registraiton error";
                    exit(-1);
                }
                //startup logger
                logManager.init();
                
                //print chaos library header
                PRINT_LIB_HEADER
                
                err = uname(&u_name);
                if(err==-1){
                    LAPP_ << "Platform: " << strerror(errno);
                } else {
                    LAPP_ << "Platform: " << u_name.sysname << " " << u_name.nodename << " " << u_name.release << " " << u_name.version << " " << u_name.machine;
                }
                
                LAPP_ << "Boost version: " << (BOOST_VERSION / 100000) << "."<< ((BOOST_VERSION / 100) % 1000)<< "."<< (BOOST_VERSION / 100000);
                LAPP_ << "Compiler Version: " << BOOST_COMPILER;
                LAPP_ << "-----------------------------------------";
                
                //find our ip
                string local_ip;
                if(GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_PUBLISHING_IP)){
                    local_ip = GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_PUBLISHING_IP);
                } else {
                    if(GlobalConfiguration::getInstance()->getConfiguration()->hasKey(InitOption::OPT_PUBLISHING_INTERFACE))
                        local_ip = common::utility::InetUtility::scanForLocalNetworkAddress(GlobalConfiguration::getInstance()->getConfiguration()->getStringValue(InitOption::OPT_PUBLISHING_INTERFACE));
                    else
                        local_ip = common::utility::InetUtility::scanForLocalNetworkAddress();
                }
                GlobalConfiguration::getInstance()->addLocalServerAddress(local_ip);
                
                LAPP_ << "The local address choosen is:  " << GlobalConfiguration::getInstance()->getLocalServerAddress();
                
                //Starting Async central
                common::utility::InizializableService::initImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(), init_data, "AsyncCentralManager", __PRETTY_FUNCTION__);
                common::utility::StartableService::initImplementation(chaos::common::network::NetworkBroker::getInstance(), init_data, "NetworkBroker", __PRETTY_FUNCTION__);
                common::utility::StartableService::startImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__);
                //force first allocation
                common::utility::StartableService::initImplementation(chaos::common::metadata_logging::MetadataLoggingManager::getInstance(), NULL, "MetadataLoggingManager", __PRETTY_FUNCTION__);
                }
                
                void deinit() throw (CException) {
                    //dellocate all
                    chaos::common::network::NetworkBroker::getInstance()->disposeMessageChannel(chaos::common::metadata_logging::MetadataLoggingManager::getInstance()->getMessageChannelInstance());
                    common::utility::StartableService::deinitImplementation(chaos::common::metadata_logging::MetadataLoggingManager::getInstance(), "MetadataLoggingManager", __PRETTY_FUNCTION__);
                    CHAOS_NOT_THROW(common::utility::StartableService::stopImplementation(chaos::common::network::NetworkBroker::getInstance(),  "NetworkBroker", __PRETTY_FUNCTION__););
                    CHAOS_NOT_THROW(common::utility::StartableService::deinitImplementation(chaos::common::network::NetworkBroker::getInstance(),  "AsyncCentralManager", __PRETTY_FUNCTION__););
                    CHAOS_NOT_THROW(common::utility::InizializableService::deinitImplementation(chaos::common::async_central::AsyncCentralManager::getInstance(),  "AsyncCentralManager", __PRETTY_FUNCTION__););
                }
                
                void start() throw (CException) {

                }
                void stop() throw (CException) {
                    
                }
                
                GlobalConfiguration *getGlobalConfigurationInstance() {
                    return GlobalConfiguration::getInstance();
                }
                };
                }
                
#endif
