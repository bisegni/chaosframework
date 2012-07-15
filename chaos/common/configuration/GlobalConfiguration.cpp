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
#include <iostream>

#include "GlobalConfiguration.h"

using namespace chaos;
namespace po = boost::program_options;

/*
 
 */
void GlobalConfiguration::preParseStartupParameters() throw (CException){

    try{   
        addOption(InitOption::OPT_HELP, "Produce help message");
        addOption(InitOption::OPT_RPC_SERVER_PORT, po::value<int>()->default_value(8888), "RPC server port");
        addOption(InitOption::OPT_RPC_SERVER_THREAD_NUMBER, po::value<int>()->default_value(2),"RPC server thread number");
        addOption(InitOption::OPT_LIVE_DATA_SERVER_ADDRESS, po::value< vector<string> >()->multitoken(), "Live server:port address");
        addOption(InitOption::OPT_METADATASERVER_ADDRESS, po::value< string >()->default_value("localhost:5000"), "Metadataserver server:port address");
        addOption(InitOption::OPT_LOG_ON_CONSOLE, po::value< bool >()->zero_tokens(), "Specify when the log must be forwarded on console");
        addOption(InitOption::OPT_LOG_ON_FILE, po::value< bool >()->zero_tokens(), "Specify when the log must be forwarded on file");
        addOption(InitOption::OPT_LOG_FILE, po::value< string >()->default_value("chaos_frameowrk.log"), "Specify when the file path of the log");
        addOption(InitOption::OPT_PUBLISHING_IP, po::value< string >(), "Specify the ip address where to publish the framework rpc system");
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::preParseStartupParameters");
    }
}

/*
 parse the tandard startup parameters
 */
void GlobalConfiguration::parseStartupParameters(int argc, char* argv[]) throw (CException){
        //int rpcServerPort;
        //int rpcServerThreadNumber;
        //string metadataServerAddress;
        //vector<string> liveDataServer;
    try{   
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm); 
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::preParseStartupParameters");

    }
    
    if (hasOption(InitOption::OPT_HELP)) {
        std::cout << desc;
        exit(0);
        return;
    }
    
        //now we can fill the gloabl configuration
        //start with getting log configuration
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(logOnConsole, InitOption::OPT_LOG_ON_CONSOLE)
    configuration.addBoolValue(InitOption::OPT_LOG_ON_CONSOLE, logOnConsole);
 
    CHECK_AND_DEFINE_BOOL_ZERO_TOKEN_OPTION(logOnFile, InitOption::OPT_LOG_ON_FILE)
    configuration.addBoolValue(InitOption::OPT_LOG_ON_FILE, logOnFile);

    CHECK_AND_DEFINE_OPTION(string, logFilePath, InitOption::OPT_LOG_FILE)
    configuration.addStringValue(InitOption::OPT_LOG_FILE, logFilePath);
    
    CHECK_AND_DEFINE_OPTION(string, publishingIp, InitOption::OPT_PUBLISHING_IP)
    bool isIp = regex_match(publishingIp, ServerIPRegExp);
    if(isIp) configuration.addStringValue(InitOption::OPT_PUBLISHING_IP, publishingIp);
    
        //configure rpc
    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(int, rpcServerPort, InitOption::OPT_RPC_SERVER_PORT, 8888)
    int freeFoundPort = InetUtility::scanForLocalFreePort(rpcServerPort);
    addLocalServerBasePort(freeFoundPort);
    configuration.addInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT, freeFoundPort);    
    

    CHECK_AND_DEFINE_OPTION_WITH_DEFAULT(int, rpcServerThreadNumber, InitOption::OPT_RPC_SERVER_THREAD_NUMBER, 2)
    configuration.addInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER, rpcServerThreadNumber);
    
    //configure the unique rpc plugin
    configuration.addStringValue(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TYPE, "MsgPack");
    
        //configure the live data
    CHECK_AND_DEFINE_OPTION(vector<string>, liveDataServer, InitOption::OPT_LIVE_DATA_SERVER_ADDRESS)
    if(liveDataServer.size()==0){
        configuration.appendStringToArray("localhost:11211");
    }else{
        for (int idx = 0; idx < liveDataServer.size(); idx++) {
            configuration.appendStringToArray(liveDataServer[idx]);
        }
    }
    configuration.finalizeArrayForKey(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS);
    
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
