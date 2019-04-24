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

#include <fstream>
#include <iostream>

#include <chaos/common/log/LogManager.h>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "GlobalConfiguration.h"
#include <chaos/common/version.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::utility;

using namespace boost;
namespace po = boost::program_options;
namespace ext_unt = chaos::common::external_unit;

#define _RPC_PORT					8888
#define _SYNC_RPC_PORT				8080
#define _DIRECT_IO_PRIORITY_PORT	1672
#define _DIRECT_IO_SERVICE_PORT		30175

GlobalConfiguration::GlobalConfiguration():
desc("!CHAOS Framework Allowed options"){}
GlobalConfiguration::~GlobalConfiguration(){}

void GlobalConfiguration::preParseStartupParameters()  {
    try{
        addOption(InitOption::OPT_HELP, "Produce help message");
        addOption<std::string>(InitOption::OPT_CONF_FILE,"File configuration path");
        addOption(InitOption::OPT_VERSION, "Printout version");
        
        addOption(InitOption::OPT_LOG_ON_CONSOLE, po::value< bool >()->zero_tokens(), "Specify when the log must be forwarded on console");
        addOption(InitOption::OPT_LOG_ON_SYSLOG, po::value< bool >()->zero_tokens(), "Specify when the log must be forwarded on syslog server");
        addOption(InitOption::OPT_LOG_SYSLOG_SERVER, po::value< string >()->default_value("localhost"), "Specify the logsrv hostname");
        addOption(InitOption::OPT_LOG_SYSLOG_SERVER_PORT, po::value< uint32_t >()->default_value(514), "Specify the logsrv port");
        addOption(InitOption::OPT_LOG_ON_FILE, po::value< bool >()->zero_tokens(), "Specify when the log must be forwarded on file");
        addOption(InitOption::OPT_LOG_FILE, po::value< string >()->default_value("chaos_framework_log_%Y-%m-%d_%H-%M-%S.%N.log"), "Specify when the file path of the log");
        addOption(InitOption::OPT_LOG_LEVEL, po::value< string >()->default_value("info"), "Specify the level of the log using the value [debug, info, notice, warning, fatal]");
        addOption(InitOption::OPT_LOG_MAX_SIZE_MB, po::value< uint32_t >()->default_value(10), "Specify the max size in megabytes fo the file log");
        addOption(InitOption::OPT_LOG_METRIC_ON_CONSOLE, po::value< bool >()->zero_tokens(), "Enable the logging metric on console");
        addOption(InitOption::OPT_LOG_METRIC_ON_FILE, po::value< bool >()->zero_tokens(), "Enable the logging metric on file");
        addOption(InitOption::OPT_LOG_METRIC_ON_FILE_PATH, po::value< string >()->default_value(boost::filesystem::current_path().string()), "Specify the path of metric logs");
        addOption(InitOption::OPT_METADATASERVER_ADDRESS, po::value< std::vector< std::string > >(), "Metadataserver server:port address");
        addOption(InitOption::OPT_DATA_IO_IMPL, po::value< string >()->default_value("IODirect"), "Specify the data io implementation");
        addOption(InitOption::OPT_DIRECT_IO_IMPLEMENTATION, po::value< string >()->default_value("ZMQ"), "Specify the direct io implementation");
        addOption(InitOption::OPT_DIRECT_IO_PRIORITY_SERVER_PORT, po::value<uint32_t>()->default_value(_DIRECT_IO_PRIORITY_PORT), "DirectIO priority server port");
        addOption(InitOption::OPT_DIRECT_IO_SERVICE_SERVER_PORT, po::value<uint32_t>()->default_value(_DIRECT_IO_SERVICE_PORT), "DirectIO service server port");
        addOption(InitOption::OPT_DIRECT_IO_SERVER_THREAD_NUMBER, po::value<uint32_t>()->default_value(1),"DirectIO server thread number");
        addOption(InitOption::OPT_DIRECT_IO_SERVER_IMPL_KV_PARAM, po::value< std::vector<std::string> >(),"DirectIO implementation key value parameters[k:v]");
        addOption(InitOption::OPT_DIRECT_IO_CLIENT_IMPL_KV_PARAM, po::value< std::vector<std::string> >(),"DirectIO implementation key value parameters[k:v]");
        addOption(InitOption::OPT_DIRECT_IO_LOG_METRIC, po::value< bool >()->zero_tokens(), "Enable the logging of the DirectIO metric");
        addOption(InitOption::OPT_DIRECT_IO_LOG_METRIC_UPDATE_INTERVAL, po::value< uint64_t >()->default_value(5), "The time interval between metric samples");
        addOption(InitOption::OPT_DIRECT_IO_CLIENT_LOG_METRIC_MERGED_ENDPOINT, po::value< bool >()->default_value(true), "Merge the metric values(of all endpoint) together");
        addOption(InitOption::OPT_RPC_SYNC_ENABLE, po::value< bool >()->default_value(false), "Enable the sync wrapper to rpc protocol");
        addOption(InitOption::OPT_RPC_LOG_METRIC, po::value< bool >()->zero_tokens(), "Enable the logging of the mrpc metric");
        addOption(InitOption::OPT_RPC_LOG_METRIC_UPDATE_INTERVAL, po::value< uint64_t >()->default_value(5), "The time intervall between metric samples");
        addOption(InitOption::OPT_RPC_IMPLEMENTATION, po::value< string >()->default_value("ZMQ"), "Specify the rpc implementation");
        addOption(InitOption::OPT_RPC_SERVER_PORT, po::value<uint32_t>()->default_value(_RPC_PORT), "RPC server port");
        addOption(InitOption::OPT_RPC_SERVER_THREAD_NUMBER, po::value<uint32_t>()->default_value(2),"RPC server thread number");
        addOption(InitOption::OPT_RPC_IMPL_KV_PARAM, po::value< std::vector<std::string> >(),"RPC implementation key value parameter[k:v]");
        addOption(InitOption::OPT_RPC_DOMAIN_QUEUE_THREAD, po::value<uint32_t>()->default_value(1),"RPC domain scheduler queue's thread consumer number");
        addOption(InitOption::OPT_RPC_DOMAIN_SCHEDULER_TYPE, po::value<uint32_t>()->default_value(0),"RPC domain scheduler type[0-default, 1-shared]");
        addOption(InitOption::OPT_EVENT_DISABLE, po::value< bool >()->default_value(false), "Disable the event system [by default it is enable]");
        addOption(InitOption::OPT_PUBLISHING_IP, po::value< string >(), "Specify the ip address where to publish the framework rpc system");
        addOption(InitOption::OPT_PUBLISHING_INTERFACE, po::value< string >(), "Specify the interface where to publish the framework rpc system");
        addOption(InitOption::OPT_TIME_CALIBRATION, po::value< bool >()->zero_tokens(), "Enable the time calibration for chaos process");
        addOption(InitOption::OPT_TIME_CALIBRATION_OFFSET_BOUND, po::value< uint32_t >()->default_value(500), "The number of millisecond of difference after wich the calibration will be activated");
        addOption(InitOption::OPT_TIME_CALIBRATION_NTP_SERVER, po::value< string >(), "Specify the NTP server used for time calibration");
        
        addOption(InitOption::OPT_PLUGIN_ENABLE, po::value< bool >()->zero_tokens(), "Enable the use of the plugin");
        addOption(InitOption::OPT_PLUGIN_DIRECTORY_PATH, po::value< std::string >()->default_value("."), "Specify the directory where are stored the plugin");
        
        addOption(InitOption::OPT_SCRIPT_VM_KV_PARAM, po::value< std::vector<std::string> >(),"Script virtual machine key value parameter [k:v]");

        addOption(InitOption::OPT_REST_POLL_TIME_US, po::value< uint32_t >()->default_value(10),"Rest poll time in us (less means more responsive, but more cpu intensive)");

        addOption(ext_unt::InitOption::OPT_UNIT_GATEWAY_ENABLE, po::value< bool >()->zero_tokens(), ext_unt::InitOption::OPT_UNIT_GATEWAY_ENABLE_DESC);
        addOption(ext_unt::InitOption::OPT_UNIT_GATEWAY_WORKER_THREAD_NUMBER, po::value< uint32_t >()->default_value(ext_unt::InitOption::OPT_UNIT_GATEWAY_WORKER_THREAD_NUMBER_DEFAULT), ext_unt::InitOption::OPT_UNIT_GATEWAY_WORKER_THREAD_NUMBER_DESC);
        addOption(ext_unt::InitOption::OPT_UNIT_GATEWAY_ADAPTER_KV_PARAM, po::value< std::vector<std::string> >(), ext_unt::InitOption::OPT_UNIT_GATEWAY_ADAPTER_KV_PARAM_DESC);
    } catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::preParseStartupParameters");
    }
}

void GlobalConfiguration::parseStartupParametersAllowingUnregistered(int argc, const char* argv[])  {
    parseParameter(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run());
}

void GlobalConfiguration::parseStartupParameters(int argc, const char* argv[])  {
    parseParameter(po::parse_command_line(argc, argv, desc));
}

void GlobalConfiguration::parseStringStream(std::istream &sStreamOptions)  {
    parseParameter(po::parse_config_file(sStreamOptions, desc));
}

int32_t GlobalConfiguration::filterLogLevel(string& levelStr)  {
    chaos::common::log::level::LogSeverityLevel level = chaos::common::log::level::LSLInfo;
    
    if (levelStr == "info")
        level = chaos::common::log::level::LSLInfo;
    else if (levelStr == "debug")
        level = chaos::common::log::level::LSLDebug;
    else if (levelStr == "notice")
        level = chaos::common::log::level::LSLNotice;
    else if (levelStr == "warning")
        level = chaos::common::log::level::LSLWarning;
    else if (levelStr == "fatal")
        level = chaos::common::log::level::LSLFatal;
    else
        throw chaos::CException(1, "Invalid log level", "GlobalConfiguration::filterLogLevel");
    
    return static_cast< int32_t >(level);
}

void GlobalConfiguration::loadStartupParameter(int argc, const char* argv[])  {
    try{
        //
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }catch (po::error &e) {
        //write error also on cerr
        std::cerr << e.what();
        throw CException(0, e.what(), __PRETTY_FUNCTION__);
    }
}

void GlobalConfiguration::loadStreamParameter(std::istream &config_file)   {
    try{
        //
        po::store(po::parse_config_file(config_file, desc), vm);
        po::notify(vm);
    }catch (po::error &e) {
        //write error also on cerr
        std::cerr << e.what();
        throw CException(0, e.what(), __PRETTY_FUNCTION__);
        
    }
}

void GlobalConfiguration::scanOption()   {
    try {
        if (hasOption(InitOption::OPT_HELP)) {
            std::cout << desc;
            exit(0);
            return;
        }
        if (hasOption(InitOption::OPT_VERSION)) {
            std::cout <<"Version:"<< CSLIB_VERSION_MAJOR<<"."<<CSLIB_VERSION_MINOR<<"."<<CSLIB_VERSION_NUMBER<< " BuildID:"<<CSLIB_BUILD_ID<< " BuildDate:"<<__DATE__ <<" " <<__TIME__<<"\n";
            exit(0);
            return;
            
        }
    } catch (po::error &e) {
        //write error also on cerr
        std::cerr << e.what();
        throw CException(0, e.what(), __PRETTY_FUNCTION__);
        
    }
    
    
}

void GlobalConfiguration::parseParameter(const po::basic_parsed_options<char>& optionsParser) {
    //int rpcServerPort;
    //int rpcServerThreadNumber;
    //string metadataServerAddress;
    //vector<string> liveDataServer;
    try{
        //
        po::store(optionsParser, vm);
        po::notify(vm);
    }catch (po::error &e) {
        //write error also on cerr
        std::cerr << e.what();
        throw CException(0, e.what(), "GlobalConfiguration::preParseStartupParameters");
        
    }
    
    //scan option
    scanOption();
    
    //check the default option
    checkDefaultOption();
}
void GlobalConfiguration::checkDefaultOption()  {
    configuration.reset(new CDataWrapper());
    //now we can fill the gloabl configuration
    //start with getting log configuration
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(logOnConsole, InitOption::OPT_LOG_ON_CONSOLE);
    configuration->addBoolValue(InitOption::OPT_LOG_ON_CONSOLE, logOnConsole);
    
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(logOnSyslog, InitOption::OPT_LOG_ON_SYSLOG);
    configuration->addBoolValue(InitOption::OPT_LOG_ON_SYSLOG, logOnSyslog);
    
    CHECK_AND_DEFINE_OPTION(string, logSyslogSrv, InitOption::OPT_LOG_SYSLOG_SERVER);
    configuration->addStringValue(InitOption::OPT_LOG_SYSLOG_SERVER, logSyslogSrv);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, logSyslogSrvPort, InitOption::OPT_LOG_SYSLOG_SERVER_PORT, 0);
    configuration->addInt32Value(InitOption::OPT_LOG_SYSLOG_SERVER_PORT, logSyslogSrvPort);
    
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(logOnFile, InitOption::OPT_LOG_ON_FILE);
    configuration->addBoolValue(InitOption::OPT_LOG_ON_FILE, logOnFile);
    
    CHECK_AND_DEFINE_OPTION(string, logFilePath, InitOption::OPT_LOG_FILE);
    configuration->addStringValue(InitOption::OPT_LOG_FILE, logFilePath);
    
    CHECK_AND_DEFINE_OPTION(string, logLevel, InitOption::OPT_LOG_LEVEL)
    configuration->addInt32Value(InitOption::OPT_LOG_LEVEL, filterLogLevel(logLevel));
    
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(log_metric_on_console, InitOption::OPT_LOG_METRIC_ON_CONSOLE);
    configuration->addBoolValue(InitOption::OPT_LOG_METRIC_ON_CONSOLE, log_metric_on_console);
    
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(log_metric_on_file, InitOption::OPT_LOG_METRIC_ON_FILE)
    configuration->addBoolValue(InitOption::OPT_LOG_METRIC_ON_FILE, log_metric_on_file);
    
    CHECK_AND_DEFINE_OPTION(string, log_metric_file_path, InitOption::OPT_LOG_METRIC_ON_FILE_PATH);
    configuration->addStringValue(InitOption::OPT_LOG_METRIC_ON_FILE_PATH, log_metric_file_path);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, log_max_size_mb, InitOption::OPT_LOG_MAX_SIZE_MB, 10);
    configuration->addInt32Value(InitOption::OPT_LOG_MAX_SIZE_MB, log_max_size_mb);
    
    CHECK_AND_DEFINE_OPTION(string, publishingIp, InitOption::OPT_PUBLISHING_IP);
    if(publishingIp.size()&&InetUtility::checkWellFormedHostPort(publishingIp)){configuration->addStringValue(InitOption::OPT_PUBLISHING_IP, publishingIp);}
    
    CHECK_AND_DEFINE_OPTION(string, publishingInterface, InitOption::OPT_PUBLISHING_INTERFACE)
    configuration->addStringValue(InitOption::OPT_PUBLISHING_INTERFACE, publishingInterface);
    
    //configure rpc
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, rpcServerPort, InitOption::OPT_RPC_SERVER_PORT, 8888);
    int32_t freeFoundPort = InetUtility::scanForLocalFreePort(rpcServerPort);
    addLocalServerBasePort(freeFoundPort);
    configuration->addInt32Value(InitOption::OPT_RPC_SERVER_PORT, freeFoundPort);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, rpcServerThreadNumber, InitOption::OPT_RPC_SERVER_THREAD_NUMBER, 1);
    configuration->addInt32Value(InitOption::OPT_RPC_SERVER_THREAD_NUMBER, rpcServerThreadNumber);
    
    CHECK_AND_DEFINE_OPTION(string, rpcImpl, InitOption::OPT_RPC_IMPLEMENTATION)
    configuration->addStringValue(InitOption::OPT_RPC_IMPLEMENTATION, rpcImpl);
    
    CHECK_AND_DEFINE_OPTION(bool, OPT_RPC_SYNC_ENABLE, InitOption::OPT_RPC_SYNC_ENABLE)
    else{
        OPT_RPC_SYNC_ENABLE = false;
    }
    configuration->addBoolValue(InitOption::OPT_RPC_SYNC_ENABLE, OPT_RPC_SYNC_ENABLE);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(bool, rpc_enable_log_metric, InitOption::OPT_RPC_LOG_METRIC, false);
    configuration->addBoolValue(InitOption::OPT_RPC_LOG_METRIC, rpc_enable_log_metric);;
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint64_t, rpc_enable_log_metric_update_sec, InitOption::OPT_RPC_LOG_METRIC_UPDATE_INTERVAL, 5);
    configuration->addInt64Value(InitOption::OPT_RPC_LOG_METRIC_UPDATE_INTERVAL, rpc_enable_log_metric_update_sec);
    
    CHECK_AND_DEFINE_OPTION(std::vector<std::string>, rpc_impl_kv_param, InitOption::OPT_RPC_IMPL_KV_PARAM);
    
    //fill the key value list
    if(rpc_impl_kv_param.size()) {
        fillKVParameter(map_kv_param_rpc_impl, rpc_impl_kv_param, "");
    }
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, rpc_domain_queue_thread_number, InitOption::OPT_RPC_DOMAIN_QUEUE_THREAD, 1);
    configuration->addInt32Value(InitOption::OPT_RPC_DOMAIN_QUEUE_THREAD, rpc_domain_queue_thread_number);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, rpc_domain_scheduler_type, InitOption::OPT_RPC_DOMAIN_SCHEDULER_TYPE, 0);
    configuration->addInt32Value(InitOption::OPT_RPC_DOMAIN_SCHEDULER_TYPE, rpc_domain_scheduler_type);
    
    //direct io
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, direct_io_server_thread_number, InitOption::OPT_DIRECT_IO_SERVER_THREAD_NUMBER, 2);
    configuration->addInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_SERVER_THREAD_NUMBER, direct_io_server_thread_number);
    
    CHECK_AND_DEFINE_OPTION(std::vector<std::string>, directio_srv_impl_kv_param, InitOption::OPT_DIRECT_IO_SERVER_IMPL_KV_PARAM);
    //fill the key value list
    if(directio_srv_impl_kv_param.size()) {
        fillKVParameter(map_kv_param_directio_srv_impl, directio_srv_impl_kv_param, "");
    }
    
    CHECK_AND_DEFINE_OPTION(std::vector<std::string>, directio_clnt_impl_kv_param, InitOption::OPT_DIRECT_IO_CLIENT_IMPL_KV_PARAM);
    //fill the key value list
    if(directio_clnt_impl_kv_param.size()) {
        fillKVParameter(map_kv_param_directio_clnt_impl, directio_clnt_impl_kv_param, "");
    }
    CHECK_AND_DEFINE_OPTION(string, direct_io_server_impl, InitOption::OPT_DIRECT_IO_IMPLEMENTATION)
    configuration->addStringValue(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_IMPL_TYPE, direct_io_server_impl);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, direct_io_priority_port, InitOption::OPT_DIRECT_IO_PRIORITY_SERVER_PORT, _DIRECT_IO_PRIORITY_PORT);
    freeFoundPort = InetUtility::scanForLocalFreePort(direct_io_priority_port);
    configuration->addInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_PRIORITY_PORT, (uint32_t)freeFoundPort);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, direct_io_service_port, InitOption::OPT_DIRECT_IO_SERVICE_SERVER_PORT, _DIRECT_IO_SERVICE_PORT);
    freeFoundPort = InetUtility::scanForLocalFreePort(direct_io_service_port);
    configuration->addInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_SERVICE_PORT, (uint32_t)freeFoundPort);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(bool, direct_io_enable_log_metric, InitOption::OPT_DIRECT_IO_LOG_METRIC, false);
    configuration->addBoolValue(InitOption::OPT_DIRECT_IO_LOG_METRIC, direct_io_enable_log_metric);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint64_t, direct_io_enable_log_metric_update_sec, InitOption::OPT_DIRECT_IO_LOG_METRIC_UPDATE_INTERVAL, 5);
    configuration->addInt64Value(InitOption::OPT_DIRECT_IO_LOG_METRIC_UPDATE_INTERVAL, direct_io_enable_log_metric_update_sec);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(bool, direct_io_enable_log_metric_merged_endpoint, InitOption::OPT_DIRECT_IO_CLIENT_LOG_METRIC_MERGED_ENDPOINT, true)
    configuration->addBoolValue(InitOption::OPT_DIRECT_IO_CLIENT_LOG_METRIC_MERGED_ENDPOINT, direct_io_enable_log_metric_merged_endpoint);
    
    //event
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(event_disable, InitOption::OPT_EVENT_DISABLE);
    configuration->addBoolValue(InitOption::OPT_EVENT_DISABLE, event_disable);
    
    configuration->addStringValue(chaos::common::event::EventConfiguration::OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION, "AsioImpl");
    
    //configure metadataserver as single or list
    CHECK_AND_DEFINE_OPTION(std::vector<std::string>, metadata_server_address_list, InitOption::OPT_METADATASERVER_ADDRESS);
    if(metadata_server_address_list.size()) {
        for(std::vector<std::string>::iterator it = metadata_server_address_list.begin();
            it != metadata_server_address_list.end();
            it++) {
            addMetadataServerAddress(*it);
        }
    } else {
        addMetadataServerAddress("localhost:5000");
    }
    finalizeMetadataServerAddress();
    
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(enable_time_calibration, InitOption::OPT_TIME_CALIBRATION);
    configuration->addBoolValue(InitOption::OPT_TIME_CALIBRATION, enable_time_calibration);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(uint32_t, offset_calibration_bound, InitOption::OPT_TIME_CALIBRATION_OFFSET_BOUND, 500);
    configuration->addInt32Value(InitOption::OPT_TIME_CALIBRATION_OFFSET_BOUND, offset_calibration_bound);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(std::string, time_calibration_ntp_server, InitOption::OPT_TIME_CALIBRATION_NTP_SERVER, "");
    configuration->addStringValue(InitOption::OPT_TIME_CALIBRATION_NTP_SERVER, time_calibration_ntp_server);
    
    CHECK_AND_DEFINE_OPTION(std::vector<std::string>, script_vm_kv_param, InitOption::OPT_SCRIPT_VM_KV_PARAM);
    //fill the key value list
    if(script_vm_kv_param.size()) {
        fillKVParameter(map_kv_param_script_vm, script_vm_kv_param, "");
    }
}


/*
 Add a custom option
 */
void GlobalConfiguration::addOption(const char* name,
                                    const char* description) {
    try{
        desc.add_options()(name, description);
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::addOption");
    }
}

/*
 Add a custom option
 */
void GlobalConfiguration::addOption(const char* name,
                                    const po::value_semantic* s,
                                    const char* description) {
    try {
        desc.add_options()(name, s, description);
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::addOption");
    }
}

/*
 Add a custom option
 */
void GlobalConfiguration::addOptionZeroTokens(const char* name,
                                              const char* description,
                                              bool *default_variable)   {
    try{
        addOption(name, po::value< bool >(default_variable)->zero_tokens(), description);
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::addOptionZeroTokens");
    }
}

void GlobalConfiguration::fillKVParameter(std::map<std::string, std::string>& kvmap,
                                          const std::vector<std::string>& kv_vector,
                                          const std::string& regex) {
    //no cache server provided
    //clear previosly pair
    std::vector<std::string> kv_splitted;
    for(std::vector<std::string>::const_iterator it = kv_vector.begin(),
        end = kv_vector.end();
        it != end;
        it++) {
        
        const std::string& kv_param_value = *it;
        
        if(regex.size() &&
           !boost::regex_match(kv_param_value,
                               boost::regex(regex))) {
               throw chaos::CException(-3, "Malformed kv parameter string", __PRETTY_FUNCTION__);
           }
        
        //clear previosly pair
        kv_splitted.clear();
        
        //get new pair
        boost::algorithm::split(kv_splitted,
                                kv_param_value,
                                boost::algorithm::is_any_of(":"),
                                boost::algorithm::token_compress_on);
        // add key/value pair
        kvmap.insert(make_pair(kv_splitted[0], kv_splitted[1]));
    }
}

/**
 *return the cdatawrapper that contains the global configuraiton
 */
chaos_data::CDataWrapper *GlobalConfiguration::getConfiguration(){
    return configuration.get();
}

void GlobalConfiguration::setConfiguration(chaos_data::CDataWrapper *conf){
    configuration->copyAllTo(*conf);
}

/**
 *Add the metadataserver address
 */
void GlobalConfiguration::addMetadataServerAddress(const string& mdsAddress)  {
    bool isHostnameAndPort = InetUtility::checkWellFormedHostNamePort(mdsAddress);
    bool isIpAndPort  = InetUtility::checkWellFormedHostIpPort(mdsAddress);
    if(!isHostnameAndPort && !isIpAndPort){
        std::stringstream ss;
        ss<< "Bad server address: '"<<mdsAddress<<"' expected ip:port or hostaddress:port";
        throw CException(1, ss.str(), "GlobalConfiguration::addMetadataServerAddress");
    }
    //address can be added
    configuration->appendStringToArray(mdsAddress);
}

void GlobalConfiguration::finalizeMetadataServerAddress() {
    configuration->finalizeArrayForKey(InitOption::OPT_METADATASERVER_ADDRESS);
}

/**
 *Add the metadataserver address
 */
void GlobalConfiguration::addLocalServerAddress(const std::string& mdsAddress)  {
    bool isIp = InetUtility::checkWellFormedHostPort(mdsAddress);
    if(!isIp){
        std::stringstream ss;
        ss<<"Bad server address:'"<<mdsAddress<<"'";
        throw CException(1, ss.str(), "GlobalConfiguration::addMetadataServerAddress");
    }
    //address can be added
    configuration->addStringValue("local_ip", mdsAddress);
}

/**
 *Add the metadataserver address
 */
void GlobalConfiguration::addLocalServerBasePort(int32_t localDefaultPort)  {
    configuration->addInt32Value("base_port", localDefaultPort);
}

std::string GlobalConfiguration::getHostname() {
    return InetUtility::getHostname();
}

/*
 return the address of metadataserver
 */
string GlobalConfiguration::getMetadataServerAddress() {
    CMultiTypeDataArrayWrapperSPtr server_array = configuration->getVectorValue(InitOption::OPT_METADATASERVER_ADDRESS);
    CHAOS_ASSERT(server_array->size());
    return server_array->getStringElementAtIndex(0);
}

VectorMetadatserver GlobalConfiguration::getMetadataServerAddressList() {
    std::vector<CNetworkAddress> result;
    CMultiTypeDataArrayWrapperSPtr server_array = configuration->getVectorValue(InitOption::OPT_METADATASERVER_ADDRESS);
    for(int idx = 0;
        idx < server_array->size();
        idx++) {
        result.push_back(CNetworkAddress(server_array->getStringElementAtIndex(idx)));
    }
    return result;
}

/*
 return the address of metadataserver
 */
string GlobalConfiguration::getLocalServerAddress() {
    return configuration->getStringValue("local_ip");
}

/*
 return the address of metadataserver
 */
int32_t GlobalConfiguration::getLocalServerBasePort() {
    return configuration->getInt32Value("base_port");
}

string GlobalConfiguration::getLocalServerAddressAnBasePort(){
    char buf[128];
    string addr = configuration->getStringValue("local_ip");
    sprintf ( buf, "%s:%d", addr.c_str(), (int)configuration->getInt32Value("base_port"));
    addr.assign(buf);
    return addr;
}

/*
 return the address of metadataserver
 */
bool GlobalConfiguration::isMEtadataServerConfigured() {
    return configuration->hasKey(InitOption::OPT_METADATASERVER_ADDRESS);
}

MapStrKeyStrValue& GlobalConfiguration::getRpcImplKVParam() {
    return map_kv_param_rpc_impl;
}

MapStrKeyStrValue& GlobalConfiguration::getDirectIOServerImplKVParam() {
    return map_kv_param_directio_srv_impl;
}

MapStrKeyStrValue& GlobalConfiguration::getDirectIOClientImplKVParam() {
    return map_kv_param_directio_clnt_impl;
}

MapStrKeyStrValue& GlobalConfiguration::getScriptVMKVParam() {
    return map_kv_param_script_vm;
}
