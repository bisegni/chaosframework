/*
 *	CVSFileMetricBackend.cpp
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

#include <chaos/common/metric/CSVFileMetricBackend.h>

using namespace chaos::common::metric;

CVSFileMetricBackend::CVSFileMetricBackend(const std::string& _backend_identity,
                                           const std::string& _file_path,
                                           bool append):
FileMetricBackend(_backend_identity,
                  _file_path,
                  ".csv",
                  (append?ios::app:ios::trunc)|ios::out){
    
}

CVSFileMetricBackend::~CVSFileMetricBackend() {
    
}

void CVSFileMetricBackend::prepare() {
    first_metric = true;
}

void CVSFileMetricBackend::preMetric() {
    if(first_metric) return;
    output_stream << ",";
}

void CVSFileMetricBackend::addMetric(const std::string& metric_name,
                                     const std::string& metric_value) {
    output_stream << metric_name << ":" << metric_value;
    first_metric = false;

}


void CVSFileMetricBackend::postMetric() {
    
}


void CVSFileMetricBackend::flush() {
    //write to file
    output_file_stream << output_stream.str();
}