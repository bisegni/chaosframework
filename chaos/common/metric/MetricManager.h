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


#ifndef MetricManager_h
#define MetricManager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/InizializableService.h>

#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/counter.h>
#include <prometheus/gauge.h>
namespace chaos {
    namespace common {
        namespace metric {
            class MetricManager;
            
            /*!
             Counter abstraction
             */
            class CCounter {
                friend class MetricManager;
                prometheus::Counter& impl;
                CCounter(prometheus::Counter& _impl);
            public:
                double operator()();
                prometheus::Counter& operator++();
                prometheus::Counter& operator++(int);
                double operator+(const double d) const;
                CCounter& operator+=(const double d);
            };
            
            /*!
             Gauge abstraction
             */
            class CGauge {
                friend class MetricManager;
                prometheus::Gauge& impl;
                
                CGauge(prometheus::Gauge& _impl);
            public:
                double operator()();
                prometheus::Gauge& operator--();
                prometheus::Gauge& operator++();
                prometheus::Gauge& operator--(int);
                prometheus::Gauge& operator++(int);
                double operator+(const double d) const;
                double operator-(const double d) const;
                double operator=(const double d) const;
                CGauge& operator+=(const double d);
                CGauge& operator-=(const double d);
            };
            
            typedef ChaosUniquePtr<CCounter> CounterUniquePtr;
            typedef ChaosUniquePtr<CGauge> GaugeUniquePtr;
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, prometheus::Family<prometheus::Counter>&, MapFamilyCounter);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapFamilyCounter, LMapFamilyCounter);
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, prometheus::Family<prometheus::Gauge>&, MapFamilyGauge);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapFamilyGauge, LMapFamilyGauge);
            /*!
             Central managment for metric exposition to Prometheus
             */
            class MetricManager:
            public common::utility::InizializableService,
            public chaos::common::utility::Singleton<MetricManager> {
                friend class chaos::common::utility::Singleton<MetricManager>;
                //main structure
                ChaosUniquePtr<prometheus::Exposer>     http_exposer;
                ChaosSharedPtr<prometheus::Registry>    metrics_registry;
                
                LMapFamilyCounter    map_counter;
                LMapFamilyGauge      map_gauge;
                
                //io base metrics family for data and packet rate
                prometheus::Family<prometheus::Counter>& io_send_byte_sec;
                prometheus::Family<prometheus::Counter>& io_send_packet_sec;
                prometheus::Family<prometheus::Counter>& io_receive_byte_sec;
                prometheus::Family<prometheus::Counter>& io_receive_packet_sec;
                
                
                
                MetricManager();
                ~MetricManager();
            protected:
                void init(void *data);
                void deinit();
            public:
                
                CounterUniquePtr getNewTxDataRateMetricFamily(const std::map<std::string,std::string>& label);
                CounterUniquePtr getNewRxDataRateMetricFamily(const std::map<std::string,std::string>& label);
                CounterUniquePtr getNewTxPacketRateMetricFamily(const std::map<std::string,std::string>& label);
                CounterUniquePtr getNewRxPacketRateMetricFamily(const std::map<std::string,std::string>& label);
                
                void createCounterFamily(const std::string& name,
                                         const std::string& desc);
                
                void createGaugeFamily(const std::string& name,
                                       const std::string& desc);
                
                CounterUniquePtr getNewCounterFromFamily(const std::string& family_name,
                                                         const std::map<std::string,std::string>& label = {});
                
                GaugeUniquePtr getNewGaugeFromFamily(const std::string& family_name,
                                                     const std::map<std::string,std::string>& label = {});
                
            };
        }
    }
}

#endif /* MetricManager_hpp */
