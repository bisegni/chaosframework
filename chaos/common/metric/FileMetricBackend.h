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

#ifndef __CHAOSFramework__FileMetricBackend__
#define __CHAOSFramework__FileMetricBackend__

#include <chaos/common/metric/AbstractMetricBackend.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <iostream>

namespace chaos {
    namespace common {
        namespace metric {
            class FileMetricBackend:
            public AbstractMetricBackend {
            protected:
                const boost::filesystem::path file_path;
                boost::filesystem::fstream output_file_stream;
            public:
                FileMetricBackend(const std::string& _backend_identity,
                                  const std::string& _file_path,
                                  const std::string& _file_extension,
                                  std::ios_base::openmode _open_mode);
                ~FileMetricBackend();
                void flush();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__FileMetricBackend__) */
