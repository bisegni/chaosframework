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
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::network;
using namespace chaos::common::healt_system;
using namespace chaos::common::async_central;

#define HM_INFO INFO_LOG(HealtManager)
#define HM_DBG DBG_LOG(HealtManager)
#define HM_ERR ERR_LOG(HealtManager)

#define HEALT_NEED_NODE_NO_METRIC_PRESENCE(n,m)\
if(map_node.count(n) == 0) return;\
if(map_node[n]->map_metric.count(m) != 0) return;

#define HEALT_NEED_NODE_AND_METRIC_PRESENCE(n,m)\
if(map_node.count(n) == 0) return;\
if(map_node[n]->map_metric.count(m) == 0) return;

#define HEALT_SET_METRIC_VALUE(node_metrics_ptr, t, m, v)\
t *tmp = static_cast<t*>(node_metrics_ptr->map_metric[m].get());\
if(tmp)tmp->value = v;

#define HEALT_SET_METRIC_TIMESTAMP_LAST_METRIC(node_metrics_ptr)\
Int64HealtMetric *ts_tmp = static_cast<Int64HealtMetric*>(node_metrics_ptr->map_metric[NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP_LAST_METRIC].get());\
ts_tmp->value = TimingUtil::getTimeStamp();

#define HEALT_FIRE_TIMEOUT 6 //timeout for next healt fire for every registered node
#define HEALT_FIRE_SLOTS 3 //define slot by a seconds


#pragma mark SendHealthStatAsyncJob
SendHealthStatAsyncJob::SendHealthStatAsyncJob(chaos::common::data::CDataWrapper& health_stat):
AsyncRunnable("SendHealthStatAsyncJob"){
    //NetworkBroker::getInstance()->getMetadataserverMessageChannel()
}

SendHealthStatAsyncJob::~SendHealthStatAsyncJob() {
    
}

void SendHealthStatAsyncJob::run() {
    
}
#pragma mark HealtManager
HealtManager::HealtManager():
mds_message_channel(NULL),
last_fire_counter_set(0),
current_fire_slot(0),
last_usr_time(0),
last_sys_time(0),
last_total_time(0),
last_sampling_time(0){}

HealtManager::~HealtManager() {
    //clear node map
    map_node.clear();
}

void HealtManager::updateProcInfo() {
    //used to compute the resource occupaiton between sampling time
    struct rusage local_process_resurce_usage;
    double local_usr_time = 0;
    double local_sys_time = 0;
    //samplig time in seconds
    uint64_t sampling_time = TimingUtil::getTimeStamp()/1000;
    getrusage(RUSAGE_SELF, &local_process_resurce_usage);
    local_usr_time = (double)local_process_resurce_usage.ru_utime.tv_sec + ((double)local_process_resurce_usage.ru_utime.tv_usec / 1000000.0);
    local_sys_time = (double)local_process_resurce_usage.ru_stime.tv_sec + ((double)local_process_resurce_usage.ru_stime.tv_usec / 1000000.0);
    if(last_usr_time &&
       last_sys_time &&
       last_sampling_time) {
        uint64_t temp_ts = (sampling_time-last_sampling_time);
        if(temp_ts > 0) {
            current_proc_info.usr_time = 100*(local_usr_time-last_usr_time)/(double)temp_ts;
            current_proc_info.sys_time = 100*(local_sys_time-last_sys_time)/(double)temp_ts;
        }else {
            current_proc_info.usr_time = current_proc_info.sys_time = 0;
        }
        
    }
    last_usr_time = local_usr_time;
    last_sys_time = local_sys_time;
    last_sampling_time = sampling_time;
    current_proc_info.swap_rsrc = local_process_resurce_usage.ru_nswap;
}


void HealtManager::init(void *init_data) throw (chaos::CException) {
    
    std::string impl_name = boost::str(boost::format("%1%IODriver") %
                                       GlobalConfiguration::getInstance()->getOption<std::string>(InitOption::OPT_DATA_IO_IMPL));
    HM_INFO << "Allocating data driver " << impl_name;
    
    mds_message_channel = NetworkBroker::getInstance()->getMultiMetadataServiceRawMessageChannel();
    if(mds_message_channel == NULL) throw CException(-2, "Unalbe to get metadata server channel", __PRETTY_FUNCTION__);
    
    io_data_driver.reset(common::utility::ObjectFactoryRegister<IODataDriver>::getInstance()->getNewInstanceByName(impl_name));
    if(io_data_driver.get()) {
        if(impl_name.compare("IODirectIODriver") == 0) {
            //set the information
            IODirectIODriverInitParam init_param;
            std::memset(&init_param, 0, sizeof(IODirectIODriverInitParam));
            init_param.client_instance = NULL;
            init_param.endpoint_instance = NULL;
            ((IODirectIODriver*)io_data_driver.get())->setDirectIOParam(init_param);
        }
    } else {
        throw CException(-2, "No IO data driver has been found", __PRETTY_FUNCTION__);
    }
    
    io_data_driver->init(NULL);
}

int HealtManager::sayHello() throw (chaos::CException) {
    int retry = 0;
    bool saying_hello = true;
    HM_INFO << "Start hello, searching metadata services";
    CDataWrapper *hello_pack = new CDataWrapper();
    //add node id
    hello_pack->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID,
                               GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort());
    hello_pack->addStringValue(NodeDefinitionKey::NODE_TYPE,
                               NodeType::NODE_TYPE_HEALT_PROCESS);
    hello_pack->addInt64Value(NodeDefinitionKey::NODE_TIMESTAMP,
                              TimingUtil::getTimeStamp());
    ChaosUniquePtr<MultiAddressMessageRequestFuture> future = mds_message_channel->sendRequestWithFuture(HealtProcessDomainAndActionRPC::RPC_DOMAIN,
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
                    return 0;
                }
            }
        } else {
            if(retry++ >= 10) {
                throw CException(-3,
                                 "Exceed the maximum number of retry  to wait the answer",
                                 __PRETTY_FUNCTION__);
            } else {
                HM_INFO << "Retry waiting answer ("<<retry<<")";
                
            }
        }
        sleep(1);
    }while(saying_hello);
    return -1;
}

void HealtManager::start() throw (chaos::CException) {
    //say hello to mds
    int32_t retry =HELLO_PHASE_RETRY;
    while(retry--){
        try{
            if(sayHello()==0){
                HM_INFO << "Found ("<<retry<<")";
                //add timer to publish all node healt very 5 second
                AsyncCentralManager::getInstance()->addTimer(this, 0, (HEALT_FIRE_TIMEOUT / HEALT_FIRE_SLOTS)*1000);
                return;
            }
        } catch(chaos::CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex);
        }
        HM_INFO << "Retry hello again ("<<retry<<")";
    }
    throw CException(-4, "Cannot find a valid MDS node" , __PRETTY_FUNCTION__);
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
        NetworkBroker::getInstance()->disposeMessageChannel(mds_message_channel);
        mds_message_channel = NULL;
    }
}

const ProcInfo& HealtManager::getLastProcInfo() {
    return current_proc_info;
}

void HealtManager::addNewNode(const std::string& node_uid) {
    
    CHAOS_BOOST_LOCK_WRAP_EXCEPTION(boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);, return;)
    if(map_node.count(node_uid) != 0) return;
    ChaosSharedPtr<NodeHealtSet> healt_metric = ChaosSharedPtr<NodeHealtSet>(new NodeHealtSet(node_uid));
    //add new node in map
    map_node.insert(make_pair(node_uid, healt_metric));
    
    //add default standard metric
    healt_metric->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP,
                                              ChaosSharedPtr<HealtMetric>(new Int64HealtMetric(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP))));
    healt_metric->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP_LAST_METRIC,
                                              ChaosSharedPtr<HealtMetric>(new Int64HealtMetric(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP_LAST_METRIC))));
    healt_metric->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                              ChaosSharedPtr<HealtMetric>(new StringHealtMetric(NodeHealtDefinitionKey::NODE_HEALT_STATUS))));
    healt_metric->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME,
                                              ChaosSharedPtr<HealtMetric>(new DoubleHealtMetric(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME))));
    healt_metric->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME,
                                              ChaosSharedPtr<HealtMetric>(new DoubleHealtMetric(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME))));
    healt_metric->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP,
                                              ChaosSharedPtr<HealtMetric>(new Int64HealtMetric(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP))));
    healt_metric->map_metric.insert(make_pair(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME,
                                              ChaosSharedPtr<HealtMetric>(new Int64HealtMetric(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME))));
    //reset the counter for publishing pushses
    healt_metric->fire_slot = (last_fire_counter_set++ % HEALT_FIRE_SLOTS);
    
    //print log info for newly created set
    HM_INFO << "Added new healt set for node :" << node_uid << " with push counter of " << healt_metric->fire_slot;
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
    try{
        boost::unique_lock<boost::shared_mutex> rl_map(map_node_mutex);
        
        //we need node but no metric
        HEALT_NEED_NODE_NO_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
        ChaosSharedPtr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        //unlock map
        rl_map.unlock();
        
        ChaosSharedPtr<HealtMetric> metric;
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
    }CHAOS_BOOST_LOCK_EXCEPTION_CACTH(lock_exception,)
}

void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      int32_t int32_value,
                                      bool publish) {
    try{
        boost::shared_lock<boost::shared_mutex> wl(map_node_mutex);
        HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        
        // read lock
        ChaosSharedPtr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        
        //
        wl.unlock();
        
        //work on local shared pointer
        HEALT_SET_METRIC_TIMESTAMP_LAST_METRIC(node_metrics_ptr)
        HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                               Int32HealtMetric,
                               node_metric,
                               int32_value);
        if(publish) {publishNodeHealt(node_uid);}
    }CHAOS_BOOST_LOCK_EXCEPTION_CACTH(lock_exception,)
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      int64_t int64_value,
                                      bool publish) {
    try{
        
        boost::shared_lock<boost::shared_mutex> wl(map_node_mutex);
        HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
        ChaosSharedPtr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
        wl.unlock();
        //work on local shared pointer
        HEALT_SET_METRIC_TIMESTAMP_LAST_METRIC(node_metrics_ptr)
        HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                               Int64HealtMetric,
                               node_metric,
                               int64_value);
        if(publish) {publishNodeHealt(node_uid);}
    }CHAOS_BOOST_LOCK_EXCEPTION_CACTH(lock_exception,)
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      double double_value,
                                      bool publish) {
    try{
        boost::shared_lock<boost::shared_mutex> wl(map_node_mutex);
        HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
        ChaosSharedPtr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
        wl.unlock();
        HEALT_SET_METRIC_TIMESTAMP_LAST_METRIC(node_metrics_ptr)
        HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                               DoubleHealtMetric,
                               node_metric,
                               double_value);
        if(publish) {publishNodeHealt(node_uid);}
    }CHAOS_BOOST_LOCK_EXCEPTION_CACTH(lock_exception,)
}
void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      const std::string& str_value,
                                      bool publish) {
    
    try{boost::shared_lock<boost::shared_mutex> wl(map_node_mutex);
        HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
        ChaosSharedPtr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
        wl.unlock();
        HEALT_SET_METRIC_TIMESTAMP_LAST_METRIC(node_metrics_ptr)
        HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                               StringHealtMetric,
                               node_metric,
                               str_value);
        if(publish) {publishNodeHealt(node_uid);}
    }CHAOS_BOOST_LOCK_EXCEPTION_CACTH(lock_exception,)
}

void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      const char * c_str_value,
                                      bool publish) {
    try{boost::shared_lock<boost::shared_mutex> wl(map_node_mutex);
        
        HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
        ChaosSharedPtr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
        wl.unlock();
        HEALT_SET_METRIC_TIMESTAMP_LAST_METRIC(node_metrics_ptr)
        HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                               StringHealtMetric,
                               node_metric,
                               c_str_value);
        if(publish) {publishNodeHealt(node_uid);}
    }CHAOS_BOOST_LOCK_EXCEPTION_CACTH(lock_exception,)
}

void HealtManager::addNodeMetricValue(const std::string& node_uid,
                                      const std::string& node_metric,
                                      const bool bool_value,
                                      bool publish) {
    
    try{boost::unique_lock<boost::shared_mutex> wl(map_node_mutex);
        
        HEALT_NEED_NODE_AND_METRIC_PRESENCE(node_uid, node_metric)
        // read lock
        ChaosSharedPtr<NodeHealtSet> node_metrics_ptr = map_node[node_uid];
        // unlock all map
        wl.unlock();
        HEALT_SET_METRIC_TIMESTAMP_LAST_METRIC(node_metrics_ptr)
        HEALT_SET_METRIC_VALUE(node_metrics_ptr,
                               BoolHealtMetric,
                               node_metric,
                               bool_value);
        if(publish) {publishNodeHealt(node_uid);}
    }CHAOS_BOOST_LOCK_EXCEPTION_CACTH(lock_exception,)
}

CDataWrapper*  HealtManager::prepareNodeDataPack(NodeHealtSet& node_health_set,
                                                 uint64_t push_timestamp) {
    CDataWrapper *node_data_pack = new CDataWrapper();
    
    if(node_data_pack) {
        //add device unique id
        node_data_pack->addStringValue(DataPackCommonKey::DPCK_DEVICE_ID, node_health_set.node_uid);
        //add dataset type
        node_data_pack->addInt32Value(DataPackCommonKey::DPCK_DATASET_TYPE, DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH);
        //set the push timestamp
        static_cast<Int64HealtMetric*>(node_health_set.map_metric[NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP].get())->value = push_timestamp;
        
        static_cast<Int64HealtMetric*>(node_health_set.map_metric[NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP].get())->value = current_proc_info.swap_rsrc;
        
        static_cast<DoubleHealtMetric*>(node_health_set.map_metric[NodeHealtDefinitionKey::NODE_HEALT_USER_TIME].get())->value = current_proc_info.usr_time;
        
        static_cast<DoubleHealtMetric*>(node_health_set.map_metric[NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME].get())->value = current_proc_info.sys_time;
        //get the uptime in seconds
        static_cast<Int64HealtMetric*>(node_health_set.map_metric[NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME].get())->value = (push_timestamp - node_health_set.startup_time)/1000;
        
        //scan all metrics
        BOOST_FOREACH(HealtNodeElementMap::value_type map_metric_element, node_health_set.map_metric) {
            //add metric to cdata wrapper
            map_metric_element.second->addMetricToCD(node_data_pack);
        }
    }
    return node_data_pack;
}

//publish the healt for the ndoe uid
void HealtManager::publishNodeHealt(const std::string& node_uid) {
    if(map_node.count(node_uid) == 0) return;
    
    //allocate the datapack
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_pack(new CDataWrapper());
    
    // get metric ptr
    _publish(map_node[node_uid], TimingUtil::getTimeStamp());
}

void HealtManager::timeout() {
    uint64_t cur_time = TimingUtil::getTimeStamp();
    //publish all registered metric
    boost::shared_lock<boost::shared_mutex> rl(map_node_mutex);
    for(HealtNodeMapIterator it = map_node.begin();
        it != map_node.end();
        it++) {
        if(it->second->fire_slot == current_fire_slot) {
            // get metric ptr
            _publish(it->second, cur_time);
        }
    }
    //go to next slot
    ++current_fire_slot %= HEALT_FIRE_SLOTS;
}

void HealtManager::_publish(const ChaosSharedPtr<NodeHealtSet>& heath_set,
                            uint64_t publish_ts) {
    //lock the driver for bublishing
    boost::unique_lock<boost::mutex> wl_io(mutex_publishing);
    //update infromation abour process
    updateProcInfo();
    
    //send datapack
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_pack(prepareNodeDataPack(*heath_set,
                                                              publish_ts));
    if(data_pack.get()) {
        //store data on cache
        io_data_driver->storeHealthData(heath_set->node_publish_key,
                                        *data_pack,
                                        DataServiceNodeDefinitionType::DSStorageTypeLive);
    } else {
        HM_ERR << "Error allocating health datapack for node:" << heath_set->node_uid;
    }
}
