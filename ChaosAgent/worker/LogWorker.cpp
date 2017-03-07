/*
 *	LogWorker.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/03/2017 INFN, National Institute of Nuclear Physics
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

#include "LogWorker.h"
#include "ProcUtil.h"

#include <chaos_service_common/data/node/Agent.h>

#include <boost/filesystem.hpp>

#define INFO  INFO_LOG(LogWorker)
#define ERROR ERR_LOG(LogWorker)
#define DBG   DBG_LOG(LogWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;

#define NUM_OF_LINE_TO_READ 10
#define SCAN_TIME_INTERVALL 500

LogWorker::LogWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::LogWorker::RPC_DOMAIN) {
    //register rpc action
    DeclareAction::addActionDescritionInstance<LogWorker>(this,
                                                          &LogWorker::starLoggingAssociation,
                                                          getName(),
                                                          AgentNodeDomainAndActionRPC::LogWorker::ACTION_START_LOGGING_ASSOCIATION,
                                                          "Start logging a managed process consol output");
    DeclareAction::addActionDescritionInstance<LogWorker>(this,
                                                          &LogWorker::stopLoggingAssociation,
                                                          getName(),
                                                          AgentNodeDomainAndActionRPC::LogWorker::ACTION_STOP_LOGGING_ASSOCIATION,
                                                          "Start logging a managed process consol output");
    
}

LogWorker::~LogWorker() {}

void LogWorker::init(void *data) throw(chaos::CException) {
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 SCAN_TIME_INTERVALL,
                                                 SCAN_TIME_INTERVALL);
}

void LogWorker::deinit() throw(chaos::CException) {
    AsyncCentralManager::getInstance()->removeTimer(this);
}

CDataWrapper *LogWorker::starLoggingAssociation(CDataWrapper *data,
                                                bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Input data is mandatory");
    CHECK_MANDATORY_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERROR, -2);
    CHECK_TYPE_OF_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, CDataWrapper, ERROR, -3);
    
    std::auto_ptr<CDataWrapper> assoc_ser(data->getCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
    
    AgentAssociationSDWrapper node_association;
    node_association.deserialize(assoc_ser.get());
    
    LockableMapLoggingPipeWriteLock wl = map_logging_file.getWriteLockObject();
    
    //! add logging file for association
    if(map_logging_file().count(node_association().associated_node_uid)) {
        //logging for node already enabled
        INFO << CHAOS_FORMAT("Logging for %1% already enabled",%node_association().associated_node_uid);
        return NULL;
    }
    
    //open named pipe for read logging
    boost::filesystem::path queue_file = CHAOS_FORMAT("%1%/%2%", %QUEUE_FILE_PATH()%NPIPE_FILE_NAME(node_association()));
    CHAOS_FORMAT("Start logging for %1% on named pipe %2%",%node_association().associated_node_uid%queue_file.string());
    
    if(boost::filesystem::exists(queue_file)) {
        boost::shared_ptr<std::ifstream> log_stream(new std::ifstream());
        log_stream->open(queue_file.string());
        if(log_stream->good()) {
            map_logging_file().insert(MapLoggingPipePair(node_association().associated_node_uid, log_stream));
        }
    }
    return NULL;
}

CDataWrapper *LogWorker::stopLoggingAssociation(CDataWrapper *data,
                                                bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Input data is mandatory");
    
    CHECK_MANDATORY_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERROR, -2);
    CHECK_TYPE_OF_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, CDataWrapper, ERROR, -3);
    
    std::auto_ptr<CDataWrapper> assoc_ser(data->getCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
    
    AgentAssociationSDWrapper node_association;
    node_association.deserialize(assoc_ser.get());
    LockableMapLoggingPipeWriteLock wl = map_logging_file.getWriteLockObject();
    if(map_logging_file().count(node_association().associated_node_uid) == 0) {
        //logging for node already enabled
        INFO << CHAOS_FORMAT("Logging for %1% already disabled",%node_association().associated_node_uid);
        return NULL;
    }
    
    boost::shared_ptr<std::ifstream> log_stream = map_logging_file()[node_association().associated_node_uid];
    map_logging_file().erase(node_association().associated_node_uid);
    if(log_stream->good()) {
        log_stream->close();
    }
    return NULL;
}

void LogWorker::timeout() {
    //!lock table in read mode
    LockableMapLoggingPipeReadLock rl = map_logging_file.getReadLockObject();

    std::string log_line;
    for(MapLoggingPipeIterator it = map_logging_file().begin(),
        end = map_logging_file().end();
        it != end;
        it++){
        //scan each log file reading some amount of line
        //(*it.second) >> log_line;
    }
}
