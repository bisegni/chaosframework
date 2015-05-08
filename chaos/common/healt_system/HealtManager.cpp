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
#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::healt_system;
using namespace chaos::common::async_central;

#define HM_INFO INFO_LOG(HealtMetric)
#define HM_DBG DBG_LOG(HealtMetric)
#define HM_ERR ERR_LOG(HealtMetric)

#define HEALT_NEED_NODE_NO_METRIC_PRESENCE(n,m)\
if(map_node.count(n) == 0) return;\
if(map_node[n]->map_metric.count(m) != 0) return;

#define HEALT_NEED_NODE_AND_METRIC_PRESENCE(n,m)\
if(map_node.count(n) == 0) return;\
if(map_node[n]->map_metric.count(m) == 0) return;

#define HEALT_SET_METRIC_VALUE(node_metrics_ptr, t, m, v)\
t *tmp = dynamic_cast<t*>(node_metrics_ptr->map_metric[m].get());\
if(tmp)tmp->value = v;

#define HEALT_SET_METRIC_TIMESTAMP(node_metrics_ptr)\
Int64HealtMetric *ts_tmp = dynamic_cast<Int64HealtMetric*>(node_metrics_ptr->map_metric[NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP].get());\
if(ts_tmp)ts_tmp->value = TimingUtil::getTimeStamp();

HealtManager::HealtManager():
network_broker_ptr(NULL),
mds_message_channel(NULL){

}
HealtManager::~HealtManager() {

}

void HealtManager::setNetworkBroker(chaos::common::network::NetworkBroker *_network_broker) {
    network_broker_ptr = _network_broker;
}
void HealtManager::init(void *init_data) throw (chaos::CException) {
    if(network_broker_ptr == NULL) throw CException(-1, "No NetworkBroker has been set", __PRETTY_FUNCTION__);

    std::string impl_name = boost::str(boost::format("%1%IODriver") %
                                       GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL));
    HM_INFO << "Allocating data driver " << impl_name;

    mds_message_channel = network_broker_ptr->getMultiMetadataServiceRawMessageChannel();
    if(mds_message_channel == NULL) throw CException(-2, "Unalbe to get metadata server channel", __PRETTY_FUNCTION__);

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

void HealtManager::sayHello() throw (chaos::CException) {
    int retry = 0;
    bool saying_hello = true;
    HM_INFO << "Start hello phase throward metadata services";
    CDataWrapper *hello_pack = new CDataWrapper();
        //add node id
    hello_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID,
                               GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort());
    hello_pack->addStringValue(NodeDefinitionKey::NODE_TYPE,
                               NodeType::NODE_TYPE_HEALT_PROCESS);
    hello_pack->addInt64Value(NodeDefinitionKey::NODE_TIMESTAMP,
                              TimingUtil::getTimeStamp());
    std::auto_ptr<MultiAddressMessageRequestFuture> future = mds_message_channel->sendRequestWithFuture(HealtProcessDomainAndActionRPC::RPC_DOMAIN,
                                                                                                        HealtProcessDomainAndActionRPC::ACTION_PROCESS_WELCOME,
                                                                                                        hello_pack,
                                                                                                        1000);
    do{
        if(future->wait()) {
            saying_hello = false;
            if(future->getError()) {
                throw CException(future->getError(),
                                 future->getErrorMessage(),
                                 future->getErrorDomain());
            } else if(!future->getResult()) {
                    //we have received no result or no server list
                throw CException(-1,
                                 "Empty answer from hello message",
                                 __PRETTY_FUNCTION__);
            } else {
                if(!future->getResult()->hasKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST)) {
                        //we have received no result or no server list
                    throw CException(-2,
                                     "No server list on hello answer",
                                     __PRETTY_FUNCTION__);
                } else {
                        //we have result and need to update the driver
                    io_data_driver->updateConfiguration(future->getResult());
                }
            }
        } else {
            if(retry++ >= 3) {
                throw CException(-3,
                                 "Exiceed the maximum number of retry to wait the answer",
                                 __PRETTY_FUNCTION__);
            } else {
                HM_INFO << "Retry waiting answer";
            }
        }
    }while(saying_hello);
}

void HealtManager::start() throw (chaos::CException) {
        //say hello to mds
    for(int retry = 0;
        retry < 3;
        retry ++) {
        try{
            sayHello();
        } catch(chaos::CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
            continue;
        }
        break;
    }

        //add timer to publish all node healt very 5 second
    AsyncCentralManager::getInstance()->addTimer(this, 0, 5000);
}

void HealtManager::stop() throw (chaos::CException) {
        //add timer to publish all node healt very 5 second
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void HealtManager::deinit() throw (chaos::CException) {
    if(io_data_driver.get()) {
        io_data_driver->deinit();
        io_data_driver.reset();
    }

    if(mds_message_channel) {
        network_broker_ptr->disposeMessageChannel(mds_message_channel);
        mds_message_channel = NULL;
    }
}

void HealtManager::addNewNode(const std::string& node_uid) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    if(map_node.count(node_uid) != 0) return;
        //add new node in map
    map_node.insert(make_pair(node_uid, boost::shared_ptr<NodeHealtSet>(new NodeHealtSet(node_uid))));

        //add default standard metric
    map_node[node_uid]->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP,
                                                    boost::shared_ptr<HealtMetric>(new Int64HealtMetric(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP))));
    map_node[node_uid]->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                    boost::shared_ptr<HealtMetric>(new StringHealtMetric(NodeHealtDefinitionKey::NODE_HEALT_STATUS))));
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
    boost::shared_lock<boost::shared_mutex> rl_map(map_node_mutex);

        //we need node but no metric
    HEALT_NEED_NODE_NO_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
    boost::shared_ptr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        //unlock map
    rl_map.unlock();

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
    node_metrics_ptr->map_metric.insert(make_pair(node_metric, metric));
}

void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      int32_t int32_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)

        // read lock
    boost::shared_ptr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];

        //
    wl.unlock();

        //work on local shared pointer
    HEALT_SET_METRIC_TIMESTAMP(node_metrics_ptr)
    HEALT_SET_METRIC_VALUE(node_metrics_ptr, Int32HealtMetric, node_metric, int32_value);
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      int64_t int64_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
    boost::shared_ptr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
    wl.unlock();
        //work on local shared pointer
    HEALT_SET_METRIC_TIMESTAMP(node_metrics_ptr)
    HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                           Int64HealtMetric,
                           node_metric,
                           int64_value);
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      double double_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
    boost::shared_ptr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
    wl.unlock();
    HEALT_SET_METRIC_TIMESTAMP(node_metrics_ptr)
    HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                           DoubleHealtMetric,
                           node_metric,
                           double_value);
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      const std::string& str_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
    boost::shared_ptr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
    wl.unlock();
    HEALT_SET_METRIC_TIMESTAMP(node_metrics_ptr)
    HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                           StringHealtMetric,
                           node_metric,
                           str_value);
}

void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      const bool bool_value) {
    boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
    HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
    boost::shared_ptr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
    wl.unlock();
    HEALT_SET_METRIC_TIMESTAMP(node_metrics_ptr)
    HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                           BoolHealtMetric,
                           node_metric,
                           bool_value);
}

void HealtManager::prepareNodeDataPack(HealtNodeElementMap& element_map,
                                       chaos::common::data::CDataWrapper& node_data_pack) {
    BOOST_FOREACH(HealtNodeElementMap::value_type map_metric_element, element_map) {
            //add metric to cdata wrapper
        map_metric_element.second->addMetricToCD(node_data_pack);
    }
}

    //publish the healt for the ndoe uid
void HealtManager::publishNodeHealt(const std::string& node_uid) {
    if(map_node.count(node_uid) == 0) return;

        //allocate the datapack
    std::auto_ptr<CDataWrapper> data_pack(new CDataWrapper());
        //compose datapack
    prepareNodeDataPack(map_node[node_uid]->map_metric,
                        *data_pack.get());
        //send datapack
    io_data_driver->storeData(map_node[node_uid]->node_key,
                              data_pack.release());
}

void HealtManager::timeout() {
        //publish all registered metric
    boost::shared_lock<boost::shared_mutex> rl(map_node_mutex);
    for(HealtNodeMapIterator it = map_node.begin();
        it != map_node.end();
        it++) {
            //publish single node
        publishNodeHealt(it->first);
    }
}