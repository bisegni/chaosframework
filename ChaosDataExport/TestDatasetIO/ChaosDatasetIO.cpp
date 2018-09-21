#include "ChaosDatasetIO.h"
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/io/SharedManagedDirecIoDataDriver.h>
#include <chaos_metadata_service_client/api_proxy/unit_server/NewUS.h>
#include <chaos_metadata_service_client/api_proxy/unit_server/ManageCUType.h>
#include <chaos_metadata_service_client/api_proxy/control_unit/SetInstanceDescription.h>
#include <chaos/common/network/NetworkBroker.h>

#define DPD_LOG_HEAD "[ChaosDatasetIO] - "

#define DPD_LAPP LAPP_ << DPD_LOG_HEAD
#define DPD_LDBG LDBG_ << DPD_LOG_HEAD << __PRETTY_FUNCTION__
#define DPD_LERR LERR_ << DPD_LOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") "

#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
using namespace chaos::common::healt_system;
using namespace chaos::metadata_service_client;

#define EXECUTE_CHAOS_API(api_name,time_out,...) \
DPD_LDBG<<" " <<" Executing Api:\""<< # api_name<<"\"" ;\
chaos::metadata_service_client::api_proxy::ApiProxyResult apires=  GET_CHAOS_API_PTR(api_name)->execute( __VA_ARGS__ );\
apires->setTimeout(time_out);\
apires->wait();\
if(apires->getError()){\
std::stringstream ss;\
ss<<" error in :"<<__FUNCTION__<<"|"<<__LINE__<<"|"<< # api_name <<" " <<apires->getErrorMessage();\
DPD_LERR<<ss.str();\
}
namespace driver{
    namespace misc{
        ChaosDatasetIO::ChaosDatasetIO(const std::string &name,
                                       const std::string &group_name):
        datasetName(name),
        groupName(group_name),
        ageing(3600),
        storageType((int)chaos::DataServiceNodeDefinitionType::DSStorageTypeLiveHistory),
        timeo(5000),
        entry_created(false),
        query_index(0),
        defaultPage(30),
        last_seq(0),
        last_push_rate_grap_ts(0),deinitialized(false) {
            InizializableService::initImplementation(chaos::common::io::SharedManagedDirecIoDataDriver::getInstance(), NULL, "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__);
            
            //ioLiveDataDriver =  chaos::metadata_service_client::ChaosMetadataServiceClient::getInstance()->getDataProxyChannelNewInstance();
            ioLiveDataDriver =chaos::common::io::SharedManagedDirecIoDataDriver::getInstance()->getSharedDriver();
            network_broker=NetworkBroker::getInstance();
            mds_message_channel = network_broker->getMetadataserverMessageChannel();
            if(!mds_message_channel)
                throw chaos::CException(-1, "No mds channel found", __PRETTY_FUNCTION__);
            
            
            StartableService::initImplementation(HealtManager::getInstance(), NULL, "HealtManager", __PRETTY_FUNCTION__);
            StartableService::startImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__);
            runid=time(NULL);
            for(int cnt=0;cnt<sizeof(pkids)/sizeof(uint64_t);cnt++){
                pkids[cnt]=0;
            }
            uid=groupName+"/"+datasetName;
            
//            ChaosUniquePtr<char[]> buff(new char [10024]);
//            CDWUniquePtr result_uptr;
//            CDWUniquePtr data_uptr(new CDataWrapper());
//            data_uptr->addStringValue("echo_key", "echo value");
//            data_uptr->addBinaryValue("bin_value", buff.get(), 1024);
//            for(int idx = 0;
//                idx < 100000000;
//                idx++) {
//                int err = mds_message_channel->sendEchoMessage(*data_uptr, result_uptr);
//                if(!err &&
//                   result_uptr) {
//                    DPD_LAPP << result_uptr->getCompliantJSONString();
//                }
//            }
//            DPD_LAPP << "end";
        }
        
        int ChaosDatasetIO::setAgeing(uint64_t secs){ageing=secs;return 0;}
        int ChaosDatasetIO::setStorage(int st){storageType=st;return 0;}
        int ChaosDatasetIO::setTimeo(uint64_t t){timeo=t;return 0;}
        
        ChaosDatasetIO::~ChaosDatasetIO(){
            deinit();
        }
        
        
        void ChaosDatasetIO::updateHealth() {
            uint64_t rate_acq_ts = TimingUtil::getTimeStamp();
            double time_offset = (double(rate_acq_ts - last_push_rate_grap_ts))/1000.0; //time in seconds
            double output_ds_rate = (time_offset>0)?(pkids[chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT]-last_seq)/time_offset:0; //rate in seconds
            
            HealtManager::getInstance()->addNodeMetricValue(uid,
                                                            chaos::ControlUnitHealtDefinitionValue::CU_HEALT_OUTPUT_DATASET_PUSH_RATE,
                                                            output_ds_rate,true);
            
            //keep track of acquire timestamp
            last_push_rate_grap_ts = rate_acq_ts;
            //reset pushe count
            last_seq = pkids[chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT];
        }
        ChaosDataSet ChaosDatasetIO::allocateDataset(int type){
            std::map<int,ChaosDataSet >::iterator i =datasets.find(type);
            if(i == datasets.end()){
                ChaosDataSet tmp(new chaos::common::data::CDataWrapper );
                datasets[type]=tmp;
                DPD_LDBG<<" allocated dataset:"<<type;
                
                return tmp;
            }
            DPD_LDBG<<" returning already allocated dataset:"<<type;
            
            return i->second;
        }
        
        // push a dataset
        int ChaosDatasetIO::pushDataset(int type) {
            int err = 0;
            //ad producer key
            CDWShrdPtr new_dataset = datasets[type];
            uint64_t ts = chaos::common::utility::TimingUtil::getTimeStamp();
            uint64_t tsh = chaos::common::utility::TimingUtil::getTimeStampInMicroseconds();
            
            if(!new_dataset->hasKey((chaos::DataPackCommonKey::DPCK_TIMESTAMP))){
                // add timestamp of the datapack
                new_dataset->addInt64Value(chaos::DataPackCommonKey::DPCK_TIMESTAMP, ts);
            } else {
                new_dataset->setValue(chaos::DataPackCommonKey::DPCK_TIMESTAMP, (int64_t)ts);
                
            }
            if(!new_dataset->hasKey((chaos::DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP))){
                // add timestamp of the datapack
                new_dataset->addInt64Value(chaos::DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP, tsh);
            } else {
                new_dataset->setValue(chaos::DataPackCommonKey::DPCK_HIGH_RESOLUTION_TIMESTAMP, (int64_t)tsh);
                
            }
            if(!new_dataset->hasKey(chaos::DataPackCommonKey::DPCK_SEQ_ID)){
                new_dataset->addInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID,pkids[type]++ );
            } else {
                new_dataset->setValue(chaos::DataPackCommonKey::DPCK_SEQ_ID, (int64_t)pkids[type]++ );
            }
            if(!new_dataset->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID)){
                new_dataset->addInt64Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID,runid);
            }
            if(!new_dataset->hasKey(chaos::DataPackCommonKey::DPCK_DEVICE_ID)){
                new_dataset->addStringValue(chaos::DataPackCommonKey::DPCK_DEVICE_ID, uid);
            }
            if(!new_dataset->hasKey(chaos::DataPackCommonKey::DPCK_DATASET_TYPE)){
                new_dataset->addInt32Value(chaos::DataPackCommonKey::DPCK_DATASET_TYPE, type);
            }
            //ChaosUniquePtr<SerializationBuffer> serialization(new_dataset->getBSONData());
            //    DPD_LDBG <<" PUSHING:"<<new_dataset->getJSONString();
            // DirectIOChannelsInfo    *next_client = static_cast<DirectIOChannelsInfo*>(connection_feeder.getService());
            // serialization->disposeOnDelete = !next_client;
            ioLiveDataDriver->storeData(uid+chaos::datasetTypeToPostfix(type),new_dataset,(chaos::DataServiceNodeDefinitionType::DSStorageType)storageType);
            
            
            return err;
        }
        
        ChaosDataSet ChaosDatasetIO::getDataset(const std::string &dsname,int type){
            size_t dim;
            ChaosDataSet tmp;
            char*ptr=ioLiveDataDriver->retriveRawData(dsname+chaos::datasetTypeToPostfix(type),&dim);
            if(ptr){
                tmp.reset(new chaos::common::data::CDataWrapper(ptr));
                delete[](ptr);
            }
            
            return tmp;
        }
        
        ChaosDataSet ChaosDatasetIO::getDataset(int type){
            size_t dim;
            ChaosDataSet tmp;
            char*ptr=ioLiveDataDriver->retriveRawData(uid+chaos::datasetTypeToPostfix(type),&dim);
            if(ptr){
                tmp.reset(new chaos::common::data::CDataWrapper(ptr));
                delete[](ptr);
            }
            
            return tmp;
        }
        
        chaos::common::data::CDataWrapper ChaosDatasetIO::wrapper2dataset(chaos::common::data::CDataWrapper& dataset_pack,int dir){
            CDataWrapper cu_dataset,mds_registration_pack;
            ChaosStringVector all_template_key;
            dataset_pack.getAllKey(all_template_key);
            
            for(ChaosStringVectorIterator it = all_template_key.begin();
                it != all_template_key.end();
                it++) {
                if(dataset_pack.isNullValue(*it)) {
                    DPD_LERR << "Removed from template null value key:" << *it;
                    continue;
                }
                CDataWrapper ds;
                int32_t dstype=0,subtype=0;
                int32_t size=0;
                ds.addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,*it);
                
                size =dataset_pack.getValueSize(*it);
                if(dataset_pack.isDoubleValue(*it)){
                    dstype |= chaos::DataType::TYPE_DOUBLE;
                    subtype= chaos::DataType::SUB_TYPE_DOUBLE;
                } else if(dataset_pack.isInt64Value(*it)){
                    dstype |=  chaos::DataType::TYPE_INT64;
                    subtype= chaos::DataType::SUB_TYPE_INT64;
                    
                } else if(dataset_pack.isInt32Value(*it)){
                    dstype |=  chaos::DataType::TYPE_INT32;
                    subtype= chaos::DataType::SUB_TYPE_INT32;
                    
                } else if(dataset_pack.isStringValue(*it)){
                    dstype |=  chaos::DataType::TYPE_STRING;
                    subtype= chaos::DataType::SUB_TYPE_STRING;
                    
                } else if(dataset_pack.isBinaryValue(*it)){
                    dstype |=  chaos::DataType::TYPE_BYTEARRAY;
                } else {
                    dstype |= chaos::DataType::TYPE_DOUBLE;
                    subtype= chaos::DataType::SUB_TYPE_DOUBLE;
                    
                }
                if(dataset_pack.isVector(*it)){
                    //dstype = chaos::DataType::TYPE_ACCESS_ARRAY;
                    dstype = chaos::DataType::TYPE_BYTEARRAY;
                }
                ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE,subtype);
                ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE,dstype);
                ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION,dir);
                ds.addInt32Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE,size);
                
                DPD_LDBG<<"- ATTRIBUTE \""<<*it<<"\" SIZE:"<<size<<" TYPE:"<<dstype<<" SUBTYPE:"<<subtype;
                cu_dataset.appendCDataWrapperToArray(ds);
                
            }
            cu_dataset.addInt64Value(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP,chaos::common::utility::TimingUtil::getTimeStamp());
            
            //close array for all device description
            cu_dataset.finalizeArrayForKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
            cu_dataset.addInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID,(int64_t)0);
            
            mds_registration_pack.addCSDataValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, cu_dataset);
            mds_registration_pack.addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, chaos::NodeType::NODE_TYPE_CONTROL_UNIT);
            return mds_registration_pack;
        }
        void ChaosDatasetIO::createMDSEntry(){
            api_proxy::control_unit::SetInstanceDescriptionHelper cud;
            
            {
                EXECUTE_CHAOS_API(api_proxy::unit_server::NewUS,timeo,groupName);
            }
            
            {
                EXECUTE_CHAOS_API(api_proxy::unit_server::ManageCUType,timeo,groupName,"datasetIO",0);
            }
            uid=groupName+"/"+datasetName;
            cud.auto_load=1;
            cud.auto_init=1;
            cud.auto_start=1;
            cud.load_parameter = "";
            cud.control_unit_uid=uid;
            cud.default_schedule_delay=1;
            cud.unit_server_uid=groupName;
            cud.control_unit_implementation="datasetIO";
            cud.history_ageing=ageing;
            cud.storage_type=(chaos::DataServiceNodeDefinitionType::DSStorageType)storageType;
            {
                EXECUTE_CHAOS_API(api_proxy::control_unit::SetInstanceDescription,timeo,cud);
            }
            HealtManager::getInstance()->addNewNode(uid);
            //add push rate metric
            HealtManager::getInstance()->addNodeMetric(uid,
                                                       chaos::ControlUnitHealtDefinitionValue::CU_HEALT_OUTPUT_DATASET_PUSH_RATE,
                                                       chaos::DataType::TYPE_DOUBLE);
            
            HealtManager::getInstance()->addNodeMetricValue(uid,
                                                            chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_START,
                                                            true);
            
        }
        
        //! register the dataset of ap roducer
        int ChaosDatasetIO::registerDataset() {
            CHAOS_ASSERT(mds_message_channel);
            if(datasets.empty()){
                DPD_LERR<<" NO DATASET ALLOCATED";
                
                return -3;
            }
            if(entry_created==false){
                createMDSEntry();
                entry_created=true;
            }
            std::map<int,ChaosSharedPtr<chaos::common::data::CDataWrapper> >::iterator i;
            
            for(i=datasets.begin();i!=datasets.end();i++){
                
                
                CDataWrapper mds_registration_pack=wrapper2dataset(*((i->second).get()),i->first);
                DEBUG_CODE(DPD_LDBG << mds_registration_pack.getJSONString());
                
                int ret;
                
                mds_registration_pack.addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, uid);
                mds_registration_pack.addStringValue(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN, chaos::common::utility::UUIDUtil::generateUUIDLite());
                mds_registration_pack.addStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR, network_broker->getRPCUrl());
                mds_registration_pack.addStringValue("mds_control_key","none");
                mds_registration_pack.addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, chaos::NodeType::NODE_TYPE_CONTROL_UNIT);
                DPD_LDBG<<"registering "<<i->first<<" registration pack:"<<mds_registration_pack.getCompliantJSONString();
                
                if((ret=mds_message_channel->sendNodeRegistration(mds_registration_pack, true, 10000)) ==0){
                    CDataWrapper mdsPack;
                    mdsPack.addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, uid);
                    mdsPack.addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, chaos::NodeType::NODE_TYPE_CONTROL_UNIT);
                    ret = mds_message_channel->sendNodeLoadCompletion(mdsPack, true, 10000);
                    
                    chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this, chaos::common::constants::CUTimersTimeoutinMSec, chaos::common::constants::CUTimersTimeoutinMSec);
                } else {
                    DPD_LERR<<" cannot register dataset "<<i->first<<" registration pack:"<<mds_registration_pack.getCompliantJSONString();
                    return -1;
                }
            }
            return 0;
        }
        uint64_t ChaosDatasetIO::queryHistoryDatasets(uint64_t ms_start,uint64_t ms_end,uint32_t page,int type){
            return queryHistoryDatasets(uid,ms_start,ms_end,page,type);
        }
        
        uint64_t ChaosDatasetIO::queryHistoryDatasets(const std::string &dsname, uint64_t ms_start,uint64_t ms_end,uint32_t page,int type){
            std::string dst=dsname+chaos::datasetTypeToPostfix(type);
            
            DPD_LDBG<<"Query To:"<<dst<<" start:"<< ms_start<<" end_ms:"<<ms_end<<" page:"<<page;
            
            chaos::common::io::QueryCursor *pnt=ioLiveDataDriver->performQuery(dst,ms_start,ms_end,page);
            if(pnt==NULL){
                DPD_LERR<<"NO CURSOR";
                
                return 0;
            }
            query_index++;
            qc_t q;
            q.page_len=page;
            q.qc=pnt;
            q.qt=query_index;
            query_cursor_map[query_index]=q;
            return query_index;
        }
        bool ChaosDatasetIO::queryHasNext(uint64_t uid){
            bool ret;
            query_cursor_map_t::iterator i=query_cursor_map.find(uid);
            if(i==query_cursor_map.end()){
                DPD_LERR<<"query ID:"<<uid<<" not exists";
                return false;
            }
            chaos::common::io::QueryCursor *pnt=(i->second).qc;
            ret=pnt->hasNext();
            DPD_LDBG<<"query ID:"<<uid<<" (0xx"<<std::hex<<pnt<<") has next: "<<std::dec<<ret;
            
            return ret;
            
        }
        std::vector<ChaosDataSet> ChaosDatasetIO::getNextPage(uint64_t uid){
            std::vector<ChaosDataSet> ret;
            query_cursor_map_t::iterator i=query_cursor_map.find(uid);
            if(i==query_cursor_map.end()){
                DPD_LERR<<"query ID:"<<uid<<" not exists";
                return ret;
            }
            chaos::common::io::QueryCursor *pnt=(i->second).qc;
            uint32_t len=(i->second).page_len;
            int cnt=0;
            DPD_LDBG<<"query ID:"<<uid<<" page len "<<pnt->getPageLen()<<" application page:"<<len;
            
            while(pnt->hasNext() && (cnt<len)){
                ChaosDataSet q_result(pnt->next());
                
                ret.push_back(q_result);
                cnt++;
            }
            
            if(pnt->hasNext()==false){
                DPD_LDBG<<"query ID:"<<uid<<" ENDED freeing resource";
                ioLiveDataDriver->releaseQuery( pnt);
                query_cursor_map.erase(i);
            }
            return ret;
        }
        std::vector<ChaosDataSet> ChaosDatasetIO::queryHistoryDatasets(const std::string &dsname, uint64_t ms_start,uint64_t ms_end,int type){
            std::vector<ChaosDataSet> ret;
            std::string dst=dsname+chaos::datasetTypeToPostfix(type);
            DPD_LDBG<<"Query To:"<<dst<<" start:"<< ms_start<<" end_ms:"<<ms_end<<"default page:"<<defaultPage;
            
            chaos::common::io::QueryCursor *pnt=ioLiveDataDriver->performQuery(dst,ms_start,ms_end,defaultPage);
            if(pnt==NULL){
                DPD_LERR<<"NO CURSOR";
                return ret;
            }
            while(pnt->hasNext() ){
                ChaosDataSet q_result(pnt->next());
                //   std::cout<<"Retrieve: "<<q_result->getCompliantJSONString()<<std::endl;
                ret.push_back(q_result);
            }
            ioLiveDataDriver->releaseQuery(pnt);
            return ret;
        }
        std::vector<ChaosDataSet> ChaosDatasetIO::queryHistoryDatasets(uint64_t ms_start,uint64_t ms_end,int type){
            return queryHistoryDatasets(uid,ms_start,ms_end,type);
            
        }
        void ChaosDatasetIO::timeout() {
            //update push metric
            updateHealth();
        }
        void ChaosDatasetIO::deinit(){
            
            if(deinitialized){
                DEBUG_CODE(DPD_LDBG << "Already deinitialized");
                return;
            }
            HealtManager::getInstance()->addNodeMetricValue(uid,
                                                            chaos::NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            chaos::NodeHealtDefinitionValue::NODE_HEALT_STATUS_DEINIT,
                                                            true);
            
            for( std::map<int,ChaosSharedPtr<chaos::common::data::CDataWrapper> >::iterator i=datasets.begin();i!=datasets.end();i++){
                DPD_LDBG<<" removing dataset:"<<i->first;
                (i->second).reset();
            }
            
            for(query_cursor_map_t::iterator i=query_cursor_map.begin();i!=query_cursor_map.end();){
                DPD_LDBG<<" removing query ID:"<<i->first;
                
                ioLiveDataDriver->releaseQuery( (i->second).qc);
                
                query_cursor_map.erase(i++);
            }
            
            DEBUG_CODE(DPD_LDBG << "Shared Manager deinitialized");
            
            DEBUG_CODE(DPD_LDBG << "Deinitialized");
            
            deinitialized=true;
            DEBUG_CODE(DPD_LDBG << "Destroy all resources");
            chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this);
            DEBUG_CODE(DPD_LDBG << "Timer removed");
            
            CHAOS_NOT_THROW(StartableService::stopImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
            DEBUG_CODE(DPD_LDBG << "Health stopped");
            
            CHAOS_NOT_THROW(StartableService::deinitImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
            DEBUG_CODE(DPD_LDBG << "Health deinitialized");
            
            CHAOS_NOT_THROW(InizializableService::deinitImplementation(chaos::common::io::SharedManagedDirecIoDataDriver::getInstance(), "SharedManagedDirecIoDataDriver", __PRETTY_FUNCTION__););
        }
    }
}
