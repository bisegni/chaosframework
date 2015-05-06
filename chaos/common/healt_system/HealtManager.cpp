/*
 *	HealtManager.cpp
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
#include <chaos/common/global.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/format.hpp>

using namespace chaos::common::io;
using namespace chaos::common::utility;
using namespace chaos::common::healt_system;

#define HM_INFO INFO_LOG(HealtMetric)
#define HM_DBG DBG_LOG(HealtMetric)
#define HM_ERR ERR_LOG(HealtMetric)

#define HEALT_NEED_NODE_NO_METRIC_PRESENCE(n,m)\
if(map_node.count(n) == 0) return;\
if(map_node[n].count(m) != 0) return;

#define HEALT_NEED_NODE_AND_METRIC_PRESENCE(n,m)\
if(map_node.count(n) == 0) return;\
if(map_node[n].count(m) == 0) return;

#define HEALT_SET_METRIC_VALUE(t, n, m, v)\
t *tmp = dynamic_cast<t*>(map_node[n][m].get());\
if(tmp)tmp->value = v;


    //----------------------metric element-------------------------
HealtMetric::HealtMetric(const std::string& _name,
                         const chaos::DataType::DataType _type):
name(_name),
type(_type){}
void HealtMetric::addMetricToCD(chaos::common::data::CDataWrapper *data) {
    if(data) addMetricToCD(*data);
}


BoolHealtMetric::BoolHealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_BOOLEAN){}
void BoolHealtMetric::addMetricToCD(chaos::common::data::CDataWrapper& data) {
    data.addBoolValue(name.c_str(), value);
}

Int32HealtMetric::Int32HealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_INT32){}
void Int32HealtMetric::addMetricToCD(chaos::common::data::CDataWrapper& data) {
    data.addInt32Value(name, value);
}

Int64HealtMetric::Int64HealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_INT64){}
void Int64HealtMetric::addMetricToCD(chaos::common::data::CDataWrapper& data) {
    data.addInt64Value(name, value);
}


DoubleHealtMetric::DoubleHealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_DOUBLE){}
void DoubleHealtMetric::addMetricToCD(chaos::common::data::CDataWrapper& data) {
    data.addDoubleValue(name, value);
}

StringHealtMetric::StringHealtMetric(const std::string& _name):
HealtMetric(_name,
            chaos::DataType::TYPE_STRING){}
void StringHealtMetric::addMetricToCD(chaos::common::data::CDataWrapper& data) {
    data.addStringValue(name, value);
}
    //----------------------metric element-------------------------

void HealtManager::setNetworkBroker(chaos::common::network::NetworkBroker *_network_broker) {
    network_broker_ptr = _network_broker;
}
void HealtManager::init(void *init_data) throw (chaos::CException) {
     if(network_broker_ptr == NULL) throw CException(-1, "No NetworkBroker has been set", __PRETTY_FUNCTION__);

    std::string impl_name = boost::str(boost::format("%1%IODriver") %
                                       GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL));
    HM_INFO << "Allocating data driver " << impl_name;

    io_data_driver.reset(common::utility::ObjectFactoryRegister<IODataDriver>::getInstance()->getNewInstanceByName(impl_name));
    if(io_data_driver.get()) {
        if(impl_name.compare("IODirectIODriver") == 0) {
                //set the information
            IODirectIODriverInitParam init_param;
            std::memset(&init_param, 0, sizeof(IODirectIODriverInitParam));
                //get client and endpoint
            init_param.network_broker = network_broker_ptr;
            init_param.client_instance = NULL;
            init_param.endpoint_instance = NULL;
            ((IODirectIODriver*)io_data_driver.get())->setDirectIOParam(init_param);
        }
    } else {
        throw CException(-2, "No IO data driver has been found", __PRETTY_FUNCTION__);
    }

    io_data_driver->init(NULL);
}

void HealtManager::start() throw (chaos::CException) {

}

void HealtManager::stop() throw (chaos::CException) {

}

void HealtManager::deinit() throw (chaos::CException) {
    io_data_driver->deinit();
}

void HealtManager::addNewNode(const std::string& node_uid) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    if(map_node.count(node_uid) != 0) return;
        //add new node in map
    map_node.insert(make_pair(node_uid, HealtNodeElementMap()));
}

void HealtManager::removeNode(const std::string& node_uid) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    if(map_node.count(node_uid) == 0) return;
        //remove node from map
    map_node.erase(node_uid);
}

void HealtManager::addNodeMetric(const std::string& node_uid,
                                 const std::string& node_metric,
                                 chaos::DataType::DataType metric_type) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);

        //we need node but no metric
    HEALT_NEED_NODE_NO_METRIC_PRESENCE(node_uid, node_metric)

    boost::shared_ptr<HealtMetric> metric;
    switch(metric_type) {
        case chaos::DataType::TYPE_BOOLEAN:
            break;
        case chaos::DataType::TYPE_INT32:
            metric.reset(new Int32HealtMetric(node_metric));
            break;
        case chaos::DataType::TYPE_INT64:
            metric.reset(new Int64HealtMetric(node_metric));
            break;
        case chaos::DataType::TYPE_DOUBLE:
            metric.reset(new DoubleHealtMetric(node_metric));
            break;
        case chaos::DataType::TYPE_STRING:
            metric.reset(new StringHealtMetric(node_metric));
            break;

        default:
            break;
    }
        //add metric
    map_node[node_uid].insert(make_pair(node_metric, metric));

}

void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      int32_t int32_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
    HEALT_SET_METRIC_VALUE(Int32HealtMetric, node_uid, node_metric, int32_value);
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      int64_t int64_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
    HEALT_SET_METRIC_VALUE(Int64HealtMetric, node_uid, node_metric, int64_value);
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      double double_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
    HEALT_SET_METRIC_VALUE(DoubleHealtMetric, node_uid, node_metric, double_value);
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      const std::string& str_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
    HEALT_SET_METRIC_VALUE(StringHealtMetric, node_uid, node_metric, str_value);
}

void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      const bool bool_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
    HEALT_SET_METRIC_VALUE(BoolHealtMetric, node_uid, node_metric, bool_value);
}

void HealtManager::timeout() {

}