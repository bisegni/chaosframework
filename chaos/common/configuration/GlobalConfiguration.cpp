/*
 *	GlobalConfiguration.cpp
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
#include <fstream>
#include <iostream>

#include <chaos/common/log/LogManager.h>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "GlobalConfiguration.h"

using namespace chaos;
using namespace boost;
namespace po = boost::program_options;

#define _RPC_PORT					8888
#define _DIRECT_IO_PRIORITY_PORT	1672
#define _DIRECT_IO_SERVICE_PORT		30175

/*
 
 */
void GlobalConfiguration::preParseStartupParameters() throw (CException){
    
    try{
        addOption(InitOption::OPT_HELP, "Produce help message");
        //cache parameter
        addOption<std::string>(InitOption::OPT_CONF_FILE,"File configuration path");
        addOption(InitOption::OPT_DATA_IO_IMPL, po::value< string >()->default_value("IODirect"), "Specify the data io implementation");
        addOption(InitOption::OPT_DIRECT_IO_IMPLEMENTATION, po::value< string >()->default_value("ZMQ"), "Specify the direct io implementation");
        addOption(InitOption::OPT_DIRECT_IO_PRIORITY_SERVER_PORT, po::value<int>()->default_value(_DIRECT_IO_PRIORITY_PORT), "DirectIO priority server port");
        addOption(InitOption::OPT_DIRECT_IO_SERVICE_SERVER_PORT, po::value<int>()->default_value(_DIRECT_IO_SERVICE_PORT), "DirectIO service server port");
        addOption(InitOption::OPT_DIRECT_IO_SERVER_THREAD_NUMBER, po::value<int>()->default_value(2),"DirectIO server thread number");
        addOption(InitOption::OPT_RPC_SYNC_ENABLE, po::value< bool >()->default_value(true), "Enable the sync wrapper to rpc protocol");
        addOption(InitOption::OPT_RPC_SYNC_IMPLEMENTATION, po::value< string >()->default_value("HTTP"), "Specify the synchronous rpc implementation");
        addOption(InitOption::OPT_RPC_IMPLEMENTATION, po::value< string >()->default_value("ZMQ"), "Specify the rpc implementation");
        addOption(InitOption::OPT_RPC_SERVER_PORT, po::value<int>()->default_value(_RPC_PORT), "RPC server port");
        addOption(InitOption::OPT_RPC_SERVER_THREAD_NUMBER, po::value<int>()->default_value(2),"RPC server thread number");
        addOption(InitOption::OPT_RPC_IMPL_KV_PARAM, po::value<string>(),"RPC implementation key value parameter[k|v-k1|v1]");
        addOption(InitOption::OPT_LIVE_DATA_SERVER_ADDRESS, po::value< vector<string> >()->multitoken(), "Live server:port address");
        addOption(InitOption::OPT_METADATASERVER_ADDRESS, po::value< string >()->default_value("localhost:5000"), "Metadataserver server:port address");
        addOption(InitOption::OPT_LOG_ON_CONSOLE, po::value< bool >()->zero_tokens(), "Specify when the log must be forwarded on console");
        addOption(InitOption::OPT_LOG_ON_FILE, po::value< bool >()->zero_tokens(), "Specify when the log must be forwarded on file");
        addOption(InitOption::OPT_LOG_FILE, po::value< string >()->default_value("chaos_frameowrk.log"), "Specify when the file path of the log");
        addOption(InitOption::OPT_LOG_LEVEL, po::value< string >()->default_value("info"), "Specify the level of the log using the value [debug, info, notice, warning, fatal]"),
        addOption(InitOption::OPT_PUBLISHING_IP, po::value< string >(), "Specify the ip address where to publish the framework rpc system");
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::preParseStartupParameters");
    }
}

/*!
 Specialized option for startup c and cpp program main options parameter
 */
void GlobalConfiguration::parseStartupParameters(int argc, char* argv[]) throw (CException) {
    parseParameter(po::parse_command_line(argc, argv, desc));
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void GlobalConfiguration::parseStringStream(std::istream &sStreamOptions) throw (CException) {
    parseParameter(po::parse_config_file(sStreamOptions, desc));
}

//!conver user input into enumeration
/*
 Conver the string into enumeration for the log level
 */
int32_t GlobalConfiguration::filterLogLevel(string& levelStr) throw (CException) {
    chaos::log::level::LogSeverityLevel level = chaos::log::level::LSLInfo;
    
    if (levelStr == "info")
        level = chaos::log::level::LSLInfo;
    else if (levelStr == "debug")
        level = chaos::log::level::LSLDebug;
    else if (levelStr == "notice")
        level = chaos::log::level::LSLNotice;
    else if (levelStr == "warning")
        level = chaos::log::level::LSLWarning;
    else if (levelStr == "fatal")
        level = chaos::log::level::LSLFatal;
    else
        throw chaos::CException(1, "Invalid log level", "GlobalConfiguration::filterLogLevel");
    
    return static_cast< int32_t >(level);
}

void GlobalConfiguration::loadStartupParameter(int argc, char* argv[]) throw (CException) {
    try{
        //
        po::store(po::parse_command_line(argc, argv, desc), vm);
    }catch (po::error &e) {
        //write error also on cerr
        std::cerr << e.what();
        throw CException(0, e.what(), __PRETTY_FUNCTION__);
        
    }
}

void GlobalConfiguration::loadStreamParameter(std::istream &config_file)  throw (CException) {
    try{
        //
        po::store(po::parse_config_file(config_file, desc), vm);
    }catch (po::error &e) {
        //write error also on cerr
        std::cerr << e.what();
        throw CException(0, e.what(), __PRETTY_FUNCTION__);
        
    }
}

void GlobalConfiguration::scanOption()  throw (CException) {
    try{
        po::notify(vm);
    }catch (po::error &e) {
        //write error also on cerr
        std::cerr << e.what();
        throw CException(0, e.what(), __PRETTY_FUNCTION__);
        
    }
    
    if (hasOption(InitOption::OPT_HELP)) {
        std::cout << desc;
        exit(0);
        return;
        
    }
}

/*
 parse the tandard startup parameters
 */
void GlobalConfiguration::parseParameter(const po::basic_parsed_options<char>& optionsParser) throw (CException){
    //int rpcServerPort;
    //int rpcServerThreadNumber;
    //string metadataServerAddress;
    //vector<string> liveDataServer;
    try{
        //
        po::store(optionsParser, vm);
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

void GlobalConfiguration::checkDefaultOption() throw (CException) {
    
    //lock file for permit to choose different tcp port for services
    std::fstream domain_file_lock_stream("/tmp/chaos_init.lock",
                                         std::ios_base::out |
                                         std::ios_base::binary);
    
    //check if we have got the lock
    boost::interprocess::file_lock flock("/tmp/chaos_init.lock");
    boost::interprocess::scoped_lock<boost::interprocess::file_lock> e_lock(flock);
    
    //now we can fill the gloabl configuration
    //start with getting log configuration
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(logOnConsole, InitOption::OPT_LOG_ON_CONSOLE)
    configuration.addBoolValue(InitOption::OPT_LOG_ON_CONSOLE, logOnConsole);
    
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(logOnFile, InitOption::OPT_LOG_ON_FILE)
    configuration.addBoolValue(InitOption::OPT_LOG_ON_FILE, logOnFile);
    
    CHECK_AND_DEFINE_OPTION(string, logFilePath, InitOption::OPT_LOG_FILE)
    configuration.addStringValue(InitOption::OPT_LOG_FILE, logFilePath);
    
    CHECK_AND_DEFINE_OPTION(string, logLevel, InitOption::OPT_LOG_LEVEL)
    configuration.addInt32Value(InitOption::OPT_LOG_LEVEL, filterLogLevel(logLevel));
    
    CHECK_AND_DEFINE_OPTION(string, publishingIp, InitOption::OPT_PUBLISHING_IP)
    bool isIp = regex_match(publishingIp, ServerIPRegExp);
    if(isIp) configuration.addStringValue(InitOption::OPT_PUBLISHING_IP, publishingIp);
    
    //configure rpc
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(int, rpcServerPort, InitOption::OPT_RPC_SERVER_PORT, 8888)
    int32_t freeFoundPort = InetUtility::scanForLocalFreePort(rpcServerPort);
    addLocalServerBasePort(freeFoundPort);
    configuration.addInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT, freeFoundPort);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(int, rpcServerThreadNumber, InitOption::OPT_RPC_SERVER_THREAD_NUMBER, 1)
    configuration.addInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER, rpcServerThreadNumber);
    
    CHECK_AND_DEFINE_OPTION(string, rpcImpl, InitOption::OPT_RPC_IMPLEMENTATION)
    configuration.addStringValue(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE, rpcImpl);
    
    CHECK_AND_DEFINE_OPTION(bool, rpc_sync_enable, InitOption::OPT_RPC_SYNC_ENABLE)
    else{
        rpc_sync_enable = false;
    }
    configuration.addBoolValue(RpcConfigurationKey::CS_CMDM_RPC_SYNC_ENABLE, rpc_sync_enable);
    
    CHECK_AND_DEFINE_OPTION(string, rpc_sync_impl, InitOption::OPT_RPC_SYNC_IMPLEMENTATION)
    configuration.addStringValue(RpcConfigurationKey::CS_CMDM_RPC_SYNC_ADAPTER_TYPE, rpc_sync_impl);
    
    CHECK_AND_DEFINE_OPTION(string, rpc_impl_kv_param, InitOption::OPT_RPC_IMPL_KV_PARAM)
    configuration.addStringValue(RpcConfigurationKey::CS_CMDM_RPC_KV_IMPL_PARAM_STRING_REGEX, rpc_sync_impl);
    
    //fill the key value list
    if(rpc_impl_kv_param.size()) {
        fillKVParameter(map_kv_param_rpc_impl, rpc_impl_kv_param, std::string(RpcConfigurationKey::CS_CMDM_RPC_KV_IMPL_PARAM_STRING_REGEX));
    }
    
    //direct io
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(int, direct_io_server_thread_number, InitOption::OPT_DIRECT_IO_SERVER_THREAD_NUMBER, 2)
    configuration.addInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_SERVER_THREAD_NUMBER, direct_io_server_thread_number);
    
    CHECK_AND_DEFINE_OPTION(string, direct_io_server_impl, InitOption::OPT_DIRECT_IO_IMPLEMENTATION)
    configuration.addStringValue(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_IMPL_TYPE, direct_io_server_impl);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(int, direct_io_priority_port, InitOption::OPT_DIRECT_IO_PRIORITY_SERVER_PORT, _DIRECT_IO_PRIORITY_PORT)
    freeFoundPort = InetUtility::scanForLocalFreePort(direct_io_priority_port);
    configuration.addInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_PRIORITY_PORT, (uint32_t)freeFoundPort);
    
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(int, direct_io_service_port, InitOption::OPT_DIRECT_IO_SERVICE_SERVER_PORT, _DIRECT_IO_SERVICE_PORT)
    freeFoundPort = InetUtility::scanForLocalFreePort(direct_io_service_port);
    configuration.addInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_SERVICE_PORT, (uint32_t)freeFoundPort);
    
    //cevent
    configuration.addStringValue(event::EventConfiguration::OPTION_KEY_EVENT_ADAPTER_IMPLEMENTATION, "AsioImpl");
    
    //configure the live data
    CHECK_AND_DEFINE_OPTION(vector<string>, liveDataServer, InitOption::OPT_LIVE_DATA_SERVER_ADDRESS)
    if(liveDataServer.size()==0){
        configuration.appendStringToArray("localhost:11211");
    }else{
        for (int idx = 0; idx < liveDataServer.size(); idx++) {
            configuration.appendStringToArray(liveDataServer[idx]);
        }
    }
    configuration.finalizeArrayForKey(DataProxyConfigurationKey::CS_DM_LD_SERVER_ADDRESS);
    
    //configure metadataserver
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(string, metadataServerAddress, InitOption::OPT_METADATASERVER_ADDRESS, "localhost:5000")
    if (metadataServerAddress.size()>0) {
        addMetadataServerAddress(metadataServerAddress);
    }
}


/*
 Add a custom option
 */
void GlobalConfiguration::addOption(const char* name,
                                    const char* description) throw (CException){
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
                                    const char* description) throw (CException){
    try {
        desc.add_options()(name, s, description);
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::addOption");
    }
}

void GlobalConfiguration::fillKVParameter(std::map<std::string, std::string>& kvmap,
                                          const std::string& kv_string,
                                          const std::string& regex) {
    //no cache server provided
    std::string tmp = kv_string;
    if(!regex_match(tmp, boost::regex(regex.c_str()))) {
        throw chaos::CException(-3, "Malformed kv parameter string", __PRETTY_FUNCTION__);
    }
    std::vector<std::string> kvtokens;
    std::vector<std::string> kv_splitted;
    algorithm::split(kvtokens,
                     kv_string,
                     algorithm::is_any_of("|"),
                     algorithm::token_compress_on);
    for (int idx = 0;
         idx < kvtokens.size();
         idx++) {
        //clear previosly pair
        kv_splitted.clear();
        
        //get new pair
        algorithm::split(kv_splitted,
                         kvtokens[idx],
                         algorithm::is_any_of("="),
                         algorithm::token_compress_on);
        // add key/value pair
        kvmap.insert(make_pair(kv_splitted[0], kv_splitted[1]));
    }
}

/**
 *return the cdatawrapper that contains the global configuraiton
 */
chaos_data::CDataWrapper *GlobalConfiguration::getConfiguration(){
    return &configuration;
}

/**
 *Add the metadataserver address
 */
void GlobalConfiguration::addMetadataServerAddress(const string& mdsAddress) throw (CException) {
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
void GlobalConfiguration::addLocalServerAddress(const std::string& mdsAddress) throw (CException) {
    bool isIp = regex_match(mdsAddress, ServerIPRegExp);
    if(!isIp)
        throw CException(1, "Bad server address", "GlobalConfiguration::addMetadataServerAddress");
    
    //address can be added
    configuration.addStringValue("local_ip", mdsAddress);
}

/**
 *Add the metadataserver address
 */
void GlobalConfiguration::addLocalServerBasePort(int32_t localDefaultPort) throw (CException) {
    configuration.addInt32Value("base_port", localDefaultPort);
}

/*
 return the address of metadataserver
 */
string GlobalConfiguration::getMetadataServerAddress() {
    return configuration.getStringValue(DataProxyConfigurationKey::CS_LIB_METADATASET_ADDRESS);
}

/*
 return the address of metadataserver
 */
string GlobalConfiguration::getLocalServerAddress() {
    return configuration.getStringValue("local_ip");
}

/*
 return the address of metadataserver
 */
int32_t GlobalConfiguration::getLocalServerBasePort() {
    return configuration.getInt32Value("base_port");
}

string GlobalConfiguration::getLocalServerAddressAnBasePort(){
    char buf[128];
    string addr = configuration.getStringValue("local_ip");
    sprintf ( buf, "%s:%d", addr.c_str(), (int)configuration.getInt32Value("base_port"));
    addr.assign(buf);
    return addr;
}

/*
 return the address of metadataserver
 */
bool GlobalConfiguration::isMEtadataServerConfigured() {
    return configuration.hasKey(DataProxyConfigurationKey::CS_LIB_METADATASET_ADDRESS);
}

const std::map<std::string, std::string>& GlobalConfiguration::getRpcImplKVParam() const {
    return map_kv_param_rpc_impl;
}
