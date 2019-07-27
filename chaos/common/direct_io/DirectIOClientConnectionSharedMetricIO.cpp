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

#include <chaos/common/direct_io/DirectIOClientConnectionSharedMetricIO.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/format.hpp>

using namespace chaos::common::direct_io;

#define DIOCCSMC_INFO INFO_LOG(DirectIOClientConnectionSharedMetricIO)
#define DIOCCSMC_DBG_ DBG_LOG(DirectIOClientConnectionSharedMetricIO)
#define DIOCCSMC_ERR_ ERR_LOG(DirectIOClientConnectionSharedMetricIO)

DirectIOClientConnectionSharedMetricIO::DirectIOClientConnectionSharedMetricIO(const std::string& client_impl,
                                                                               const std::string& server_endpoint):
MetricCollectorIO() {
    DIOCCSMC_DBG_ << "Allocate collector";
}

DirectIOClientConnectionSharedMetricIO::~DirectIOClientConnectionSharedMetricIO() {

}

void DirectIOClientConnectionSharedMetricIO::incrementPackCount() {
    pack_count++;
}

//increment packet size
void DirectIOClientConnectionSharedMetricIO::incrementBandWidth(uint64_t increment) {
    bandwith+=increment;
}

void DirectIOClientConnectionSharedMetricIO::fetchMetricForTimeDiff(uint64_t time_diff) {
   
}
