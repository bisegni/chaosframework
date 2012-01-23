//
//  AbstractRpcAdapter.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 01/05/11.
//  Copyright 2011 INFN. All rights reserved.
//
#ifndef AbstractRpcAdapter_H
#define AbstractRpcAdapter_H

#include <boost/shared_ptr.hpp>
#include <string>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/dispatcher/CommandDispatcher.h>
#include <chaos/common/exception/CException.h>


namespace chaos {
    /*
     Abstract class for standard adapter method for permit, to CommandManager
     the correct initialization for the adapter instance
     */
    class RpcServer {
        string *typeName;
    protected:
        CommandDispatcher *commandDispatcher;
    public:
        RpcServer(string *alias){typeName = alias;};
            /*
             init the rpc adapter
             */
        virtual void init(CDataWrapper*) throw(CException) = 0;
            
            /*
             start the rpc adapter
             */
        virtual void start() throw(CException) = 0;
        
            /*
             deinit the rpc adapter
             */
        virtual void deinit() throw(CException) = 0;

        
            /*
             set the command dispatcher associated to the instance of rpc adapter
             */
        void setCommandDispatcher(CommandDispatcher *newCommandDispatcher){
            commandDispatcher = newCommandDispatcher;
        }
        
        /*
         Return the adapter alias
         */
        const char * getName() const {
            return typeName->c_str();
        }
    };
}

#endif