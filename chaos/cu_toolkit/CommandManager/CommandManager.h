    //
    //  CommandManager.h
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 21/02/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef CommandManager_H
#define CommandManager_H

#include <map>
#include <vector>
#include <map>
#include <string>
    //#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <chaos/common/global.h>
#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>
#include <chaos/common/general/Manager.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/ObjectFactoryAliasInstantiation.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/cu_toolkit/CommandManager/CommandManagerDefaultAdapters.h>
    //#include <chaos/ChaosCUToolkit.h"

namespace chaos{
    using namespace std;
    using namespace boost;
    
    /*
     * CommandManager
     * - Command Manager is the central class for the registration and execution of the custom command
     *
     */
    class CommandManager : public DeclareAction,  public Manager, public Configurable, public Singleton<CommandManager> {
        friend class RpcAdapterRegister;
        friend class CommandDispatcherRegister;
        friend class ServerDelegator;
        friend class Singleton<CommandManager>;
            //rpc server isntance
        shared_ptr<RpcServer> rpcServer;
            //rpc server isntance
        shared_ptr<RpcClient> rpcClient;       
            //take the reference to the dispatcher used by all rpc
        shared_ptr<CommandDispatcher> cmdDispatcher;
        
            //take care to startup an rpc adapter according to configuration
        void startupRPCAdapter(CDataWrapper*)throw(CException);

    public:
        /*
         reference to the master pirv lib controller, for shutdown operation
         by rpc
         */
        ServerDelegator *privLibControllerPtr;
        
        /*
         * Initzialize the command manager
         */
        void init() throw(CException);
        /*
         * Deinitzialize the command manager
         */
        void deinit() throw(CException);
        
        /*
         * Start all sub process
         */
        void start() throw(CException);
        
        /*
         Configure the sandbox and all subtree of the CU
         */
        CDataWrapper* updateConfiguration(CDataWrapper*);
        
        /*
         Send message over rpc channel
         */
        void sendMessage(string&, CDataWrapper*, bool onThisThread = false) throw(CException);
        
        /*
         Send message over rpc channel
         */
        void sendMessageToMetadataServer(CDataWrapper*, bool onThisThread = false) throw(CException);
        
        /*
         Register actions defined by AbstractActionDescriptor instance contained in the array
         */
        void registerAction(DeclareAction*);
        /*
         Deregister actions for a determianted domain
         */
        void deregisterAction(DeclareAction*);
        
        /*
         Shutdown the chaos control library
         */
        CDataWrapper* shutdown(CDataWrapper*) throw (CException);
    private:
        CommandManager();
        ~CommandManager();
    };
}
#endif