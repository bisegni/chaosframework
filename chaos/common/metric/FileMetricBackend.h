/*
 *	FileMetricBackend.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
