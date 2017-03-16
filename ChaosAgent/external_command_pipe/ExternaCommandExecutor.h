/*
 *	ExternaCommandExecutor.h
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

#ifndef __CHAOSFramework__4A9675F_4899_4A01_A6DB_657FAB56B314_ExternaCommandExecutor_h
#define __CHAOSFramework__4A9675F_4899_4A01_A6DB_657FAB56B314_ExternaCommandExecutor_h

#include "../utility/PipeLineReader.h"

#include <chaos_service_common/data/node/Agent.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/io/ManagedDirectIODataDriver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include <iostream>

namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            //! define the worker that permit to deploy chaos executable on host
            class ExternaCommandExecutor:
            public utility::PipeLineReaderReceiver,
            public chaos::common::utility::InizializableService {
                boost::filesystem::path pipe_in_path;
                boost::filesystem::path pipe_out_path;
                 FILE *output_fd;
                
                boost::thread_group asio_threads;
                boost::asio::io_service io_service;
                boost::asio::io_service::work dummy_work;
                
                utility::PipeLineReader::PipeLineReaderWeakPtr pip_line_reader;
                
                common::message::MDSMessageChannel *mds_message_channel;
            protected:
                void readLine(const std::string& new_read_line);
            public:
                ExternaCommandExecutor();
                ~ExternaCommandExecutor();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
            };
        }
    }
}

#endif /* __CHAOSFramework__4A9675F_4899_4A01_A6DB_657FAB56B314_ExternaCommandExecutor_h */
