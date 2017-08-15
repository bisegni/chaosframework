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
                typedef ChaosWeakPtr<PipeLineReader>     PipeLineReaderWeakPtr;
                
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
