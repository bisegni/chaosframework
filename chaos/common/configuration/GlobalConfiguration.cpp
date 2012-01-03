//
//  GlobalConfiguration.cpp
//  ControlSystemLib
//
//  Created by Bisegni Claudio on 01/01/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "GlobalConfiguration.h"

using namespace chaos;
namespace po = boost::program_options;

#define OPT_HELP "help"
#define OPT_RPC_SERVER_PORT "rpc-server-port"
#define OPT_RPC_SERVER_THREAD_NUMBER "rpc-server-thread-number"
#define OPT_LIVE_DATA_SERVER_ADDRESS "live-data-servers"
#define OPT_METADATASERVER_ADDRESS "metadata-server"

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
/*
 
 */
void GlobalConfiguration::preParseStartupParameters() throw (CException){

    try{   
        addOption(OPT_HELP, "Produce help message");
        addOption(OPT_RPC_SERVER_PORT, po::value<int>()->default_value(8888), "RPC server port");
        addOption(OPT_RPC_SERVER_THREAD_NUMBER, po::value<int>()->default_value(2),"RPC server thread number");
        addOption(OPT_LIVE_DATA_SERVER_ADDRESS, po::value< vector<string> >()->multitoken(), "Live server:port address");
        addOption(OPT_METADATASERVER_ADDRESS, po::value< string >()->default_value("localhost:5000"), "Metadataserver server:port address");
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::preParseStartupParameters");
    }
}

/*
 parse the tandard startup parameters
 */
void GlobalConfiguration::parseStartupParameters(int argc,const char* argv[]) throw (CException){
    int rpcServerPort;
    int rpcServerThreadNumber;
    string metadataServerAddress;
    vector<string> liveDataServer;
    try{   
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm); 
    }catch (po::error &e) {
        throw CException(0, e.what(), "GlobalConfiguration::preParseStartupParameters");

    }
    
    if (hasOption(OPT_HELP)) {
        LAPP_ << desc;
        exit(0);
        return;
    }
    
        //now we can fill the gloabl configuration
    
        //configure rpc
    CHECK_OPTION_WITH_DEFAULT(int, rpcServerPort, OPT_RPC_SERVER_PORT, 8888)
    addLocalServerBasePort(rpcServerPort);
    configuration.addInt32Value(CommandManagerConstant::RpcAdapterConstant::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT, rpcServerPort);    
    

    CHECK_OPTION_WITH_DEFAULT(int, rpcServerThreadNumber, OPT_RPC_SERVER_THREAD_NUMBER, 2)
    configuration.addInt32Value(CommandManagerConstant::RpcAdapterConstant::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER, rpcServerThreadNumber);
    
    //configure the unique rpc plugin
    configuration.addStringValue(CommandManagerConstant::RpcAdapterConstant::CS_CMDM_RPC_ADAPTER_TYPE, "MsgPack");
    
        //configure the live data
    CHECK_OPTION(vector<string>, liveDataServer, OPT_LIVE_DATA_SERVER_ADDRESS)
    if(liveDataServer.size()==0){
        configuration.appendStringToArray("localhost:11211");
    }else{
        for (int idx = 0; idx < liveDataServer.size(); idx++) {
            configuration.appendStringToArray(liveDataServer[idx]);
        }
    }
    configuration.finalizeArrayForKey(DataManagerConstant::LiveDataConstant::CS_DM_LD_SERVER_ADDRESS);
    
        //configure metadataserver
    CHECK_OPTION_WITH_DEFAULT(string, metadataServerAddress, OPT_METADATASERVER_ADDRESS, "localhost:5000")
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