/*
 *	CreateSnapshotBatch.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/12/2016 INFN, National Institute of Nuclear Physics
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

#include "CreateSnapshotBatch.h"
#include "../../DriverPoolManager.h"

#include <chaos/common/utility/UUIDUtil.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;

using namespace chaos::metadata_service;
using namespace chaos::metadata_service::batch::general;

#define G_RS_INFO INFO_LOG(CreateSnapshotBatch)
#define G_RS_DBG  DBG_LOG(CreateSnapshotBatch)
#define G_RS_ERR  ERR_LOG(CreateSnapshotBatch)

DEFINE_MDS_COMAMND_ALIAS(CreateSnapshotBatch)

CreateSnapshotBatch::CreateSnapshotBatch():
MDSBatchCommand(),
work_id(UUIDUtil::generateUUID()),
curren_node_id(-1){}

CreateSnapshotBatch::~CreateSnapshotBatch() {}

// inherited method
void CreateSnapshotBatch::setHandler(CDataWrapper *data) {
    int err = 0;
    
    CHECK_CDW_THROW_AND_LOG(data, G_RS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, "snapshot_name", G_RS_ERR, -2, "The name of the snapshot is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(data->isStringValue("snapshot_name"), G_RS_ERR, -3, "snapshot_name need to be a string type")
    CHECK_KEY_THROW_AND_LOG(data, "node_list", G_RS_ERR, -2, "The name of the snapshot is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(data->isVectorValue("node_list"), G_RS_ERR, -3, "snapshot_name need to be a vector type")
    
    //set default scheduler delay 0,5 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)50000);
    
    clearFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT);
    
    snapshot_name = data->getStringValue("snapshot_name");
    std::auto_ptr<CMultiTypeDataArrayWrapper> _node_list(data->getVectorValue("node_list"));
    for(int idx =0;
        idx < _node_list->size();
        idx++) {
        list_node_in_snapshot.push_back(_node_list->getStringElementAtIndex(idx));
    }
    
    if(list_node_in_snapshot.size() == 0) {
        LOG_AND_TROW(G_RS_ERR, -1, "No node as input parameter for snapshot");
    }
    
    
    persistence::data_access::SnapshotDataAccess *s_da = getDataAccess<mds_data_access::SnapshotDataAccess>();
    if((err = s_da->snapshotCreateNewWithName(snapshot_name,
                                              work_id))) {
        LOG_AND_TROW(G_RS_ERR, err, CHAOS_FORMAT("Error creating the snapshot %1% with error %2%",%snapshot_name%err));
    } else if((err = s_da->snapshotIncrementJobCounter(work_id,
                                                       snapshot_name,
                                                       true))) {
        LOG_AND_TROW(G_RS_ERR, err, CHAOS_FORMAT("Error incrementing the snapshot job counter for %1% with error %2%",%snapshot_name%err));
    }
    //set start id for node array
    curren_node_id = 0;
}

// inherited method
void CreateSnapshotBatch::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void CreateSnapshotBatch::ccHandler() {
    MDSBatchCommand::ccHandler();
    CHAOS_ASSERT(curren_node_id < list_node_in_snapshot.size());
    
    //we can work
    storeDatasetForDeviceID(list_node_in_snapshot[curren_node_id++]);
    
    //check if it is the last element
    if(curren_node_id == list_node_in_snapshot.size()){
        int err = 0;
        persistence::data_access::SnapshotDataAccess *s_da = getDataAccess<mds_data_access::SnapshotDataAccess>();
        if((err = s_da->snapshotIncrementJobCounter(work_id,
                                                    snapshot_name,
                                                    false))) {
            LOG_AND_TROW(G_RS_ERR, err, CHAOS_FORMAT("Error decrementing the snapshot job counter for %1% with error %2%",%snapshot_name%err));
        }
        BC_END_RUNNING_PROPERTY;
    }
}

// inherited method
bool CreateSnapshotBatch::timeoutHandler() {
    return MDSBatchCommand::timeoutHandler();
}


int CreateSnapshotBatch::storeDatasetTypeInSnapsnot(const std::string& job_work_code,
                                                    const std::string& snapshot_name,
                                                    const std::string& unique_id,
                                                    const std::string& dataset_type) {
    int err = 0;
    void *data = NULL;
    uint32_t data_len = 0;
    
    //identify the dataaset
    std::string dataset_to_fetch = unique_id + dataset_type;
    
    G_RS_DBG << "Get live data for " << dataset_to_fetch << " in channel";
    persistence::data_access::SnapshotDataAccess *s_da = getDataAccess<mds_data_access::SnapshotDataAccess>();
    data_service::CachePoolSlot *cache_slot = data_service::DriverPoolManager::getInstance()->getCacheDriverInstance();
    try{
        //get data
        if((err = cache_slot->resource_pooled->getData((void*)dataset_to_fetch.c_str(), dataset_to_fetch.size(), &data, data_len))) {
            G_RS_ERR << "Error retrieving live data for " << dataset_to_fetch << " with error: " << err;
        } else if(data) {
            G_RS_DBG << "Store data on snapshot for " << dataset_to_fetch;
            if((err = s_da->snapshotAddElementToSnapshot(job_work_code,
                                                         snapshot_name,
                                                         unique_id,
                                                         dataset_to_fetch,
                                                         data,
                                                         data_len))) {
                G_RS_ERR<< "Error storign dataset type "<< dataset_type <<
                " for " << unique_id <<
                " in snapshot " << snapshot_name <<
                " with error: " << err;
            }
            
            free(data);
        }else {
            err = -1;
            G_RS_ERR<< "No data has been fetched for " << dataset_to_fetch;
        }
    } catch(...) {
        
    }
    data_service::DriverPoolManager::getInstance()->releaseCacheDriverInstance(cache_slot);
    return err;
}

int CreateSnapshotBatch::storeDatasetForDeviceID(const std::string& device_id) {
    int err = 0;
    //snap output channel
    if((err = storeDatasetTypeInSnapsnot(work_id,
                                         snapshot_name,
                                         device_id,
                                         DataPackPrefixID::OUTPUT_DATASET_POSTFIX))) {
        G_RS_ERR << CHAOS_FORMAT("Error %1% snapping dataset %2% for device %3% in snapshot %4% ",%err%DataPackPrefixID::OUTPUT_DATASET_POSTFIX%device_id%snapshot_name);
    }
    //snap input channel
    if((err = storeDatasetTypeInSnapsnot(work_id,
                                         snapshot_name,
                                         device_id,
                                         DataPackPrefixID::INPUT_DATASET_POSTFIX))) {
        G_RS_ERR << CHAOS_FORMAT("Error %1% snapping dataset %2% for device %3% in snapshot %4% ",%err%DataPackPrefixID::INPUT_DATASET_POSTFIX%device_id%snapshot_name);
    }
    //snap custom channel
    if((err = storeDatasetTypeInSnapsnot(work_id,
                                         snapshot_name,
                                         device_id,
                                         DataPackPrefixID::CUSTOM_DATASET_POSTFIX))){
        G_RS_ERR << CHAOS_FORMAT("Error %1% snapping dataset %2% for device %3% in snapshot %4% ",%err%DataPackPrefixID::CUSTOM_DATASET_POSTFIX%device_id%snapshot_name);
    }
    //snap system channel
    if((err = storeDatasetTypeInSnapsnot(work_id,
                                         snapshot_name,
                                         device_id,
                                         DataPackPrefixID::SYSTEM_DATASET_POSTFIX))) {
        G_RS_ERR << CHAOS_FORMAT("Error %1% snapping dataset %2% for device %3% in snapshot %4% ",%err%DataPackPrefixID::SYSTEM_DATASET_POSTFIX%device_id%snapshot_name);
    }
    return err;
}
