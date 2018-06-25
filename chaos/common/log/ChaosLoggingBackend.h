/*
 * Copyright 2012, 29/01/2018 INFN
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

#ifndef __CHAOSFramework__1BA8927_9D5A_4782_83F3_C93ERQTD5061_data_h
#define __CHAOSFramework__1BA8927_9D5A_4782_83F3_C93ERQTD5061_data_h

#include <chaos/common/io/ManagedDirectIODataDriver.h>

#include <boost/log/sinks/basic_sink_backend.hpp>

namespace chaos {
    namespace common {
        namespace log {
            
            class ChaosLoggingBackend:
            public boost::log::sinks::basic_sink_backend<
            boost::log::sinks::combine_requirements<
            boost::log::sinks::synchronized_feeding,
            boost::log::sinks::flushing>::type> {
            private:
                std::string log_source;
                ChaosStringVector log_entries;
                common::utility::InizializableServiceContainer<chaos::common::io::ManagedDirectIODataDriver> data_driver;
            public:
                ChaosLoggingBackend();
                ~ChaosLoggingBackend();
                void setSource(const std::string& _log_source);
                // The function consumes the log records that come from the frontend
                void consume(boost::log::record_view const& rec);
                // The function flushes the file
                void flush();
            };
        }
    }
}

#endif /* __CHAOSFramework__1BA8927_9D5A_4782_83F3_C93ERQTD5061_data_h */
