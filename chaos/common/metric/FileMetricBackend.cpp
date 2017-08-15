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

#include <chaos/common/global.h>
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
