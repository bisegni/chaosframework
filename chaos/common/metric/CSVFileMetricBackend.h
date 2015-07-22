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

#ifndef __CHAOSFramework__CSVFileMetricBackend__
#define __CHAOSFramework__CSVFileMetricBackend__

#include <chaos/common/metric/FileMetricBackend.h>

namespace chaos {
    namespace common {
        namespace metric {
            class CVSFileMetricBackend:
            public FileMetricBackend {
            protected:
                const boost::filesystem::path file_path;
                boost::filesystem::fstream output_file_stream;
            public:
                CVSFileMetricBackend(const std::string& _backend_identity,
                                  const std::string& _file_path,
                                  std::ios_base::openmode _open_mode);
                ~CVSFileMetricBackend();
                void addMetric(const std::string& metric_name,
                                       const std::string& metric_value);
                void prepare();
                void flush();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__CSVFileMetricBackend__) */
