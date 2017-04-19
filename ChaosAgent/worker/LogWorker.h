/*
 *	LogWorker.h
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

#ifndef __CHAOSFramework__8315DFE_1D95_48D8_A408_7C4B0D444477_LogWorker_h
#define __CHAOSFramework__8315DFE_1D95_48D8_A408_7C4B0D444477_LogWorker_h

#include "../AbstractWorker.h"

#include <chaos_service_common/data/node/Agent.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/io/ManagedDirectIODataDriver.h>

#include <boost/asio.hpp>

#include <fstream>

namespace chaos {
    namespace agent {
        namespace worker {

            typedef boost::asio::posix::stream_descriptor AsioPipeStreamDescriptor;
            class PipeReader {
                const std::string node_uid;
                ChaosStringVector log_line;
                chaos::common::io::ManagedDirectIODataDriver *data_driver;
            public:
                bool remote_logging;
                typedef boost::shared_ptr<PipeReader>   PipeReaderPtr;
                typedef boost::weak_ptr<PipeReader>     PipeReaderWeakPtr;
                void close();
                static PipeReaderWeakPtr create(asio::io_service& io_service,
                                                const std::string node_uid,
                                                const std::string& path,
                                                chaos::common::io::ManagedDirectIODataDriver *_data_driver,
                                                bool enable_remote);
                
                void handleRead(PipeReaderPtr me,
                                const boost::system::error_code &error,
                                std::size_t bytes_transferred);
            private:
                AsioPipeStreamDescriptor m_pipe;
                char buf[4096];
                boost::asio::streambuf asio_buffer;
                PipeReader(asio::io_service& io_service,
                           const std::string node_uid,
                           const std::string& path,
                           chaos::common::io::ManagedDirectIODataDriver *_data_driver);
            };
            
            //map that containes the file accessor for named pipe
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, PipeReader::PipeReaderWeakPtr, MapLoggingPipe);
            
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapLoggingPipe, LockableMapLoggingPipe);
            
            //! define the worker that permit to deploy chaos executable on host
            class LogWorker:
            public AbstractWorker {
                LockableMapLoggingPipe map_logging_file;
                
                boost::thread_group asio_threads;
                boost::asio::io_service io_service;
                boost::asio::io_service::work dummy_work;
                
                common::utility::InizializableServiceContainer<chaos::common::io::ManagedDirectIODataDriver> data_driver;
            protected:
                chaos::common::data::CDataWrapper *starLoggingAssociation(chaos::common::data::CDataWrapper *data,
                                                                          bool& detach);
                
                chaos::common::data::CDataWrapper *stopLoggingAssociation(chaos::common::data::CDataWrapper *data,
                                                                          bool& detach);
            public:
                LogWorker();
                ~LogWorker();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                bool startLogFetcher(service_common::data::agent::AgentAssociation& agent_association,
                                     bool enable_remote = false);
                void enableRemoteLogging(service_common::data::agent::AgentAssociation& agent_association,
                                         bool enable);
                void stopLogFetcher(service_common::data::agent::AgentAssociation& agent_association);
            };
        }
    }
}
//serviceThreads.create_thread(bind(&asio::io_service::run, &io_service));
#endif /* __CHAOSFramework__8315DFE_1D95_48D8_A408_7C4B0D444477_LogWorker_h */
