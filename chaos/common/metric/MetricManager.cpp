/*
 * Copyright 2012, 2019 INFN
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


#include <chaos/common/metric/MetricManager.h>

using namespace chaos::common::metric;

MetricManager::MetricManager():
http_exposer(){}

MetricManager::~MetricManager() {}

void MetricManager::init(void *data) {
    http_exposer.reset(new prometheus::Exposer("127.0.0.1:8080"));
}

void MetricManager::deinit() {
    http_exposer.reset();
}
