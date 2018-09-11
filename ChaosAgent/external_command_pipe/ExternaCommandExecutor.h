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

#ifndef __CHAOSFramework__4A9675F_4899_4A01_A6DB_657FAB56B314_ExternaCommandExecutor_h
#define __CHAOSFramework__4A9675F_4899_4A01_A6DB_657FAB56B314_ExternaCommandExecutor_h

#include "../utility/PipeLineReader.h"
#include "AbstractExternalCommand.h"

#include <chaos_service_common/data/node/Agent.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/io/ManagedDirectIODataDriver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "AbstractCommandOutputStream.h"

namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, AbstractExternalCommandShrdPtr, MapCommand);
            
            //! define the worker that permit to deploy chaos executable on host
            class ExternaCommandExecutor:
            public utility::PipeLineReaderReceiver,
            public chaos::common::utility::InizializableService {
                boost::filesystem::path pipe_in_path;
                boost::filesystem::path pipe_out_path;
                
                common::utility::InizializableServiceContainer<AbstractCommandOutputStream> external_cmd_executor;
                
                boost::thread_group asio_threads;
                boost::asio::io_service io_service;
                boost::asio::io_service::work dummy_work;
                
                utility::PipeLineReader::PipeLineReaderWeakPtr pip_line_reader;
                
                common::message::MDSMessageChannel *mds_message_channel;
                
                MapCommand map_command;
            protected:
                void readLine(const std::string& new_read_line);
            public:
                ExternaCommandExecutor();
                ~ExternaCommandExecutor();
                void init(void *data);
                void deinit();
            };
        }
    }
}

#endif /* __CHAOSFramework__4A9675F_4899_4A01_A6DB_657FAB56B314_ExternaCommandExecutor_h */
