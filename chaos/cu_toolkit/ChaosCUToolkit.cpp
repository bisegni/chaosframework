/*
 *	ChaosCUToolkit.cpp
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
#include <chaos/cu_toolkit/ChaosCUToolkit.h>
#include <chaos/cu_toolkit/data_manager/DataManager.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <chaos/cu_toolkit/control_manager/script/api/api.h>
#include <chaos/cu_toolkit/external_gateway/external_gateway.h>

#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/metadata_logging/MetadataLoggingManager.h>

#include <csignal>
#include <execinfo.h>

using namespace std;
using namespace chaos::common::utility;

using namespace chaos::cu;
using namespace chaos::common::healt_system;
using namespace chaos::cu::data_manager;
using namespace chaos::cu::command_manager;
using namespace chaos::cu::control_manager;
using namespace chaos::cu::driver_manager;
using namespace chaos::common::metadata_logging;

//http://stackoverflow.com/questions/11465148/using-sigaction-c-cpp
void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext) {
    std::cerr << "signal " << sig_num
    << " (" << strsignal(sig_num) << "), address is "
    << info->si_addr << std::endl << std::endl;
    
    void * array[50];
    int size = backtrace(array, 50);
    
    char ** messages = backtrace_symbols(array, size);
    
    // skip first stack frame (points here)
    for (int i = 1; i < size && messages != NULL; ++i)
    {
        char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;
        
        // find parantheses and +address offset surrounding mangled name
        for (char *p = messages[i]; *p; ++p)
        {
            if (*p == '(')
            {
                mangled_name = p;
            }
            else if (*p == '+')
            {
                offset_begin = p;
            }
            else if (*p == ')')
            {
                offset_end = p;
                break;
            }
        }
        
        // if the line could be processed, attempt to demangle the symbol
        if (mangled_name && offset_begin && offset_end &&
            mangled_name < offset_begin)
        {
            *mangled_name++ = '\0';
            *offset_begin++ = '\0';
            *offset_end++ = '\0';
            
            int status;
            char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);
            
            // if demangling is successful, output the demangled function name
            if (status == 0)
            {
                std::cerr << "[bt]: (" << i << ") " << messages[i] << " : "
                << real_name << "+" << offset_begin << offset_end
                << std::endl;
                
            }
            // otherwise, output the mangled function name
            else
            {
                std::cerr << "[bt]: (" << i << ") " << messages[i] << " : "
                << mangled_name << "+" << offset_begin << offset_end
                << std::endl;
            }
            free(real_name);
        }
        // otherwise, print the whole line
        else
        {
            std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
        }
    }
    std::cerr << std::endl;
    
    free(messages);
    
    exit(EXIT_FAILURE);
}

void kill_err_hdlr(int sig_num, siginfo_t * info, void * ucontext) {
    std::cerr << "signal " << sig_num
    << " (" << strsignal(sig_num) << "), address is "
    << info->si_addr << std::endl << std::endl;
    exit(EXIT_FAILURE);
}

//boost::mutex ChaosCUToolkit::monitor;
//boost::condition ChaosCUToolkit::endWaithCondition;
chaos::WaitSemaphore ChaosCUToolkit::waitCloseSemaphore;

ChaosCUToolkit::ChaosCUToolkit() {
    
    GlobalConfiguration::getInstance()->addOption<bool>(CU_OPT_IN_MEMORY_DATABASE,
                                                        "Specify when to use in memory or on disc contorl unit internal database",
                                                        true);
    //
    
    
    GlobalConfiguration::getInstance()->addOption<bool>(CONTROL_MANAGER_UNIT_SERVER_ENABLE,
                                                        CONTROL_MANAGER_UNIT_SERVER_ENABLE_desc,
                                                        true);
    
    GlobalConfiguration::getInstance()->addOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_ALIAS,
                                                               CONTROL_MANAGER_UNIT_SERVER_ALIAS_desc);
    
    GlobalConfiguration::getInstance()->addOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_KEY,
                                                               CONTROL_MANAGER_UNIT_SERVER_KEY_desc);
    
    
    GlobalConfiguration::getInstance()->addOption<uint64_t>(CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC,
                                                            CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_desc,
                                                            CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_DEFAULT);
    
    GlobalConfiguration::getInstance()->addOption< std::vector<std::string> >(CONTROL_MANAGER_EXECUTION_POOLS,
                                                                              CONTROL_MANAGER_EXECUTION_POOLS_DESC);
    
    GlobalConfiguration::getInstance()->addOption< double >(CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP,
                                                            CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DESC,
                                                            CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT);
    
    GlobalConfiguration::getInstance()->addOption< std::string >(EU_PLUGIN_DIRECTORY,
                                                                 EU_PLUGIN_DIRECTORY_DESC,
                                                                 EU_PLUGIN_DIRECTORY_DEFAULT);
    
    GlobalConfiguration::getInstance()->addOption< bool >(EU_PLUGIN_ENABLE,
                                                          EU_PLUGIN_ENABLE_DESC,
                                                          EU_PLUGIN_ENABLE_DEFAULT);
    
    GlobalConfiguration::getInstance()->addOption< bool >(UNIT_GATEWAY_ENABLE,
                                                          UNIT_GATEWAY_ENABLE_DESC,
                                                          UNIT_GATEWAY_ENABLE_DEFAULT);
    
    GlobalConfiguration::getInstance()->addOption< unsigned int >(CU_EG_OPT_WORKER_THREAD_NUMBER,
                                                                  CU_EG_OPT_WORKER_THREAD_NUMBER_DESC,
                                                                  CU_EG_OPT_WORKER_THREAD_NUMBER_DEFAULT);
    
    GlobalConfiguration::getInstance()->addOption< std::vector<std::string> >(CU_EG_OPT_WORKER_KV_PARAM,
                                                                              CU_EG_OPT_WORKER_KV_PARAM_DESC);
    
}

ChaosCUToolkit::~ChaosCUToolkit() {
    
}

void ChaosCUToolkit::closeUIToolkit() {
    signalHanlder(SIGTERM);
}

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosCUToolkit::init(int argc, char* argv[]) throw (CException) {
    ChaosCommon<ChaosCUToolkit>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosCUToolkit::init(istringstream &initStringStream) throw (CException) {
    ChaosCommon<ChaosCUToolkit>::init(initStringStream);
}

/*
 *
 */
void ChaosCUToolkit::init(void *init_data)  throw(CException) {
    try {
        ChaosCommon<ChaosCUToolkit>::init(init_data);
        LAPP_ << "Initializing !CHAOS Control Unit System";
        
        struct sigaction sigact;
        sigact.sa_sigaction = crit_err_hdlr;
        sigact.sa_flags = SA_RESTART | SA_SIGINFO;
        
        if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0) {
            LERR_ << "error setting signal handler for SIGSEGV";
        }
        
        if (signal((int) SIGINT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGINT Signal handler registration error";
        }
        
        if (signal((int) SIGQUIT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGQUIT Signal handler registration error";
        }
        
        if (signal((int) SIGTERM, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGTERM Signal handler registration error";
        }
        
        //        if (signal((int) SIGSEGV, ChaosCUToolkit::signalHanlder) == SIG_ERR){
        //            LERR_ << "SIGSEGV Signal handler registration error";
        //        }
        
        if (signal((int) SIGABRT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGABRT Signal handler registration error";
        }
        
        //force first allocation of metadata logging
        if(GlobalConfiguration::getInstance()->getMetadataServerAddressList().size()) {
            //we can initilize the logging manager
            InizializableService::initImplementation(chaos::common::metadata_logging::MetadataLoggingManager::getInstance(), NULL, "MetadataLoggingManager", __PRETTY_FUNCTION__);
        }
        
        
        //init healt manager singleton
        StartableService::initImplementation(HealtManager::getInstance(), NULL, "HealtManager", __PRETTY_FUNCTION__);
        
        StartableService::initImplementation(CommandManager::getInstance(), NULL, "CommandManager", "ChaosCUToolkit::init");
        CommandManager::getInstance()->server_handler=this;
        
        StartableService::initImplementation(DriverManager::getInstance(), NULL, "DriverManager", "ChaosCUToolkit::init");
        
        StartableService::initImplementation(DataManager::getInstance(), NULL, "DataManager", "ChaosCUToolkit::init");
        
        StartableService::initImplementation(ControlManager::getInstance(), NULL, "ControlManager", "ChaosCUToolkit::init");
        
        if(GlobalConfiguration::getInstance()->hasOption(UNIT_GATEWAY_ENABLE) &&
           GlobalConfiguration::getInstance()->getOption<bool>(UNIT_GATEWAY_ENABLE)) {
            //initilize unit gateway
            InizializableService::initImplementation(external_gateway::ExternalUnitGateway::getInstance(), NULL, "ExternalUnitGateway", __PRETTY_FUNCTION__);
        }
        
        LAPP_ << "Control Manager Initialized";
        
        LAPP_ << "!CHAOS Control Unit System Initialized";
        
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
    //start data manager
}

/*
 *
 */
void ChaosCUToolkit::start() throw(CException){
    try {
        ChaosCommon<ChaosCUToolkit>::start();
        
        LAPP_ << "Starting !CHAOS Control Unit System";
        
        //init healt manager singleton
        StartableService::startImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__);
        
        //start command manager, this manager must be the last to startup
        StartableService::startImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::start");
        
        //start driver manager
        StartableService::startImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::start");
        
        //start command manager, this manager must be the last to startup
        StartableService::startImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::start");
        
        //start Control Manager
        StartableService::startImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::start");
        
        LAPP_ << "-----------------------------------------";
        LAPP_ << "!CHAOS Control Unit System Started";
        LAPP_ << "RPC Server address: " << CommandManager::getInstance()->broker->getRPCUrl();
        LAPP_ << "DirectIO Server address: " << CommandManager::getInstance()->broker->getDirectIOUrl();
        LAPP_ << "-----------------------------------------";
        //at this point i must with for end signal
        waitCloseSemaphore.wait();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
    //execute the stop and the deinitialization of the toolkit
    stop();
    deinit();
}

/*
 Stop the toolkit execution
 */
void ChaosCUToolkit::stop() throw(CException) {
    LAPP_ << "Stopping !CHAOS Control Unit System";
    //stop command manager, this manager must be the last to startup
    CHAOS_NOT_THROW(StartableService::stopImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::stop"););
    
    //stop control manager
    CHAOS_NOT_THROW(StartableService::stopImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::stop"););
    
    //stop command manager, this manager must be the last to startup
    CHAOS_NOT_THROW(StartableService::stopImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::stop"););
    
    //stop driver manager
    CHAOS_NOT_THROW(StartableService::stopImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::stop"););
    
    //init healt manager singleton
    CHAOS_NOT_THROW(StartableService::stopImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
    
    ChaosCommon<ChaosCUToolkit>::stop();
}

/*
 Deiniti all the manager
 */
void ChaosCUToolkit::deinit() throw(CException) {
    LAPP_ << "Deinitilizzating !CHAOS Control Unit System";
    if(GlobalConfiguration::getInstance()->hasOption(UNIT_GATEWAY_ENABLE) &&
       GlobalConfiguration::getInstance()->getOption<bool>(UNIT_GATEWAY_ENABLE)) {
        //initilize unit gateway
        InizializableService::deinitImplementation(external_gateway::ExternalUnitGateway::getInstance(), "ExternalUnitGateway", __PRETTY_FUNCTION__);
    }
    //deinit command manager, this manager must be the last to startup
    CHAOS_NOT_THROW(StartableService::deinitImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::stop"););
    
    //deinit control manager
    CHAOS_NOT_THROW(StartableService::deinitImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::stop"););
    
    //deinit data manager
    CHAOS_NOT_THROW(StartableService::deinitImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::deinit"););
    
    //deinit metadata logging manager
    CHAOS_NOT_THROW(StartableService::deinitImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::deinit"););
    
    CHAOS_NOT_THROW(InizializableService::deinitImplementation(MetadataLoggingManager::getInstance(), "MetadataLoggingManager", __PRETTY_FUNCTION__););
    LAPP_ << "!CHAOS Control Unit System Stopped";
    
    //deinit healt manager singleton
    CHAOS_NOT_THROW(StartableService::deinitImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
    
    
    //forward the deinitialization to the common sublayer
    ChaosCommon<ChaosCUToolkit>::deinit();
    
    LAPP_ << "-----------------------------------------";
    LAPP_ << "!CHAOS Control Unit System deinitialized  ";
    LAPP_ << "-----------------------------------------";
}

/*
 * Add a new Control Unit Class for execution
 */
void ChaosCUToolkit::setProxyCreationHandler(ProxyLoadHandler load_handler) {
    //call command manager to submit Custom Control Unit
    return ControlManager::getInstance()->setProxyCreationHandler(load_handler);
}

/*
 *
 */
void ChaosCUToolkit::signalHanlder(int signalNumber) {
    //lock lk(monitor);
    //unlock the condition for end start method
    //endWaithCondition.notify_one();
    //waitCloseSemaphore.unlock();
    if((signalNumber==SIGABRT) || (signalNumber==SIGSEGV)){
        LAPP_ << "INTERNAL ERROR, please provide log, Catch SIGNAL: "<< signalNumber;
        
        throw CFatalException(signalNumber,"trapped signal",__PRETTY_FUNCTION__);
    } else {
        LAPP_ << "CATCH SIGNAL "<< signalNumber;
        if(signalNumber == SIGINT || signalNumber == SIGTERM){
            waitCloseSemaphore.unlock();
        } else {
            LAPP_ << "EXITING THROUGH SIGNAL '"<<signalNumber<<"' ....";
            exit(0);
        }
    }
}
