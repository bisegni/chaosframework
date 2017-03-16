/*
 *	ExternaCommandExecutor.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 16/03/2017 INFN, National Institute of Nuclear Physics
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

#include "ExternaCommandExecutor.h"

#include "../ChaosAgent.h"
#include "../worker/ProcUtil.h"

#include <chaos/common/network/NetworkBroker.h>

#include <fstream>

#define INFO  INFO_LOG(ExternaCommandExecutor)
#define ERROR ERR_LOG(ExternaCommandExecutor)
#define DBG   DBG_LOG(ExternaCommandExecutor)

using namespace chaos::agent::utility;
using namespace chaos::agent::worker;
using namespace chaos::common::network;
using namespace chaos::agent::external_command_pipe;


ExternaCommandExecutor::ExternaCommandExecutor():
dummy_work(io_service),
output_fd(NULL){}

ExternaCommandExecutor::~ExternaCommandExecutor(){}

void ExternaCommandExecutor::init(void *data) throw(chaos::CException) {
    //create MDS channel
    mds_message_channel = NetworkBroker::getInstance()->getMetadataserverMessageChannel();
    CHECK_ASSERTION_THROW_AND_LOG((mds_message_channel != NULL), ERROR, -1, "Error creating new mds channel");
    
    //open pipe
    pipe_in_path = CHAOS_FORMAT("%1%/%2%.in",%ChaosAgent::getInstance()->settings.ext_cmd_pipe_path%ChaosAgent::getInstance()->settings.ext_cmd_pipe_name);
    pipe_out_path = CHAOS_FORMAT("%1%/%2%.out",%ChaosAgent::getInstance()->settings.ext_cmd_pipe_path%ChaosAgent::getInstance()->settings.ext_cmd_pipe_name);
    
    ProcUtil::createNamedPipe(pipe_in_path.string());
    ProcUtil::createNamedPipe(pipe_out_path.string());
    
    output_fd = fopen( "pipe_addr", "wa");
    CHECK_ASSERTION_THROW_AND_LOG((output_fd != NULL), ERROR, -4, CHAOS_FORMAT("Error opening out named pipe %1%",%pipe_out_path.string()));
    
    asio_threads.create_thread(bind(&asio::io_service::run, &io_service));
}

void ExternaCommandExecutor::deinit() throw(chaos::CException) {
    io_service.stop();
    asio_threads.join_all();
    fclose(output_fd);
    ProcUtil::removeNamedPipe(pipe_in_path.string());
    ProcUtil::removeNamedPipe(pipe_out_path.string());
    
    if(mds_message_channel) {
        NetworkBroker::getInstance()->disposeMessageChannel(mds_message_channel);
        mds_message_channel = NULL;
    }
}

void ExternaCommandExecutor::readLine(const std::string& new_read_line) {
    INFO << new_read_line;
    fprintf(output_fd, "test receive data\n[ %d ]\n\n", new_read_line.c_str());
    fflush(output_fd);
}
