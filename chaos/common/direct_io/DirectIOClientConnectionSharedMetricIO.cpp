/*
 *	DirectIOClientConnectionMetricCollector.cpp
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

#include <chaos/common/direct_io/DirectIOClientConnectionSharedMetricIO.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/format.hpp>

using namespace chaos::common::direct_io;

DirectIOClientConnectionSharedMetricIO::DirectIOClientConnectionSharedMetricIO(const std::string& client_impl,
                                                                               const std::string& server_endpoint):
MetricCollectorIO(client_impl,
                  GlobalConfiguration::getInstance()->getConfiguration()->getUInt64Value(InitOption::OPT_DIRECT_IO_LOG_METRIC_UPDATE_INTERVAL)) {
    
    //add backend
    addBackend(metric::MetricBackendPointer(new metric::ConsoleMetricBackend(boost::str(boost::format("%1%_%2%")%client_impl%server_endpoint))));
    
    startLogging();
}

DirectIOClientConnectionSharedMetricIO::~DirectIOClientConnectionSharedMetricIO() {
    stopLogging();
}

void DirectIOClientConnectionSharedMetricIO::incrementPackCount() {
    pack_count++;
}

//increment packet size
void DirectIOClientConnectionSharedMetricIO::incrementBandWidth(uint64_t increment) {
    bandwith+=increment;
}

void DirectIOClientConnectionSharedMetricIO::fetchMetricForTimeDiff(uint64_t time_diff) {
    MetricCollectorIO::fetchMetricForTimeDiff(time_diff);
}