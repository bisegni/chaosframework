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

#include "ExternaCommandExecutor.h"

#include "../ChaosAgent.h"
#include "../utility/ProcUtil.h"

#include "FileCommandOutputStream.h"
#include "CommandEcho.h"
#include "GetUSStatistic.h"

#include <chaos/common/network/NetworkBroker.h>

#include <boost/algorithm/string.hpp>

#include <fstream>

#define INFO  INFO_LOG(ExternaCommandExecutor)
#define ERROR ERR_LOG(ExternaCommandExecutor)
#define DBG   DBG_LOG(ExternaCommandExecutor)

using namespace chaos::agent::utility;
using namespace chaos::common::network;
using namespace chaos::agent::external_command_pipe;


ExternaCommandExecutor::ExternaCommandExecutor():
dummy_work(io_service){}

ExternaCommandExecutor::~ExternaCommandExecutor(){}

void ExternaCommandExecutor::init(void *data)  {
    //start asio
    asio_threads.create_thread(bind(&asio::io_service::run, &io_service));
    
    //create MDS channel
    mds_message_channel = NetworkBroker::getInstance()->getMetadataserverMessageChannel();
    CHECK_ASSERTION_THROW_AND_LOG((mds_message_channel != NULL), ERROR, -1, "Error creating new mds channel");
    
    //open pipe
    pipe_in_path = CHAOS_FORMAT("%1%/%2%.in",%ChaosAgent::getInstance()->settings.ext_cmd_pipe_path%ChaosAgent::getInstance()->settings.ext_cmd_pipe_name);
    pipe_out_path = CHAOS_FORMAT("%1%/%2%.out",%ChaosAgent::getInstance()->settings.ext_cmd_pipe_path%ChaosAgent::getInstance()->settings.ext_cmd_pipe_name);
    
    ProcUtil::removeNamedPipe(pipe_in_path.string());
    ProcUtil::createNamedPipe(pipe_in_path.string());
    
    ProcUtil::removeNamedPipe(pipe_out_path.string());
    ProcUtil::createNamedPipe(pipe_out_path.string());
    
    //output_fd = fopen(pipe_out_path.string().c_str(), "wa");
    external_cmd_executor.reset(new FileCommandOutputStream(pipe_out_path.string()), "FileCommandOutputStream");
    external_cmd_executor.init(NULL, __PRETTY_FUNCTION__);
    
    //add command
    map_command.insert(MapCommandPair("ECHO", AbstractExternalCommandShrdPtr(new CommandEcho(*external_cmd_executor,
                                                                                             *mds_message_channel))));
    map_command.insert(MapCommandPair("US_STAT", AbstractExternalCommandShrdPtr(new GetUSStatistic(*external_cmd_executor,
                                                                                                   *mds_message_channel))));
    //read on input pipe
    pip_line_reader = PipeLineReader::start(new PipeLineReader(io_service,
                                                               pipe_in_path.string(),
                                                               this));
}

void ExternaCommandExecutor::deinit()  {
    CHAOS_NOT_THROW(external_cmd_executor.deinit(__PRETTY_FUNCTION__););
    io_service.stop();
    asio_threads.join_all();
    ProcUtil::removeNamedPipe(pipe_in_path.string());
    ProcUtil::removeNamedPipe(pipe_out_path.string());
    
    if(mds_message_channel) {
        NetworkBroker::getInstance()->disposeMessageChannel(mds_message_channel);
        mds_message_channel = NULL;
    }
}

void ExternaCommandExecutor::readLine(const std::string& new_read_line) {
    int err = 0;
    ChaosStringVector tokens;
    boost::split(tokens,
                 new_read_line,
                 boost::is_any_of(" "));
    INFO << CHAOS_FORMAT("External command received = %1% number fo tockes %2%", %new_read_line%tokens.size());
    
    //tokens 0 need to be the command to execute
    if(map_command.count(tokens[0]) == 0) {
        //print error
        *external_cmd_executor.get() << CHAOS_FORMAT("Command %1% is not available\n", %tokens[0]);
    } else if((err = map_command[tokens[0]]->execute(tokens))){
        *external_cmd_executor.get() << CHAOS_FORMAT("Command %1% has reported error %2%\n", %tokens[0]%err);
    }
    
    //add exstra return for command termination
    *external_cmd_executor.get() << "\n";
}
