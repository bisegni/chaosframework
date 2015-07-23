/*
 *	FileMetricBackend.cpp
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

#include <chaos/common/metric/FileMetricBackend.h>

#include <boost/format.hpp>
using namespace chaos::common::metric;

#define FILE_MB_INFO INFO_LOG(FileMetricBackend)
#define FILE_MB_LDBG DBG_LOG(FileMetricBackend)
#define FILE_MB_LERR ERR_LOG(FileMetricBackend)

FileMetricBackend::FileMetricBackend(const std::string& _backend_identity,
                                     const std::string& _file_path,
                                     const std::string& _file_extension,
                                     std::ios_base::openmode _open_mode):
AbstractMetricBackend(_backend_identity),
file_path(_file_path){
//open file
    CHAOS_ASSERT(boost::filesystem::create_directories(file_path) || boost::filesystem::exists(file_path))
    boost::filesystem::path file = file_path;
    file /= _backend_identity;
    file.replace_extension(_file_extension);
    
    output_file_stream.open(file, _open_mode);
}

FileMetricBackend::~FileMetricBackend() {
    output_file_stream.close();
}

void FileMetricBackend::flush() {
    output_file_stream.flush();
}