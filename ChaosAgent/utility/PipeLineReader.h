/*
 *	PipeReader.h
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

#ifndef __CHAOSFramework__6C0FC78_8265_4026_92B1_93DC5EABF789_PipeReader_h
#define __CHAOSFramework__6C0FC78_8265_4026_92B1_93DC5EABF789_PipeReader_h

#include <chaos/common/chaos_types.h>

#include <boost/asio.hpp>

#include <fstream>

namespace chaos {
    namespace agent {
        namespace utility {
            typedef boost::asio::posix::stream_descriptor AsioPipeStreamDescriptor;
            class PipeLineReaderReceiver {
            public:
                virtual void readLine(const std::string& new_read_line) = 0;
            };
            
            class PipeLineReader {
            public:
                typedef ChaosSharedPtr<PipeLineReader>   PipeLineReaderPtr;
                typedef boost::weak_ptr<PipeLineReader>     PipeLineReaderWeakPtr;
                
                explicit PipeLineReader(boost::asio::io_service& io_service,
                                        const std::string& path,
                                        PipeLineReaderReceiver *_receiver);
                void close();
                static PipeLineReaderWeakPtr start(PipeLineReader *new_reader);
                
                void handleRead(PipeLineReaderPtr me,
                                const boost::system::error_code &error,
                                std::size_t bytes_transferred);
                
            private:
                char buf[4096];
                PipeLineReaderReceiver *receiver;
                AsioPipeStreamDescriptor m_pipe;
                boost::asio::streambuf asio_buffer;
            };
        }
    }
}
#endif /* __CHAOSFramework__6C0FC78_8265_4026_92B1_93DC5EABF789_PipeReader_h */
