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

#include <chaos/common/metric/CSVFileMetricBackend.h>

#include <fstream>

using namespace chaos::common::metric;

CVSFileMetricBackend::CVSFileMetricBackend(const std::string& _backend_identity,
                                           const std::string& _file_path,
                                           bool append):
FileMetricBackend(_backend_identity,
                  _file_path,
                  ".csv",
                  (append?std::fstream::app:std::fstream::trunc)| std::fstream::out){

}

CVSFileMetricBackend::~CVSFileMetricBackend() {

}

void CVSFileMetricBackend::prepare(uint64_t metric_acquire_ts) {
    output_stream << metric_acquire_ts;
}

void CVSFileMetricBackend::preMetric() {
    output_stream << ",";
}

void CVSFileMetricBackend::addMetric(const std::string& metric_name,
                                     const std::string& metric_value) {
    output_stream << metric_value;
}


void CVSFileMetricBackend::postMetric() {

}


void CVSFileMetricBackend::flush() {
    //write to file
    output_stream << std::endl;
    output_file_stream << output_stream.str();
    FileMetricBackend::flush();
    output_stream.clear();
    output_stream.str("");
}
