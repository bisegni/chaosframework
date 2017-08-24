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

#include "PipeLineReader.h"

#include <chaos/common/global.h>

using namespace chaos::agent::utility;

using namespace boost;
using namespace boost::asio;

#define INFO  INFO_LOG(PipeLineReader)
#define ERROR ERR_LOG(PipeLineReader)
#define DBG   DBG_LOG(PipeLineReader)

PipeLineReader::PipeLineReaderWeakPtr PipeLineReader::start(PipeLineReader *new_reader) {
    PipeLineReaderPtr ptr(new_reader);
    
    boost::asio::async_read_until(ptr->m_pipe,
                                  ptr->asio_buffer,
                                  "\n",
                                  boost::bind(&PipeLineReader::handleRead,
                                              ptr.get(),
                                              ptr,
                                              asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
    return ptr;
}

PipeLineReader::PipeLineReader(io_service& io_service,
                               const std::string& path,
                               PipeLineReaderReceiver *_receiver):
m_pipe(io_service),
receiver(_receiver) {
    int dev = open(path.c_str(), O_RDWR);
    if (dev == -1) {
        std::cout << "failed to open path - " << path << std::endl;
    } else {
        m_pipe.assign(dev);
    }
}

void PipeLineReader::close() {
    m_pipe.close();
}

void PipeLineReader::handleRead(PipeLineReaderPtr me,
                                const boost::system::error_code &error,
                                std::size_t bytes_transferred) {
    if (!error) {
        std::string line;
        std::istream is(&me->asio_buffer);
        if (std::getline(is, line)) {
            if(receiver){receiver->readLine(line);}
        }
        boost::asio::async_read_until(me->m_pipe,
                                      me->asio_buffer,
                                      "\n",
                                      boost::bind(&PipeLineReader::handleRead,
                                                  this,
                                                  me,
                                                  asio::placeholders::error,
                                                  boost::asio::placeholders::bytes_transferred));
    } else {
        ERROR << "got error - " << error.message() << std::endl;
    }
}
