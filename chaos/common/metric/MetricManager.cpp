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

using namespace prometheus;
#pragma mark Counter
chaos::common::metric::Counter::Counter(prometheus::Counter& _impl):
impl(_impl){}

double chaos::common::metric::Counter::operator()() {
    return impl.Value();
}
prometheus::Counter& chaos::common::metric::Counter::operator++() {
    impl.Increment();
    return impl;
}
prometheus::Counter& chaos::common::metric::Counter::operator++(int) {
    impl.Increment();
    return impl;
}
double chaos::common::metric::Counter::operator+(const double d) const {
    impl.Increment(d);
    return impl.Value();
}
chaos::common::metric::Counter& chaos::common::metric::Counter::operator+=(const double d) {
    impl.Increment(d);
    return *this;
}
#pragma mark Gauge
chaos::common::metric::Gauge::Gauge(prometheus::Gauge& _impl):
impl(_impl){}

double chaos::common::metric::Gauge::operator()() {
    return impl.Value();
}

prometheus::Gauge& chaos::common::metric::Gauge::operator--() {
    impl.Decrement();
    return impl;
}

prometheus::Gauge& chaos::common::metric::Gauge::operator++() {
    impl.Increment();
    return impl;
}

prometheus::Gauge& chaos::common::metric::Gauge::operator--(int) {
    impl.Decrement();
    return impl;
}

prometheus::Gauge& chaos::common::metric::Gauge::operator++(int) {
    impl.Increment();
    return impl;
}
double chaos::common::metric::Gauge::operator+(const double d) const {
    impl.Increment(d);
    return impl.Value();
}
double chaos::common::metric::Gauge::operator-(const double d) const {
    impl.Decrement(d);
    return impl.Value();
}

#pragma mark Manager
MetricManager::MetricManager():
http_exposer(new prometheus::Exposer("127.0.0.1:8080")),
metrics_registry(std::make_shared<Registry>()),
io_send_byte_sec(BuildGauge()
                 .Name("io_send_byte_sec")
                 .Help("The data rate of transmitted byte/sec out of data service")
                 .Labels({})
                 .Register(*metrics_registry)),
io_send_packet_sec(BuildGauge()
                   .Name("io_send_packet_sec")
                   .Help("The data rate of transmitted packet out of data service")
                   .Labels({})
                   .Register(*metrics_registry)),
io_receive_byte_sec(BuildGauge()
                    .Name("io_receive_receive_sec")
                    .Help("The data rate of received byte/sec out of data service")
                    .Labels({})
                    .Register(*metrics_registry)),
io_receive_packet_sec(BuildGauge()
                      .Name("io_receive_packet_sec")
                      .Help("The data rate of received packet out of data service")
                      .Labels({})
                      .Register(*metrics_registry)){}

MetricManager::~MetricManager() {}

void MetricManager::init(void *data) {
    
}

void MetricManager::deinit() {
    metrics_registry.reset();
    http_exposer.reset();
}

GaugeUniquePtr MetricManager::getNewTxDataRateMetricFamily(const std::map<std::string,std::string>& label) {
    return GaugeUniquePtr(new chaos::common::metric::Gauge(io_send_byte_sec.Add(label)));
}

GaugeUniquePtr MetricManager::getNewRxDataRateMetricFamily(const std::map<std::string,std::string>& label) {
    return GaugeUniquePtr(new chaos::common::metric::Gauge(io_receive_byte_sec.Add(label)));
}

GaugeUniquePtr MetricManager::getNewTxPacketRateMetricFamily(const std::map<std::string,std::string>& label) {
    return GaugeUniquePtr(new chaos::common::metric::Gauge(io_send_packet_sec.Add(label)));
}

GaugeUniquePtr MetricManager::getNewRxPacketRateMetricFamily(const std::map<std::string,std::string>& label) {
    return GaugeUniquePtr(new chaos::common::metric::Gauge(io_receive_packet_sec.Add(label)));
}

void MetricManager::createCounterFamily(const std::string& name,
                                        const std::string& desc) {
    LMapFamilyCounterWriteLock wl = map_counter.getWriteLockObject();
    map_counter().insert(MapFamilyCounterPair(name,
                                              BuildCounter()
                                              .Name(name)
                                              .Help(desc)
                                              .Labels({})
                                              .Register(*metrics_registry)));
}

void MetricManager::createGaugeFamily(const std::string& name,
                                      const std::string& desc) {
    LMapFamilyGaugeWriteLock wl = map_gauge.getWriteLockObject();
    map_gauge().insert(MapFamilyGaugePair(name,
                                          BuildGauge()
                                          .Name(name)
                                          .Help(desc)
                                          .Labels({})
                                          .Register(*metrics_registry)));
}

CounterUniquePtr MetricManager::getNewCounterFromFamily(const std::string& family_name,
                                                        const std::map<std::string,std::string>& label) {
    LMapFamilyCounterReadLock wl = map_counter.getReadLockObject();
    MapFamilyCounterIterator i = map_counter().find(family_name);
    if(i == map_counter().end()) return CounterUniquePtr();
    return CounterUniquePtr(new chaos::common::metric::Counter(i->second.Add(label)));
}

GaugeUniquePtr MetricManager::getNewGaugeFromFamily(const std::string& family_name,
                                                    const std::map<std::string,std::string>& label) {
    LMapFamilyGaugeReadLock wl = map_gauge.getReadLockObject();
    MapFamilyGaugeIterator i = map_gauge().find(family_name);
    if(i == map_gauge().end()) return GaugeUniquePtr();
    return GaugeUniquePtr(new chaos::common::metric::Gauge(i->second.Add(label)));
}
