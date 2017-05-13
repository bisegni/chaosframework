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

#include <ChaosAgent/ChaosAgent.h>
#include <ChaosAgent/utility/ProcUtil.h>
#include <chaos/common/network/NetworkBroker.h>

#include <chaos_service_common/data/node/Agent.h>

#include <boost/filesystem.hpp>

#define INFO  INFO_LOG(LogWorker)
#define ERROR ERR_LOG(LogWorker)
#define DBG   DBG_LOG(LogWorker)

using namespace chaos::agent;
using namespace chaos::agent::worker;
using namespace chaos::service_common::data::agent;

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;

using namespace boost::asio;

#define NUM_OF_LINE_TO_READ 10
#define SCAN_TIME_INTERVALL 500
#pragma mark PipeReader


PipeReader::PipeReaderWeakPtr PipeReader::create(io_service& io_service,
                                                 const std::string node_uid,
                                                 const std::string& path,
                                                 chaos::common::io::ManagedDirectIODataDriver *_data_driver,
                                                 bool enable_remote) {
    PipeReaderPtr ptr(new PipeReader(io_service, node_uid, path, _data_driver));
    ptr->remote_logging = enable_remote;
    
    boost::asio::async_read_until(ptr->m_pipe,
                                  ptr->asio_buffer,
                                  "\n",
                                  boost::bind(&PipeReader::handleRead,
                                              ptr.get(),
                                              ptr,
                                              asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
    return ptr;
}

PipeReader::PipeReader(io_service& io_service,
                       const std::string _node_uid,
                       const std::string& path,
                       chaos::common::io::ManagedDirectIODataDriver *_data_driver):
m_pipe(io_service),
node_uid(_node_uid),
data_driver(_data_driver),
remote_logging(false){
    int dev = open(path.c_str(), O_RDWR);
    if (dev == -1) {
        std::cout << "failed to open path - " << path << std::endl;
    } else {
        m_pipe.assign(dev);
    }
}

void PipeReader::close() {
    m_pipe.close();
}

void PipeReader::handleRead(PipeReaderPtr me,
                            const boost::system::error_code &error,
                            std::size_t bytes_transferred) {
    int err = 0;
    if (!error) {
        std::string line;
        std::istream is(&me->asio_buffer);
        
        if (std::getline(is, line)) {
            if(ChaosAgent::getInstance()->settings.enable_us_merge_logging) {
                //log on standard chaos log
                INFO << CHAOS_FORMAT("[%1%] - %2%", %node_uid%line);
            }
            //chak if remote logging is enabled
            if (remote_logging) {
                log_line.push_back(line);
                data_driver->storeLogEntries(node_uid,
                                             log_line);
                log_line.clear();
            }
            
        }
        boost::asio::async_read_until(me->m_pipe,
                                      me->asio_buffer,
                                      "\n",
                                      boost::bind(&PipeReader::handleRead,
                                                  this,
                                                  me,
                                                  asio::placeholders::error,
                                                  boost::asio::placeholders::bytes_transferred));
    } else {
        ERROR << "got error - " << error.message() << std::endl;
    }
}
#pragma mark LogWorker
LogWorker::LogWorker():
AbstractWorker(AgentNodeDomainAndActionRPC::LogWorker::RPC_DOMAIN),
dummy_work(io_service) {
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
    data_driver.reset(new ManagedDirectIODataDriver(), "ManagedDirectIODataDriver");
    if(data_driver.get() == NULL) throw chaos::CException(-1, "error creating driver", __PRETTY_FUNCTION__);
    data_driver.init(NULL, __PRETTY_FUNCTION__);
    
    asio_threads.create_thread(bind(&asio::io_service::run, &io_service));
}

void LogWorker::deinit() throw(chaos::CException) {
    io_service.stop();
    asio_threads.join_all();
    
    CHAOS_NOT_THROW(data_driver.deinit(__PRETTY_FUNCTION__););
}

bool LogWorker::startLogFetcher(AgentAssociation& agent_association,
                                bool enable_remote) {
    LockableMapLoggingPipeWriteLock wl = map_logging_file.getWriteLockObject();
    
    //! add logging file for association
    if(map_logging_file().count(agent_association.associated_node_uid)) {
        //logging for node already enabled
        INFO << CHAOS_FORMAT("Logging for %1% already enabled",%agent_association.associated_node_uid);
        return false;
    }
    
    //open named pipe for read logging
    boost::filesystem::path queue_file = CHAOS_FORMAT("%1%/%2%", %QUEUE_FILE_PATH()%NPIPE_FILE_NAME(agent_association));
    INFO << CHAOS_FORMAT("Start logging for %1% on named pipe %2%",%agent_association.associated_node_uid%queue_file.string());
    
    PipeReader::PipeReaderWeakPtr pipe_reader_wptr = PipeReader::create(io_service,
                                                                        agent_association.associated_node_uid,
                                                                        queue_file.string(),
                                                                        data_driver.get(),
                                                                        enable_remote);
    map_logging_file().insert(MapLoggingPipePair(agent_association.associated_node_uid,
                                                 pipe_reader_wptr));
    return true;
}

void LogWorker::enableRemoteLogging(AgentAssociation& agent_association,
                                    bool enable) {
    LockableMapLoggingPipeReadLock wl = map_logging_file.getReadLockObject();
    PipeReader::PipeReaderWeakPtr pipe_reader_wptr = map_logging_file()[agent_association.associated_node_uid];
    if(PipeReader::PipeReaderPtr pipe_readr_ptr = pipe_reader_wptr.lock()) {
        pipe_readr_ptr->remote_logging = enable;
    }
}

void LogWorker::stopLogFetcher(AgentAssociation& agent_association) {
    LockableMapLoggingPipeWriteLock wl = map_logging_file.getWriteLockObject();
    if(map_logging_file().count(agent_association.associated_node_uid) == 0) {
        //logging for node already enabled
        INFO << CHAOS_FORMAT("Logging for %1% already disabled",%agent_association.associated_node_uid);
        return;
    }
    INFO << CHAOS_FORMAT("Disable logging for %1%",%agent_association.associated_node_uid);
    PipeReader::PipeReaderWeakPtr pipe_reader_wptr = map_logging_file()[agent_association.associated_node_uid];
    if(PipeReader::PipeReaderPtr pipe_readr_ptr = pipe_reader_wptr.lock()) {
        pipe_readr_ptr->close();
    }
    map_logging_file().erase(agent_association.associated_node_uid);
}

CDataWrapper *LogWorker::starLoggingAssociation(CDataWrapper *data,
                                                bool& detach) {
    CHECK_CDW_THROW_AND_LOG(data,
                            ERROR, -1,
                            "Input data is mandatory");
    CHECK_MANDATORY_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, ERROR, -2);
    CHECK_TYPE_OF_KEY(data, AgentNodeDefinitionKey::NODE_ASSOCIATED, CDataWrapper, ERROR, -3);
    
    std::unique_ptr<CDataWrapper> assoc_ser(data->getCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
    
    AgentAssociationSDWrapper agent_association;
    agent_association.deserialize(assoc_ser.get());
    
    if(startLogFetcher(agent_association(),
                       true) == false) {
        enableRemoteLogging(agent_association(),
                            true);
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
    
    std::unique_ptr<CDataWrapper> assoc_ser(data->getCSDataValue(AgentNodeDefinitionKey::NODE_ASSOCIATED));
    
    AgentAssociationSDWrapper agent_association;
    agent_association.deserialize(assoc_ser.get());
    
    enableRemoteLogging(agent_association(),
                        false);
    return NULL;
}
