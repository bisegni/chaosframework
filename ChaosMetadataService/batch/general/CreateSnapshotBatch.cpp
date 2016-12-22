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

using namespace chaos::common::data;
using namespace chaos::common::network;

using namespace chaos::metadata_service;
using namespace chaos::metadata_service::batch::general;

#define G_RS_INFO INFO_LOG(CreateSnapshotBatch)
#define G_RS_DBG  DBG_LOG(CreateSnapshotBatch)
#define G_RS_ERR  ERR_LOG(CreateSnapshotBatch)

DEFINE_MDS_COMAMND_ALIAS(CreateSnapshotBatch)

CreateSnapshotBatch::CreateSnapshotBatch():
MDSBatchCommand(),
curren_node_id(-1){}

CreateSnapshotBatch::~CreateSnapshotBatch() {}

// inherited method
void CreateSnapshotBatch::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    int err = 0;
    
    CHECK_CDW_THROW_AND_LOG(data, G_RS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, "snapshot_name", G_RS_ERR, -2, "The name of the snapshot is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(data->isStringValue("snapshot_name"), G_RS_ERR, -3, "snapshot_name need to be a string type")
    CHECK_KEY_THROW_AND_LOG(data, "node_list", G_RS_ERR, -2, "The name of the snapshot is mandatory")
    CHECK_ASSERTION_THROW_AND_LOG(data->isVectorValue("node_list"), G_RS_ERR, -3, "snapshot_name need to be a vector type")
   
    const std::string snapshot_name = data->getStringValue("snapshot_name");
    std:auto_ptr<CMultiTypeDataArrayWrapper> _node_list(data->getVectorValue("node_list"));
    for(int idx =0;
        idx < _node_list->size();
        idx++) {
        list_node_in_snapshot.push_back(_node_list->getStringElementAtIndex(idx));
    }
}

// inherited method
void CreateSnapshotBatch::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    
}

// inherited method
void CreateSnapshotBatch::ccHandler() {
    MDSBatchCommand::ccHandler();
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



//void SnapshotCreationWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
//    int err = 0;
//    
//    //the set of unique id to snap
//    std::vector<std::string> snapped_producer_keys;
//    SnapshotCreationJob *job_ptr = reinterpret_cast<SnapshotCreationJob*>(job_info);
//    
//    if((err = s_da->snapshotIncrementJobCounter(job_ptr->job_work_code,
//                                                job_ptr->snapshot_name,
//                                                true))) {
//        SCW_LERR_<< "error incrementing the snapshot job counter for " << job_ptr->snapshot_name << " with error: " << err;
//        //delete job memory
//        FREE_JOB(job_ptr)
//        return;
//    }
//    try {
//        
//        
//        //recreate the array of producer key set
//        if(job_ptr->concatenated_unique_id_memory_size) {
//            std::string concatenated_keys((const char*)job_ptr->concatenated_unique_id_memory, job_ptr->concatenated_unique_id_memory_size);
//            //split the concatenated string
//            boost::split( snapped_producer_keys, concatenated_keys, is_any_of(","), token_compress_on );
//        }
//        
//        //get the unique id to snap
//        if(snapped_producer_keys.size()) {
//            SCW_LDBG_ << "make snapshot on the user producer id set";
//        } else {
//            SCW_LERR_ << "Snapshoot need to have target node id";
//            FREE_JOB(job_ptr)
//            return;
//        }
//        
//        //scann all id
//        for (std::vector<std::string>::iterator it = snapped_producer_keys.begin();
//             it != snapped_producer_keys.end();
//             it++) {
//            
//            //snap output channel
//            if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code,
//                                                 job_ptr->snapshot_name,
//                                                 *it,
//                                                 DataPackPrefixID::OUTPUT_DATASET_POSTFIX))) {
//                
//            }
//            //snap input channel
//            if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code,
//                                                 job_ptr->snapshot_name,
//                                                 *it,
//                                                 DataPackPrefixID::INPUT_DATASET_POSTFIX))) {
//                
//            }
//            //snap custom channel
//            if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code,
//                                                 job_ptr->snapshot_name,
//                                                 *it,
//                                                 DataPackPrefixID::CUSTOM_DATASET_POSTFIX))){
//                
//            }
//            //snap system channel
//            if((err = storeDatasetTypeInSnapsnot(job_ptr->job_work_code,
//                                                 job_ptr->snapshot_name,
//                                                 *it,
//                                                 DataPackPrefixID::SYSTEM_DATASET_POSTFIX))) {
//                
//            }
//        }
//        
//    }catch(...) {
//        
//    }
//    
//    if((err = s_da->snapshotIncrementJobCounter(job_ptr->job_work_code,
//                                                job_ptr->snapshot_name,
//                                                false))) {
//        SCW_LERR_<< "error decrementig the snapshot job counter for " << job_ptr->snapshot_name << " with error: " << err;
//    }
//    //delete job memory
//    FREE_JOB(job_ptr)
//}
